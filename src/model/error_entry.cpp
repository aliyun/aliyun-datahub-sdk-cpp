#include "rapidjson/document.h"
#include "datahub/datahub_result.h"

namespace aliyun
{
namespace datahub
{

ErrorEntry::ErrorEntry() :
    mIndex(-1)
{
}

ErrorEntry::~ErrorEntry()
{
}

int ErrorEntry::GetIndex() const
{
    return mIndex;
}

std::string ErrorEntry::GetErrorCode() const
{
    return mErrorCode;
}

std::string ErrorEntry::GetErrorMessage() const
{
    return mErrorMessage;
}

std::string ErrorEntry::GetErrorDetail() const
{
    return mErrorDetail;
}

} // namespace datahub
} // namespace aliyun
