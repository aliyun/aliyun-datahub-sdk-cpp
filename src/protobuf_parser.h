#ifndef  DATAHUB_SDK_PROTOBUF_PARSER_H
#define  DATAHUB_SDK_PROTOBUF_PARSER_H

#include <netinet/in.h>
#include <memory>
#include "google/protobuf/message.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

namespace aliyun
{
namespace datahub
{

typedef google::protobuf::io::ArrayInputStream ArrayInputStream;
typedef std::shared_ptr<ArrayInputStream> ArrayInputStreamPtr;
class ProtobufParser
{
public:
    static bool ParsePbFromArrayBuff(google::protobuf::Message* message,
            const char* buff, int64_t size, std::string& errMsg);
    static bool ParsePbFromRawData(google::protobuf::Message* message, const char* buff, int64_t size);
    static void ParsePbToString(const google::protobuf::Message& message, std::string& out);
    static void ParsePbToRawData(const google::protobuf::Message& message, char* buff, int64_t size);
    static uint32_t DoCrc(const char* buff, int64_t size);
};

#pragma pack(1)
struct PackHeader
{
    PackHeader(uint32_t vCrc, uint32_t vLen);
    bool FormatAndCheck(uint32_t totalLen, std::string& errMsg);

    char magicNumber[4];
    uint32_t crc;
    uint32_t dataLen;
};
#pragma pack()

} // namespace datahub
} // namespace aliyun

#endif // DATAHUB_SDK_PROTOBUF_PARSER_H

