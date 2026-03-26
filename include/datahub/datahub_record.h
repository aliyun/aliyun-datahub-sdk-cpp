#ifndef DATAHUB_SDK_DATAHUB_RECORD_H
#define DATAHUB_SDK_DATAHUB_RECORD_H

#include <map>
#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_subscription_offset.h"

namespace aliyun
{
namespace datahub
{

enum RecordType
{
    TUPLE,
    BLOB
};

std::string GetNameForRecordType(const RecordType& recordType);

enum FieldType
{
    UNKNOWN = -1,
    BIGINT = 0,
    DOUBLE,
    BOOLEAN,
    TIMESTAMP,
    STRING,
    DECIMAL,
    INTEGER,
    FLOAT,
    TINYINT,
    SMALLINT,
    JSON
};

std::string GetNameForFieldType(const FieldType& fieldType);

FieldType GetFieldTypeForName(const std::string& name);

class Field
{
public:
    Field(const std::string& name, const FieldType& type, bool allowNull = true, const std::string& comment = "");

    const std::string& GetFieldName() const;
    const std::string& GetFieldComment() const;
    FieldType GetFieldType() const;
    bool IsAllowNull() const;
    bool operator == (const Field& rhs) const;

private:
    bool mAllowNull;
    std::string mName;
    FieldType mType;
    std::string mComment;
};

class FieldData
{
public:
    FieldData() : mIsNull(true) {}
    FieldData(const std::string& value) : mIsNull(false), mValue(value) {}

    void SetValue(const std::string& value)
    {
        mValue = value;
        mIsNull = false;
    }

    void SetNull()
    {
        mValue.clear();
        mIsNull = true;
    }

    std::string GetValue() const { return mValue; }
    bool IsNull() const { return mIsNull; }

private:
    bool mIsNull;
    std::string mValue;
};
typedef std::vector<FieldData> FieldDataVec;

class MessageKey
{
public:
    MessageKey(const std::string& shardId, const std::string& nextCursor, const SubscriptionOffset& offset);

    void Ack();
    bool IsReady();
    const std::string& GetShardId() const;
    const std::string& GetNextCursor() const;
    const SubscriptionOffset& GetOffset() const;

    MessageKey& operator= (const MessageKey& messageKey)
    {
        mIsReady = messageKey.mIsReady.load();
        mShardId = messageKey.mShardId;
        mNextCursor = messageKey.mNextCursor;
        mOffset = messageKey.mOffset;
        return *this;
    }

private:
    std::atomic<bool> mIsReady;
    std::string mShardId;
    std::string mNextCursor;
    SubscriptionOffset mOffset;
};

typedef std::shared_ptr<MessageKey> MessageKeyPtr;

class RecordEntry
{
public:
    explicit RecordEntry(int fieldCount);
    explicit RecordEntry(RecordType recordType);
    RecordEntry(const StringMap& attributes,
            const StringVec& values);
    ~RecordEntry();

    bool SetString(uint32_t index, const std::string& value);
    bool SetBigint(uint32_t index, int64_t value);
    bool SetDouble(uint32_t index, double value);
    bool SetTimestamp(uint32_t index, int64_t value);
    bool SetBoolean(uint32_t index, bool value);
    bool SetDecimal(uint32_t index, const std::string& value);
    bool SetTinyint(uint32_t index, int8_t value);
    bool SetSmallint(uint32_t index, int16_t value);
    bool SetInteger(uint32_t index, int32_t value);
    bool SetFloat(uint32_t index, float value);
    bool SetJson(uint32_t index, const std::string& value);
    bool SetData(const char* data, int len);
    bool SetData(const std::string& data);

    void SetAttribute(const std::string& key, const std::string& value);
    void SetShardId(const std::string& shardId);
    void SetPartitionKey(const std::string& partitionKey);
    void SetHashValue(const std::string& hashValue);
    uint32_t GetTotalSize(void) const;

    std::string GetShardId() const { return mShardId; }
    std::string GetPartitionKey() const { return mPartitionKey; }
    std::string GetHashValue() const { return mHashValue; }

private:
    friend class JsonTool;
    friend class ProtobufTool;
    std::string mShardId;
    std::string mPartitionKey;
    std::string mHashValue;
    StringMap mAttributes;
    FieldDataVec mFieldData;
    std::string mBlobValue;
    RecordType mRecordType;
};

typedef std::vector<RecordEntry> RecordEntryVec;

class RecordResult
{
public:
    RecordResult();
    ~RecordResult();

    int64_t GetSystemTime() const;
    std::string GetCursor() const;
    int64_t GetSequence() const;

    const StringMap& GetAttributes() const;
    const FieldDataVec& GetFieldData() const;

    bool IsNull(uint32_t index) const;
    std::string GetString(uint32_t index) const;
    int64_t GetBigint(uint32_t index) const;
    double GetDouble(uint32_t index) const;
    int64_t GetTimestamp(uint32_t index) const;
    bool GetBoolean(uint32_t index) const;
    std::string GetDecimal(uint32_t index) const;
    int8_t GetTinyint(uint32_t index) const;
    int16_t GetSmallint(uint32_t index) const;
    int32_t GetInteger(uint32_t index) const;
    float GetFloat(uint32_t index) const;
    std::string GetJson(uint32_t index) const;
    const char* GetData(int& len) const;
    std::string GetData(void) const;
    uint32_t GetTotalSize(void) const;

    void SetMessageKey(MessageKeyPtr messageKey);
    MessageKeyPtr GetMessageKey();

private:
    void CheckIndex(uint32_t index) const;
    void CheckNotNull(uint32_t index) const;

private:
    friend class JsonTool;
    friend class ProtobufTool;
    int64_t mSystemTime;
    std::string mCursor;
    int64_t mSequence;
    StringMap mAttributes;
    FieldDataVec mFieldData;
    std::string mBlobValue;
    MessageKeyPtr mMessageKey;
};

typedef std::shared_ptr<RecordResult> RecordResultPtr;

class RecordSchema
{
public:
    RecordSchema();

    void AddField(const Field& field);

    int GetFieldCount() const;

    int GetFieldIndex(const std::string& name) const;

    const Field& GetField(int index) const;

    const std::vector<Field>& GetFields() const;

    std::string ToJsonString() const;

    void FromJsonString(const std::string& jsonString);

    bool operator == (const RecordSchema& rhs) const;

private:
    std::vector<Field> mFields;
    std::map<std::string, int> mNameMap;
};

} // namespace datahub
} // namespace aliyun
#endif
