#include <string>

#include "datahub_client_impl.h"
#include "datahub/datahub_result.h"

namespace aliyun
{
namespace datahub
{

static const int MIN_HTTP_ERROR_CODE = 400;

DatahubClientImpl::DatahubClientImpl()
{
    memset(&mReadMetric, 0, sizeof(mReadMetric));
    memset(&mWriteMetric, 0, sizeof(mWriteMetric));
}

DatahubClientImpl::~DatahubClientImpl()
{
}

bool DatahubClientImpl::IsResponseOK(const http::HttpResponse& response)
{
    int responseCode = static_cast<int>(response.GetResponseCode());
    return responseCode < MIN_HTTP_ERROR_CODE;
}

DatahubMetric DatahubClientImpl::GetReadMetric() const
{
    DatahubMetric metric = {
        mReadMetric.records,
        mReadMetric.requests,
        mReadMetric.rawBytes,
        mReadMetric.transBytes
    };

    return metric;
}

DatahubMetric DatahubClientImpl::GetWriteMetric() const
{
    DatahubMetric metric = {
        mWriteMetric.records,
        mWriteMetric.requests,
        mWriteMetric.rawBytes,
        mWriteMetric.transBytes
    };

    return metric;
}

} // namespace datahub
} // namespace aliyun
