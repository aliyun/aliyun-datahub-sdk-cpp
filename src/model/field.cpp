#include <string>
#include "datahub/datahub_record.h"

namespace aliyun
{
namespace datahub
{

Field::Field(const std::string& name, const FieldType& type, bool allowNull, const std::string& comment) :
    mAllowNull(allowNull),
    mName(name),
    mType(type),
    mComment(comment)
{
}

const std::string& Field::GetFieldName() const
{
    return mName;
}

const std::string& Field::GetFieldComment() const
{
    return mComment;
}

FieldType Field::GetFieldType() const
{
    return mType;
}

bool Field::IsAllowNull() const
{
    return mAllowNull;
}

bool Field::operator == (const Field& rhs) const
{
    return rhs.mName == mName && rhs.mType == mType && rhs.mAllowNull == mAllowNull && rhs.mComment == mComment;
}

} // namespace datahub
} // namespace aliyun

