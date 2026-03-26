//-------------------------------------
//file: crc32c.h
//date: 2015-04-24
//describe: crc32c function
//-------------------------------------

#include <cstdint>

#ifndef DATAHUB_SDK_CRCUTIL_H
#define DATAHUB_SDK_CRCUTIL_H

namespace aliyun
{
namespace datahub
{

typedef uint32_t (*CrcHdr)(uint32_t crc, const uint8_t* data, uint64_t len);

class CrcUtil
{
public:
    CrcUtil(void);
    ~CrcUtil(void);

    static void InstallHdr(CrcHdr handler);
    static uint32_t DoCrc32c(uint32_t crc, const uint8_t* data, uint64_t len);
    static uint32_t DoCrc(uint32_t crc, uint8_t* dst, const uint8_t* data, uint64_t len);
    static CrcUtil& GetInstance(void);

public:
    void  SetProfile(bool profile) { mSseMode = (DoProbe() && profile); }

private:
    bool     DoProbe(void);
    uint32_t Process(uint32_t crc, const uint8_t* data, uint64_t len);
    uint32_t Process(uint32_t crc, uint8_t* dst, const uint8_t* src, uint64_t len);
    uint32_t CrcAccel(uint32_t crc, uint8_t* dst, const uint8_t* src, uint64_t len);
    uint32_t CrcAccel(uint32_t crc, const uint8_t* data, uint64_t len);
    uint32_t CrcGeneric(uint32_t crc, uint8_t* dst, const uint8_t* src, uint64_t len);
    uint32_t CrcGeneric(uint32_t crc, const uint8_t* data, uint64_t len);

private:
    bool   mSseMode;
    CrcHdr mHandler;
};

} // namespace datahub
} // namespace aliyun
#endif
