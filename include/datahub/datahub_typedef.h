#ifndef DATAHUB_DATAHUB_TYPEDEF_H
#define DATAHUB_DATAHUB_TYPEDEF_H

#include <map>
#include <vector>
#include <memory>
#include <string>

namespace aliyun
{
namespace datahub
{

typedef std::shared_ptr<std::string> StringPtr;
typedef std::shared_ptr<std::map<std::string, std::string> > MapPtr;

typedef std::vector<std::string> StringVec;
typedef std::map<std::string, std::string> StringMap;

typedef std::pair<std::string, std::string> StringPair;
typedef std::vector<StringPair > StringPairVec;

}
}
#endif //DATAHUB_DATAHUB_TYPEDEF_H
