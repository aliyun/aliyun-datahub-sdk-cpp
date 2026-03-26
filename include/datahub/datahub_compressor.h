#ifndef DATAHUB_SDK_XSTREAM_COMPRESSOR_H
#define DATAHUB_SDK_XSTREAM_COMPRESSOR_H

#include <memory>
#include <string>
#include <map>
#include <vector>

namespace aliyun {
namespace datahub {
namespace compress {

const std::string COMPRESS_METHOD_LZ4 = "lz4";
const std::string COMPRESS_METHOD_ZLIB = "zlib";
const std::string COMPRESS_METHOD_DEFLATE = "deflate";
const int COMPRESS_DEFAULT_LEVEL = 6; //deflate, default: 6

enum CompressMethod
{
    NONE = 0,
    LZ4 = 1,
    ZLIB = 2,
    DEFLATE = 3
};

std::string GetNameOfCompressMethod(const CompressMethod& compressMethod);

CompressMethod GetCompressMethodByName(const std::string& name);

class Compressor
{
public:
    Compressor(void) {};
    virtual ~Compressor(void) {}

    std::string Compress(const std::string& data, int level = COMPRESS_DEFAULT_LEVEL);
    std::string Decompress(const std::string& data, int destSize);

    virtual std::string Compress(const char* src, int size, int level = COMPRESS_DEFAULT_LEVEL) = 0;
    virtual std::string Decompress(const char* src, int size, int destSize) = 0;
    virtual CompressMethod GetCompressMethod(void) const = 0;
};

class Lz4Compressor : public Compressor
{
public:
    Lz4Compressor(void) {}
    ~Lz4Compressor(void) {}

    virtual std::string Compress(const char* src, int size, int level);
    virtual std::string Decompress(const char* src, int size, int destSize);
    virtual CompressMethod GetCompressMethod(void) const { return LZ4; }
};

class ZlibCompressor : public Compressor
{
public:
    ZlibCompressor(void) {}
    ~ZlibCompressor(void) {}

    virtual std::string Compress(const char* src, int size, int level);
    virtual std::string Decompress(const char* src, int size, int destSize);
    virtual CompressMethod GetCompressMethod(void) const { return ZLIB; }

private:
    int CompressBound(int sourceLen);
};

class DeflateCompressor : public Compressor
{
public:
    DeflateCompressor(void) {}
    ~DeflateCompressor(void) {}

    virtual std::string Compress(const char* src, int size, int level);
    virtual std::string Decompress(const char* src, int size, int destSize);
    virtual CompressMethod GetCompressMethod(void) const { return DEFLATE; }
};

//for thread safe, compressor should not have member variable which is not read only
class CompressorFactory
{
public:
    CompressorFactory(void);
    static Compressor* GetCompressor(const CompressMethod& compressMethod, bool exception = false);

private:
    typedef std::shared_ptr<Compressor> CompressorPtr;
    typedef std::map<CompressMethod, CompressorPtr> CompressorMap;
    CompressorMap mCompressorMap;
};

} // namespace compress
} // namespace datahub
} // namespace aliyun

#endif
