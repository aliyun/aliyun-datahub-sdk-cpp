#include "json_tool.h"
#include "utils.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/buffer.h>
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

void JsonTool::JsonToErrorEntry(const rapidjson::Value& jsonValue, ErrorEntry& entry)
{
    rapidjson::Value::ConstMemberIterator indexItr = jsonValue.FindMember("Index");
    if (indexItr != jsonValue.MemberEnd() && indexItr->value.IsInt())
    {
        entry.mIndex = indexItr->value.GetInt();
    }

    rapidjson::Value::ConstMemberIterator it = jsonValue.FindMember("ErrorCode");
    if (it != jsonValue.MemberEnd() && it->value.IsString())
    {
        entry.mErrorCode.assign(it->value.GetString(), it->value.GetStringLength());
    }

    it = jsonValue.FindMember("ErrorMessage");
    if (it != jsonValue.MemberEnd() && it->value.IsString())
    {
        entry.mErrorMessage.assign(it->value.GetString(), it->value.GetStringLength());
    }

    it = jsonValue.FindMember("ErrorDetail");
    if (it != jsonValue.MemberEnd() && it->value.IsString())
    {
        entry.mErrorDetail.assign(it->value.GetString(), it->value.GetStringLength());
    }
}

void JsonTool::JsonToRecordResult(const rapidjson::Value& jsonValue, RecordResult& entry)
{
    rapidjson::Value::ConstMemberIterator dataItr = jsonValue.FindMember("Data");
    if (dataItr != jsonValue.MemberEnd())
    {
        const rapidjson::Value& data = dataItr->value;
        if (data.IsArray())
        {
            entry.mFieldData.clear();
            entry.mFieldData.resize(data.Size());
            for (rapidjson::SizeType i = 0; i < data.Size(); ++i)
            {
                if (!data[i].IsNull())
                {
                    entry.mFieldData[i].SetValue(std::string(data[i].GetString(), data[i].GetStringLength()));
                }
            }
        }
        else if (data.IsString())
        {
            DecodeBlobValue(data.GetString(), data.GetStringLength(), entry.mBlobValue);
        }
    }

    rapidjson::Value::ConstMemberIterator attrItr = jsonValue.FindMember("Attributes");
    if (attrItr != jsonValue.MemberEnd())
    {
        const rapidjson::Value& attr = attrItr->value;
        if (attr.IsObject())
        {
            for (rapidjson::Value::ConstMemberIterator itr = attr.MemberBegin();
                    itr != attr.MemberEnd(); ++itr)
            {
                if (itr->name.IsString() && itr->value.IsString())
                {
                    std::string key = itr->name.GetString();
                    std::string val = itr->value.GetString();
                    entry.mAttributes[key] = val;
                }
            }
        }
    }

    rapidjson::Value::ConstMemberIterator stItr = jsonValue.FindMember("SystemTime");
    if (stItr != jsonValue.MemberEnd())
    {
        if (stItr->value.IsInt64())
        {
            entry.mSystemTime = stItr->value.GetInt64();
        }
        else
        {
            entry.mSystemTime = -1;
        }
    }

    rapidjson::Value::ConstMemberIterator cursorItr = jsonValue.FindMember("Cursor");
    if (cursorItr != jsonValue.MemberEnd() && cursorItr->value.IsString())
    {
        entry.mCursor.assign(cursorItr->value.GetString(), cursorItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator seqItr = jsonValue.FindMember("Sequence");
    if (seqItr != jsonValue.MemberEnd())
    {
        if (seqItr->value.IsInt64())
        {
            entry.mSequence = seqItr->value.GetInt64();
        }
        else
        {
            entry.mSequence = -1;
        }
    }
}

void JsonTool::RecordEntryToJson(const RecordEntry& entry, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue)
{
    if (!entry.mShardId.empty())
    {
        rapidjson::Value shardId(rapidjson::kStringType);
        shardId.SetString(entry.mShardId.c_str(), allocator);

        jsonValue.AddMember("ShardId", shardId, allocator);
    }
    else if (!entry.mPartitionKey.empty())
    {
        rapidjson::Value partitionKey(rapidjson::kStringType);
        partitionKey.SetString(entry.mPartitionKey.c_str(), allocator);

        jsonValue.AddMember("PartitionKey", partitionKey, allocator);
    }
    else if (!entry.mHashValue.empty())
    {
        rapidjson::Value hashValue(rapidjson::kStringType);
        hashValue.SetString(entry.mHashValue.c_str(), allocator);

        jsonValue.AddMember("HashValue", hashValue, allocator);
    }

    rapidjson::Value attributes(rapidjson::kObjectType);

    std::map<std::string, std::string>::const_iterator iter;
    for (iter = entry.mAttributes.begin(); iter != entry.mAttributes.end(); ++iter)
    {
        rapidjson::Value key(rapidjson::kStringType);
        key.SetString(iter->first.c_str(), allocator);

        rapidjson::Value value(rapidjson::kStringType);
        value.SetString(iter->second.c_str(), allocator);

        attributes.AddMember(key, value, allocator);
    }
    jsonValue.AddMember("Attributes", attributes, allocator);

    if (entry.mRecordType == TUPLE)
    {
        rapidjson::Value data(rapidjson::kArrayType);
        FieldDataVec::const_iterator it;
        for (it = entry.mFieldData.begin(); it != entry.mFieldData.end(); ++it)
        {
            rapidjson::Value str;
            if (!it->IsNull())
            {
                str.SetString(it->GetValue().c_str(), it->GetValue().size(), allocator);
            }
            data.PushBack(str, allocator);
        }
        jsonValue.AddMember("Data", data, allocator);
    }
    else
    {
        rapidjson::Value blobValue(rapidjson::kStringType);
        EncodeBlobValue(entry.mBlobValue, allocator, blobValue);
        jsonValue.AddMember("Data", blobValue, allocator);
    }
}

void JsonTool::JsonToShardDesc(const rapidjson::Value& jsonValue, ShardDesc& desc)
{
    rapidjson::Value::ConstMemberIterator shardIdItr = jsonValue.FindMember("ShardId");
    if (shardIdItr != jsonValue.MemberEnd() && shardIdItr->value.IsString())
    {
        desc.mShardId.assign(shardIdItr->value.GetString(), shardIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator bhkItr = jsonValue.FindMember("BeginHashKey");
    if (bhkItr != jsonValue.MemberEnd() && bhkItr->value.IsString())
    {
        desc.mBeginHashKey.assign(bhkItr->value.GetString(), bhkItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator ehkItr = jsonValue.FindMember("EndHashKey");
    if (ehkItr != jsonValue.MemberEnd() && ehkItr->value.IsString())
    {
        desc.mEndHashKey.assign(ehkItr->value.GetString(), ehkItr->value.GetStringLength());
    }
}

void JsonTool::JsonToShardEntry(const rapidjson::Value& jsonValue, ShardEntry& entry)
{
    rapidjson::Value::ConstMemberIterator shardIdItr = jsonValue.FindMember("ShardId");
    if (shardIdItr != jsonValue.MemberEnd() && shardIdItr->value.IsString())
    {
        entry.mShardId.assign(shardIdItr->value.GetString(), shardIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator stateItr = jsonValue.FindMember("State");
    if (stateItr != jsonValue.MemberEnd() && stateItr->value.IsString())
    {
        entry.mState = GetShardStateForName(stateItr->value.GetString());
    }

    rapidjson::Value::ConstMemberIterator ctItr = jsonValue.FindMember("ClosedTime");
    if (ctItr != jsonValue.MemberEnd() && ctItr->value.IsInt64())
    {
        entry.mClosedTime = ctItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator bhkItr = jsonValue.FindMember("BeginHashKey");
    if (bhkItr != jsonValue.MemberEnd() && bhkItr->value.IsString())
    {
        entry.mBeginHashKey.assign(bhkItr->value.GetString(), bhkItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator ehkItr = jsonValue.FindMember("EndHashKey");
    if (ehkItr != jsonValue.MemberEnd() && ehkItr->value.IsString())
    {
        entry.mEndHashKey.assign(ehkItr->value.GetString(), ehkItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator psiItr = jsonValue.FindMember("ParentShardIds");
    if (psiItr != jsonValue.MemberEnd() && psiItr->value.IsArray())
    {
        const rapidjson::Value& data = psiItr->value;
        for (rapidjson::SizeType i = 0; i < data.Size(); ++i)
        {
            std::string shardId = data[i].GetString();
            entry.mParentShardIds.push_back(shardId);
        }
    }

    rapidjson::Value::ConstMemberIterator lsiItr = jsonValue.FindMember("LeftShardId");
    if (lsiItr != jsonValue.MemberEnd() && lsiItr->value.IsString())
    {
        entry.mLeftShardId.assign(lsiItr->value.GetString(), lsiItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator rsiItr = jsonValue.FindMember("RightShardId");
    if (rsiItr != jsonValue.MemberEnd() && rsiItr->value.IsString())
    {
        entry.mRightShardId.assign(rsiItr->value.GetString(), rsiItr->value.GetStringLength());
    }
}

void JsonTool::JsonToSubscriptionEntry(const rapidjson::Value& jsonValue, SubscriptionEntry& entry)
{
    rapidjson::Value::ConstMemberIterator subIdItr = jsonValue.FindMember("SubId");
    if (subIdItr != jsonValue.MemberEnd() && subIdItr->value.IsString())
    {
        entry.mSubId.assign(subIdItr->value.GetString(), subIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator commentItr = jsonValue.FindMember("Comment");
    if (commentItr != jsonValue.MemberEnd() && commentItr->value.IsString())
    {
        entry.mComment.assign(commentItr->value.GetString(), commentItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator isOwnerItr = jsonValue.FindMember("IsOwner");
    if (isOwnerItr != jsonValue.MemberEnd() && isOwnerItr->value.IsBool())
    {
        entry.mIsOwner = isOwnerItr->value.GetBool();
    }

    rapidjson::Value::ConstMemberIterator stateItr = jsonValue.FindMember("State");
    if (stateItr != jsonValue.MemberEnd() && stateItr->value.IsInt())
    {
        entry.mState = GetSubscriptionStateFromValue(stateItr->value.GetInt());
    }

    rapidjson::Value::ConstMemberIterator typeItr = jsonValue.FindMember("Type");
    if (typeItr != jsonValue.MemberEnd() && typeItr->value.IsInt())
    {
        entry.mType = GetSubscriptionTypeFromValue(typeItr->value.GetInt());
    }

    rapidjson::Value::ConstMemberIterator createTimeItr = jsonValue.FindMember("CreateTime");
    if (createTimeItr != jsonValue.MemberEnd() && createTimeItr->value.IsInt())
    {
        entry.mCreateTime = createTimeItr->value.GetInt() * 1000l;
    }

    rapidjson::Value::ConstMemberIterator lastModifyTimeItr = jsonValue.FindMember("LastModifyTime");
    if (lastModifyTimeItr != jsonValue.MemberEnd() && lastModifyTimeItr->value.IsInt())
    {
        entry.mLastModifyTime = lastModifyTimeItr->value.GetInt() * 1000l;
    }
}

void JsonTool::JsonToSubscriptionOffset(const rapidjson::Value& jsonValue, SubscriptionOffset& entry)
{
    rapidjson::Value::ConstMemberIterator timestampItr = jsonValue.FindMember("Timestamp");
    if (timestampItr != jsonValue.MemberEnd() && timestampItr->value.IsInt64())
    {
        entry.SetTimestamp(timestampItr->value.GetInt64());
    }

    rapidjson::Value::ConstMemberIterator sequenceItr = jsonValue.FindMember("Sequence");
    if (sequenceItr != jsonValue.MemberEnd() && sequenceItr->value.IsInt64())
    {
        entry.SetSequence(sequenceItr->value.GetInt64());
    }

    rapidjson::Value::ConstMemberIterator batchIndexItr = jsonValue.FindMember("BatchIndex");
    if (batchIndexItr != jsonValue.MemberEnd() && batchIndexItr->value.IsUint())
    {
        entry.SetBatchIndex(batchIndexItr->value.GetUint());
    }

    rapidjson::Value::ConstMemberIterator versionItr = jsonValue.FindMember("Version");
    if (versionItr != jsonValue.MemberEnd() && versionItr->value.IsInt64())
    {
        entry.SetVersion(versionItr->value.GetInt64());
    }

    rapidjson::Value::ConstMemberIterator sessionIdItr = jsonValue.FindMember("SessionId");
    if (sessionIdItr != jsonValue.MemberEnd() && sessionIdItr->value.IsInt64())
    {
        entry.SetSessionId(sessionIdItr->value.GetInt64());
    }
}

void JsonTool::SubscriptionOffsetToJson(const SubscriptionOffset& entry, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue)
{
    rapidjson::Value timestamp(rapidjson::kNumberType);
    timestamp.SetInt64(entry.GetTimestamp());
    jsonValue.AddMember("Timestamp", timestamp, allocator);

    rapidjson::Value sequence(rapidjson::kNumberType);
    sequence.SetInt64(entry.GetSequence());
    jsonValue.AddMember("Sequence", sequence, allocator);

    rapidjson::Value batchIndex(rapidjson::kNumberType);
    batchIndex.SetUint(entry.GetBatchIndex());
    jsonValue.AddMember("BatchIndex", batchIndex, allocator);

    rapidjson::Value version(rapidjson::kNumberType);
    version.SetInt64(entry.GetVersion());
    jsonValue.AddMember("Version", version, allocator);

    rapidjson::Value sessionId(rapidjson::kNumberType);
    sessionId.SetInt64(entry.GetSessionId());
    jsonValue.AddMember("SessionId", sessionId, allocator);
}

void JsonTool::JsonToMeterRecord(const rapidjson::Value& jsonValue, MeterRecord& record)
{
    rapidjson::Value::ConstMemberIterator atItr = jsonValue.FindMember("ActiveTime");
    if (atItr != jsonValue.MemberEnd() && atItr->value.IsInt64())
    {
        record.mActiveTime = atItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator ssItr = jsonValue.FindMember("Storage");
    if (ssItr != jsonValue.MemberEnd() && ssItr->value.IsInt64())
    {
        record.mStorageSize = ssItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator rdsItr = jsonValue.FindMember("ReadDataSize");
    if (rdsItr != jsonValue.MemberEnd() && rdsItr->value.IsInt64())
    {
        record.mReadDataSize = rdsItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator wdsItr = jsonValue.FindMember("WriteDataSize");
    if (wdsItr != jsonValue.MemberEnd() && wdsItr->value.IsInt64())
    {
        record.mWriteDataSize = wdsItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator rtItr = jsonValue.FindMember("ReadTimes");
    if (rtItr != jsonValue.MemberEnd() && rtItr->value.IsInt64())
    {
        record.mReadTimes = rtItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator wtItr = jsonValue.FindMember("WriteTimes");
    if (wtItr != jsonValue.MemberEnd() && wtItr->value.IsInt64())
    {
        record.mWriteTimes = wtItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator cdsItr = jsonValue.FindMember("ConnectorDataSize");
    if (cdsItr != jsonValue.MemberEnd() && cdsItr->value.IsInt64())
    {
        record.mConnectorDataSize = cdsItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator stItr = jsonValue.FindMember("StartTime");
    if (stItr != jsonValue.MemberEnd() && stItr->value.IsInt64())
    {
        record.mStartTime = stItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator etItr = jsonValue.FindMember("EndTime");
    if (etItr != jsonValue.MemberEnd() && etItr->value.IsInt64())
    {
        record.mEndTime = etItr->value.GetInt64();
    }
}

void JsonTool::EncodeBlobValue(const std::string& blobValue, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& value)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bmem = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, blobValue.c_str(), blobValue.length());
    (void)BIO_flush(b64);

    BUF_MEM *bptr;
    BIO_get_mem_ptr(b64, &bptr);

    value.SetString(bptr->data, bptr->length, allocator);

    BIO_free_all(b64);
}

void JsonTool::DecodeBlobValue(const char* data, int len, std::string& out)
{
    BIO *bio,*base64_filter,*bio_out;
    char inbuf[512];
    int inlen;
    base64_filter = BIO_new(BIO_f_base64());
    BIO_set_flags(base64_filter, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf((void*)data, len);
    bio = BIO_push(base64_filter, bio);
    bio_out = BIO_new(BIO_s_mem());
    while((inlen = BIO_read(bio, inbuf, 512)) > 0)
    {
        BIO_write(bio_out, inbuf, inlen);
    }
    (void)BIO_flush(bio_out);

    char *new_data;
    long bytes_written = BIO_get_mem_data(bio_out, &new_data);
    out.assign(new_data, bytes_written);
    BIO_free_all(bio);
    BIO_free_all(bio_out);
}

} // namespace datahub
} // namespace aliyun
