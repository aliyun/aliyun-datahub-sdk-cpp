#include "protobuf_parser.h"
#include "crcutils.h"
#include "utils.h"

namespace aliyun
{
namespace datahub
{

bool ProtobufParser::ParsePbFromArrayBuff(google::protobuf::Message* message,
        const char* buff, int64_t size, std::string& errMsg)
{
    uint32_t headerSize = sizeof(PackHeader);
    if (size < headerSize)
    {
        errMsg = "pb data header size is less than 12";
        return false;
    }

    PackHeader* header = (PackHeader*)buff;
    if (!(*header).FormatAndCheck(size, errMsg))
    {
        return false;
    }

    if ((*header).crc != 0)
    {
        uint32_t crcRet = DoCrc(buff + headerSize, (*header).dataLen);
        if ((*header).crc != crcRet)
        {
            errMsg = "crc check error. crc:" + std::to_string((*header).crc) + ", compute crc:" + std::to_string(crcRet);
            return false;
        }
    }

    return ParsePbFromRawData(message, buff + headerSize, (*header).dataLen);
}

bool ProtobufParser::ParsePbFromRawData(google::protobuf::Message* message, const char* buff, int64_t size)
{
    ArrayInputStream input(buff, size);
    google::protobuf::io::CodedInputStream codedInput(&input);
    codedInput.SetTotalBytesLimit(INT_MAX, -1);
    return message->ParseFromCodedStream(&codedInput) && codedInput.ConsumedEntireMessage();
}

void ProtobufParser::ParsePbToString(const google::protobuf::Message& message, std::string& out)
{
    uint32_t headerSize = sizeof(PackHeader);
    uint32_t byteSize = message.ByteSize();
    out.resize(byteSize + headerSize);

    char* buff = &out[0];
    ParsePbToRawData(message, buff + headerSize, byteSize);
    uint32_t crc = DoCrc(buff + headerSize, byteSize);
    *((PackHeader*)buff) = PackHeader(crc, byteSize);;
}

void ProtobufParser::ParsePbToRawData(const google::protobuf::Message& message, char* buff, int64_t size)
{
    google::protobuf::io::ArrayOutputStream output(buff, size);
    google::protobuf::io::CodedOutputStream codedOutput(&output);
    message.SerializeWithCachedSizes(&codedOutput);
}

uint32_t ProtobufParser::DoCrc(const char* buff, int64_t size)
{
    uint32_t crc = CrcUtil::DoCrc32c(0xFFFFFFFF, (const uint8_t*)buff,
            (uint64_t)size);
    return crc ^ 0xFFFFFFFF;
}

PackHeader::PackHeader(uint32_t vCrc, uint32_t vLen)
{
    *((uint32_t*)magicNumber) = *((uint32_t*)"DHUB");
    crc = ntohl(vCrc);
    dataLen = ntohl(vLen);
}

bool PackHeader::FormatAndCheck(uint32_t totalLen, std::string& errMsg)
{
    if (*((const uint32_t*)magicNumber) != *(const uint32_t*)"DHUB")
    {
        errMsg = "pb data header magic number is not DHUB";
        return false;
    }

    crc = htonl(crc);
    dataLen = htonl(dataLen);
    if (dataLen + sizeof(PackHeader)!= totalLen)
    {
        errMsg = "pb data size not match with header";
        return false;
    }

    return true;
}

} // namespace dathub
} // namespace aliyun
