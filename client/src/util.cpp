#include "util.h"

namespace aliyun
{
namespace datahub
{

std::string PrintUtil::GetMsg(const std::set<std::string>& srcSet)
{
    std::string ret;
    if (srcSet.empty())
    {
        return ret;
    }

    size_t len = 0;
    for (auto it = srcSet.begin(); it != srcSet.end(); it++)
    {
        len += (it->size() + 1);
    }
    ret.reserve(len);
    for (auto it = srcSet.begin(); it != srcSet.end(); it++)
    {
        ret.append(*it).append(";");
    }
    return ret;
}

std::string PrintUtil::GetMsg(const StringVec& srcVec)
{
    std::string ret;
    if (srcVec.empty())
    {
        return ret;
    }

    size_t len = 0;
    for (auto it = srcVec.begin(); it != srcVec.end(); it++)
    {
        len += (it->size() + 1);
    }
    ret.reserve(len);
    for (auto it = srcVec.begin(); it != srcVec.end(); it++)
    {
        ret.append(*it).append(";");
    }
    return ret;
}

std::string PrintUtil::GetMsg(const std::map<std::string, int64_t>& srcMap)
{
    std::string ret;
    if (srcMap.empty())
    {
        return ret;
    }

    size_t len = 0;
    for (auto it = srcMap.begin(); it != srcMap.end(); it++)
    {
        len += (it->first.size() + std::to_string(it->second).size() + 3);
    }
    ret.reserve(len);
    for (auto it = srcMap.begin(); it != srcMap.end(); it++)
    {
        ret.append(it->first).append("->").append(std::to_string(it->second)).append(";");
    }
    return ret;
}

std::string PrintUtil::GetMsg(const std::map<std::string, OffsetMeta>& srcMap)
{
    std::string ret;
    if (srcMap.empty())
    {
        return ret;
    }

    size_t len = 0;
    for (auto it = srcMap.begin(); it != srcMap.end(); it++)
    {
        len += (it->first.size() + 1);
    }
    ret.reserve(len);
    for (auto it = srcMap.begin(); it != srcMap.end(); it++)
    {
        ret.append(it->first).append(";");
    }
    return ret;
}

void GenUtil::GenKey(std::string& tar, const StringVec& src)
{
    auto it = src.begin();
    if (it != src.end())
    {
        tar.append(*it);
    }
    while (++it != src.end())
    {
        tar.append(":").append(*it);
    }
}

} // namespace datahub
} // namespace aliyun
