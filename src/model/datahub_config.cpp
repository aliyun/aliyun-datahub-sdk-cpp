#include "datahub/datahub_config.h"

namespace aliyun
{
namespace datahub
{

static const unsigned DEFAULT_MAX_POOL_SIZE = 10;
static const long DEFAULT_REQUEST_TIMEOUT = 30000;
static const long DEFAULT_CONNECT_TIMEOUT = 10000;

Configuration::Configuration(const Account& account, const std::string& endpoint,
    bool enableProtobuf, const compress::CompressMethod& compress) :
    mEnableProtobuf(enableProtobuf),
    mEnableA4Signing(false),
    mCompress(compress),
    mCompressLevel(compress::COMPRESS_DEFAULT_LEVEL),
    mMaxPoolSize(DEFAULT_MAX_POOL_SIZE),
    mRequestTimeout(DEFAULT_REQUEST_TIMEOUT),
    mConnectTimeout(DEFAULT_CONNECT_TIMEOUT),
    mUserInfo(DATAHUB_SDK),
    mEndpoint(endpoint),
    mAccount(account)
{
    auto start = mEndpoint.find("dh-");
    if (start != std::string::npos)
    {
        auto end = mEndpoint.find("-int-vpc", start + 3);
        mRegion = mEndpoint.substr(start + 3, end != std::string::npos ? end - start : mEndpoint.find('.', start + 3) - start);
    }
}

} // namespace datahub
} // namespace aliyun
