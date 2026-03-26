#ifndef INCLUDE_DATAHUB_CLIENT_UTIL_H
#define INCLUDE_DATAHUB_CLIENT_UTIL_H

#include "offset_meta.h"
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_subscription_offset.h"
#include <set>
#include <string>


namespace aliyun
{
namespace datahub
{

class PrintUtil
{
public:
    static std::string GetMsg(const std::set<std::string>& srcSet);

    static std::string GetMsg(const StringVec& srcVec);

    static std::string GetMsg(const std::map<std::string, int64_t>& srcMap);

    static std::string GetMsg(const std::map<std::string, OffsetMeta>& srcMap);
};

class GenUtil
{
public:
    static void GenKey(std::string& tar, const StringVec& src);
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_UTIL_H
