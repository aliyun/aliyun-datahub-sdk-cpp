#ifndef DATAHUB_SDK_DATAHUB_CONFIG_H
#define DATAHUB_SDK_DATAHUB_CONFIG_H
#include <string>
#include "datahub/datahub_compressor.h"

namespace aliyun
{
namespace datahub
{

static const std::string& DATAHUB_CLIENT = "DATAHUB-CLIENT-CPP";
static const std::string& DATAHUB_SDK = "DATAHUB-SDK-CPP";
static const std::string& CLIENT_VERSION = "1.1";

struct Account
{
    std::string id;
    std::string key;
    std::string token;

    Account()
    {
    }

    Account(const std::string& id, const std::string& key) :
        id(id),
        key(key)
    {
    }

    Account(const std::string& id, const std::string& key, const std::string& token) :
        id(id),
        key(key),
        token(token)
    {
    }
};

struct Configuration
{
public:
    /**
     * Client configuration
     * @param account AliyunAccount
     * @param endpoint DatahubEndpoint
     */
    Configuration(const Account& account, const std::string& endpoint, bool enableProtobuf = false, const compress::CompressMethod& compress = compress::LZ4);
    virtual ~Configuration() {}

    const Account& GetAccount() const { return mAccount; }
    void SetAccount(const Account& account) { mAccount = account; }

    std::string GetEndpoint() const { return mEndpoint; }
    void SetEndpoint(const std::string& endpoint) { mEndpoint = endpoint; }

    compress::CompressMethod GetCompress() const { return mCompress; }
    void SetCompress(const compress::CompressMethod& compress) { mCompress = compress; }

    bool GetEnableProtobuf() const { return mEnableProtobuf; }
    void SetEnableProtobuf(bool enableProtobuf) { mEnableProtobuf = enableProtobuf; }

    unsigned GetMaxPoolSize() const { return mMaxPoolSize; }
    void SetMaxPoolSize(unsigned maxPoolSize) { mMaxPoolSize = maxPoolSize; }

    long GetRequestTimeout() const {return mRequestTimeout; }
    void SetRequestTimeout(long requestTimeout) { mRequestTimeout = requestTimeout; }

    long GetConnectTimeout() const { return mConnectTimeout; }
    void SetConnectTimeout(long connectTimeout) { mConnectTimeout = connectTimeout; }

    int GetCompressLevel() const { return mCompressLevel; }
    void SetCompressLevel(int level) { mCompressLevel = level; }

    std::string GetRegion() const { return mRegion; }
    void SetRegion(const std::string& region) { mRegion = region; }

    void SetA4Signing(bool enable) { mEnableA4Signing = enable; }
    bool IsA4Signing() const { return mEnableA4Signing; }

    void SetUserInfo(const std::string& userInfo) { mUserInfo = userInfo; }
    const std::string& GetUserInfo() const { return mUserInfo; }

protected:
    bool mEnableProtobuf;
    bool mEnableA4Signing;
    compress::CompressMethod mCompress;
    unsigned mCompressLevel;
    unsigned mMaxPoolSize;
    long mRequestTimeout;
    long mConnectTimeout;
    std::string mUserInfo;
    std::string mEndpoint;
    std::string mRegion;
    std::string mSigningKey;
    Account mAccount;
};

} // namespace datahub
} // namespace aliyun

#endif
