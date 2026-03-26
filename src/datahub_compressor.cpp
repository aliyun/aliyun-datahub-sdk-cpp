#include "lz4.h"
#include "lz4frame.h"
#include "datahub/datahub_compressor.h"
#include "datahub/datahub_exception.h"
#include <zlib.h>

namespace aliyun {
namespace datahub {
namespace compress {

#define DEFAULT_BLOCK (16*1024)

std::string GetNameOfCompressMethod(const CompressMethod& compressMethod)
{
    switch(compressMethod)
    {
        case LZ4:
            return COMPRESS_METHOD_LZ4;
        case ZLIB:
            return COMPRESS_METHOD_ZLIB;
        case DEFLATE:
            return COMPRESS_METHOD_DEFLATE;
        case NONE:
            return "";
    }
    throw DatahubException(LOCAL_ERROR_CODE, "unknown compress method");
}

CompressMethod GetCompressMethodByName(const std::string& name)
{
    if (name == COMPRESS_METHOD_LZ4)
    {
        return LZ4;
    }
    else if (name == COMPRESS_METHOD_ZLIB)
    {
        return ZLIB;
    }
    else if (name == COMPRESS_METHOD_DEFLATE)
    {
        return DEFLATE;
    }
    else if (name == "")
    {
        return NONE;
    }
    throw DatahubException(LOCAL_ERROR_CODE, "unsupported compress method");
}

std::string Compressor::Compress(const std::string& data, int level)
{
    return Compress(data.c_str(), (int)data.size(), level);
}

std::string Compressor::Decompress(const std::string& data, int destSize)
{
    return Decompress(data.c_str(), (int)data.size(), destSize);
}

std::string Lz4Compressor::Compress(const char* src, int size, int level)
{
    int bound = LZ4_compressBound(size);
    std::string dest(bound, 0);
    int ret = LZ4_compress_default(src, &dest[0], size, bound);
    if (ret <= 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Compress failed");
    }

    dest.resize(ret);
    return dest;
}

std::string Lz4Compressor::Decompress(const char* src, int size, int destSize)
{
    if (destSize <= 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid hint size");
    }

    std::string dest(destSize, 0);
    int ret = LZ4_decompress_safe(src, &dest[0], size, destSize);
    if (ret <= 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Decompression failed");
    }

    dest.resize(ret);
    return dest;
}

int ZlibCompressor::CompressBound(int sourceLen)
{
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) + (sourceLen >> 25) + 13;
}

std::string ZlibCompressor::Compress(const char* src, int size, int level)
{
    int bound = CompressBound(size);
    std::string dest(bound, 0);
    uLongf olen = dest.size();
    uLong  ilen = size;
    int ret = compress2((Bytef*)dest.c_str(), &olen, (const Bytef*)src, ilen, level);
    if (ret != Z_OK)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Compress failed");
    }

    dest.resize(olen);
    return dest;
}

std::string ZlibCompressor::Decompress(const char* src, int size, int destSize)
{
    if (destSize <= 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid hint size");
    }

    std::string dest(destSize, 0);
    uLongf olen = dest.size();
    uLong  ilen = size;
    int ret = uncompress((Bytef*)dest.c_str(), &olen, (const Bytef*)src, ilen);
    if (ret != Z_OK)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Decompression failed");
    }

    dest.resize(olen);
    return dest;
}

std::string DeflateCompressor::Compress(const char* src, int size, int level)
{
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = deflateInit(&strm, level);
    if (ret != Z_OK)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Compress failed");
    }

    std::vector<char> buf(DEFAULT_BLOCK, 0);
    strm.avail_in = size;
    strm.next_in = (unsigned char*)src;

    std::string dest;
    bool bk = false;
    do {
        strm.avail_out = buf.size();
        strm.next_out = (unsigned char*)buf.data();

        ret = deflate(&strm, Z_FINISH);
        if (ret == Z_STREAM_ERROR)
        {
            bk = true;
            break;
        }

        dest.append(buf.data(), buf.size() - strm.avail_out);
    } while (strm.avail_out == 0 || ret != Z_STREAM_END);

    ret = deflateEnd(&strm);
    if (bk || ret != Z_OK)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Compress failed");
    }

    return dest;
}

std::string DeflateCompressor::Decompress(const char* src, int size, int destSize)
{
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    int ret = inflateInit(&strm);
    if (ret != Z_OK)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Decompress failed");
    }

    std::vector<char> buf(DEFAULT_BLOCK, 0);
    strm.avail_in = size;
    strm.next_in = (unsigned char*)src;

    bool bk = false;
    std::string dest;
    do {
        strm.avail_out = buf.size();
        strm.next_out = (unsigned char*)buf.data();

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR ||
            ret == Z_NEED_DICT ||
            ret == Z_DATA_ERROR ||
            ret == Z_MEM_ERROR ||
            ret == Z_BUF_ERROR)
        {
            bk = true;
            break;
        }

        dest.append(buf.data(), buf.size() - strm.avail_out);
    } while (strm.avail_out == 0 || ret != Z_STREAM_END);

    ret = inflateEnd(&strm);
    if (bk || ret != Z_OK)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Decompress failed");
    }

    return dest;
}

CompressorFactory::CompressorFactory(void)
{
    CompressorPtr compressors[] = {
        CompressorPtr(new Lz4Compressor()),
        CompressorPtr(new ZlibCompressor()),
        CompressorPtr(new DeflateCompressor())
    };

    for (size_t i = 0; i < sizeof(compressors) / sizeof(CompressorPtr); i++)
    {
        mCompressorMap[compressors[i]->GetCompressMethod()] = compressors[i];
    }
}

Compressor* CompressorFactory::GetCompressor(const CompressMethod& compressMethod, bool exception)
{
    static CompressorFactory factory;
    CompressorMap::iterator it = factory.mCompressorMap.find(compressMethod);

    if (it == factory.mCompressorMap.end() && exception)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid compressor type :" + GetNameOfCompressMethod(compressMethod));
    }

    return it != factory.mCompressorMap.end() ? (*it).second.get() : NULL;
}

} // namespace compress
} // namespace datahub
} // namespace aliyun
