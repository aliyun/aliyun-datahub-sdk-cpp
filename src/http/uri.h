#ifndef DATAHUB_SDK_URI_H
#define DATAHUB_SDK_URI_H

#include <string>
#include <http/scheme.h>
#include <cstdint>

namespace aliyun
{
namespace datahub
{
namespace http
{
class URI
{
public:
    explicit URI(const std::string& endpoint);
    ~URI();

    void SetPath(const std::string& path);

    const std::string& GetPath() const;

    std::string GetURIString() const;

private:
    std::string mEndpoint;
    std::string mPath;
};

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
