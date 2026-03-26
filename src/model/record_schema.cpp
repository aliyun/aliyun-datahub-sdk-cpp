#include <map>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

RecordSchema::RecordSchema()
{
}

void RecordSchema::AddField(const Field& field)
{
    if (mNameMap.find(field.GetFieldName()) != mNameMap.end())
    {
        return;
    }

    mNameMap[field.GetFieldName()] = mFields.size();

    mFields.push_back(field);
}

int RecordSchema::GetFieldCount() const
{
    return (int)mFields.size();
}

int RecordSchema::GetFieldIndex(const std::string& name) const
{
    std::map<std::string, int>::const_iterator it = mNameMap.find(name);
    if (it != mNameMap.end())
    {
        return (*it).second;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Field name don't exist: " + name);
    }
}

const Field& RecordSchema::GetField(int index) const
{
    if (index < 0 || index >= GetFieldCount())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid schema index: " + std::to_string(index));
    }
    else
    {
        return mFields[index];
    }
}

const std::vector<Field>& RecordSchema::GetFields() const
{
    return mFields;
}

std::string RecordSchema::ToJsonString() const
{
    if (mFields.empty())
    {
        return std::string();
    }

    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    rapidjson::Value arr(rapidjson::kArrayType);
    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    std::vector<Field>::const_iterator iter;
    for (iter = mFields.begin(); iter != mFields.end(); ++iter)
    {
        rapidjson::Value typeObjString(rapidjson::kStringType);
        typeObjString.SetString(GetNameForFieldType(iter->GetFieldType()).c_str(), allocator);

        rapidjson::Value nameObjString(rapidjson::kStringType);
        nameObjString.SetString(iter->GetFieldName().c_str(), allocator);

        rapidjson::Value notNullObjBool(iter->IsAllowNull() ? rapidjson::kFalseType : rapidjson::kTrueType);

        rapidjson::Value commentObjString(rapidjson::kStringType);
        commentObjString.SetString(iter->GetFieldComment().c_str(), allocator);

        rapidjson::Value objValue;
        objValue.SetObject();
        objValue.AddMember("type", typeObjString, allocator);
        objValue.AddMember("name", nameObjString, allocator);
        objValue.AddMember("notnull", notNullObjBool, allocator);
        objValue.AddMember("comment", commentObjString, allocator);
        arr.PushBack(objValue, allocator);
    }

    jsonDoc.AddMember("fields", arr, allocator);
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    std::string jsonString(strbuf.GetString());

    return jsonString;
}

void RecordSchema::FromJsonString(const std::string& jsonString)
{
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());
    if (!doc.HasMember("fields"))
    {
        return;
    }

    rapidjson::Value& value = doc["fields"];
    if (!value.IsArray())
    {
        return;
    }

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
    {
        rapidjson::Value& fieldJson = value[i];
        if (!fieldJson.IsObject())
        {
            return;
        }

        if (fieldJson.HasMember("name") &&
            fieldJson.HasMember("type") &&
            fieldJson["name"].IsString() &&
            fieldJson["type"].IsString())
        {
            std::string name = fieldJson["name"].GetString();
            std::string type = fieldJson["type"].GetString();

            bool allowNull = true;
            if (fieldJson.HasMember("notnull") && fieldJson["notnull"].IsBool())
            {
                allowNull = !fieldJson["notnull"].GetBool();
            }

            std::string comment = "";
            if (fieldJson.HasMember("comment") && fieldJson["comment"].IsString())
            {
                comment = fieldJson["comment"].GetString();
            }

            AddField(Field(name, GetFieldTypeForName(type), allowNull, comment));
        }
    }
}

bool RecordSchema::operator == (const RecordSchema& rhs) const
{
    if (mFields.size() != rhs.mFields.size())
    {
        return false;
    }
    else
    {
        for (uint32_t i = 0; i < mFields.size(); ++i)
        {
            if (!(mFields[i] == rhs.mFields[i]))
            {
                return false;
            }
        }
        std::map<std::string, int>::const_iterator it = mNameMap.begin();
        for (;it != mNameMap.end(); ++it)
        {
            if (rhs.GetFieldIndex(it->first) != it->second)
            {
                return false;
            }
        }
    }
    return true;
}

} // namespace datahub
} // namespace aliyun
