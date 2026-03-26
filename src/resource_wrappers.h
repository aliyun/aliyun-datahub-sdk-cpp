#ifndef DATAHUB_SDK_RESOURCE_WRAPPERS_H
#define DATAHUB_SDK_RESOURCE_WRAPPERS_H

#include "curl/curl.h"

namespace aliyun
{
namespace datahub
{

class CurlSListWrapper
{
public:
    explicit CurlSListWrapper(struct curl_slist* slist) : mList(slist)
    {}

    ~CurlSListWrapper()
    {
        if (mList)
        {
            curl_slist_free_all(mList);
        }
    }

    struct curl_slist* GetList() { return mList; }
private:
    struct curl_slist* mList;
};

} // namespace datahub
} // namespace aliyun
#endif
