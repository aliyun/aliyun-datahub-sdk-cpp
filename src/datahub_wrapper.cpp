#include <datahub/datahub_wrapper.h>
#include <datahub/datahub_client.h>
#include <datahub/datahub_exception.h>
#include <iostream>

using namespace aliyun::datahub;

void dh_clear_error(dh_error_t* error)
{
    if (error != NULL)
    {
        error->code = DH_SUCCESS;
        if (error->message != NULL)
        {
            free((void*)error->message);
            error->message = NULL;
        }
    }
}

inline void dh_set_error(int code, const char* message, dh_error_t* error)
{
    if (error != NULL)
    {
        error->code = (dh_ecode_t)code;
        free((void*)error->message);
        error->message = NULL;
        if (message != NULL && code != DH_SUCCESS)
        {
            int len = strlen(message);
            char* buf = (char*)malloc(len + 1);
            memcpy(buf, message, len);
            buf[len] = '\0';
            error->message = buf;
        }
    }
}

void *dh_create_client(const char *endpoint, const char *accessid, const char *accesskey, dh_error_t *error)
{
    if (endpoint == NULL || accessid == NULL || accesskey == NULL)
    {
        dh_set_error(DH_CLIENT_ERROR, "Invalid parameter", error);
        return NULL;
    }

    try
    {
        Account account(accessid, accesskey);
        Configuration config(account, endpoint, true);
        config.SetCompress(compress::LZ4);
        DatahubClient* client = new DatahubClient(config);
        dh_set_error(DH_SUCCESS, NULL, error);
        return (void*)client;
    }
    catch (DatahubException& e)
    {
        dh_set_error(e.GetHttpCode() > 500 ? DH_SERVER_ERROR : DH_CLIENT_ERROR, e.GetMessage().c_str(), error);
        return NULL;
    }
    catch (std::exception& e)
    {
        dh_set_error(DH_SERVER_ERROR, e.what(), error);
        return NULL;
    }
}

void dh_destroy_client(void* client)
{
    DatahubClient *c = (DatahubClient*)client;
    delete c;
}

void *dh_get_schema(void *client, const char *project, const char *topic, dh_error_t *error)
{
    if (client == NULL || project == NULL || topic == NULL)
    {
        dh_set_error(DH_CLIENT_ERROR, "Invalid parameter", error);
        return NULL;
    }

    try
    {
        DatahubClient *c = (DatahubClient*)client;
        const GetTopicResult& result = c->GetTopic(project, topic);
        RecordSchema* schema = new RecordSchema();
        schema->FromJsonString(result.GetRecordSchema().ToJsonString());
        dh_set_error(DH_SUCCESS, NULL, error);
        return (void*)schema;
    }
    catch (DatahubException& e)
    {
        dh_set_error(e.GetHttpCode() > 500 ? DH_SERVER_ERROR : DH_CLIENT_ERROR, e.GetMessage().c_str(), error);
        return NULL;
    }
    catch (std::exception& e)
    {
        dh_set_error(DH_SERVER_ERROR, e.what(), error);
        return NULL;
    }
}

void dh_destroy_schema(void *schema)
{
    RecordSchema *s = (RecordSchema*)schema;
    delete s;
}

int dh_get_field_count(void *schema)
{
    RecordSchema *s = (RecordSchema*)schema;
    return s->GetFieldCount();
}

dh_field_type_t dh_get_field_type(void *schema, int index)
{
    RecordSchema *s = (RecordSchema*)schema;
    const std::vector<Field>& fields = s->GetFields();
    if ((uint32_t)index < fields.size())
    {
        return (dh_field_type_t)fields[index].GetFieldType();
    }
    else
    {
        return dh_field_type_t::UNKNOWN;
    }
}

const char* dh_get_field_name(void *schema, int index)
{
    RecordSchema *s = (RecordSchema*)schema;
    const std::vector<Field>& fields = s->GetFields();
    if ((uint32_t)index < fields.size())
    {
        return fields[index].GetFieldName().c_str();
    }
    else
    {
        return NULL;
    }
}

void *dh_create_record_entries(void)
{
    return new std::vector<RecordEntry>();
}

void dh_destroy_record_entries(void *entries)
{
    delete (std::vector<RecordEntry>*)entries;
}

void *dh_append_record(void *entries, void *schema)
{
    if (entries == NULL || schema == NULL)
    {
        return NULL;
    }

    std::vector<RecordEntry>* e = (std::vector<RecordEntry>*)entries;
    RecordSchema *s = (RecordSchema*)schema;
    if (s->GetFieldCount() > 0)
    {
        RecordEntry entry(s->GetFieldCount());
        e->emplace_back(entry);
    }
    else
    {
        RecordEntry entry(BLOB);
        e->emplace_back(entry);
    }

    return (void*)(&e->back());
}

void dh_set_record_property(void *record, const char *key, int klen, const char *value, int vlen)
{
    RecordEntry* e = (RecordEntry*)record;
    e->SetAttribute(std::string(key, klen), std::string(value, vlen));
}

void dh_set_record_shard_id(void *record, const char *id, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    e->SetShardId(std::string(id, len));
}

void dh_set_record_partition_key(void *record, const char *pk, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    e->SetPartitionKey(std::string(pk, len));
}

void dh_set_record_hash_value(void *record, const char *hv, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    e->SetHashValue(std::string(hv, len));
}

int dh_get_record_size(void *record)
{
    RecordEntry* e = (RecordEntry*)record;
    return (int)e->GetTotalSize();
}

bool dh_set_string_value(void *record, int index, const char *value, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetString(index, std::string(value, len));
}

bool dh_set_decimal_value(void *record, int index, const char *value, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetDecimal(index, std::string(value, len));
}

bool dh_set_timestamp_value(void *record, int index, int64_t value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetTimestamp(index, value);
}

bool dh_set_boolean_value(void *record, int index, bool value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetBoolean(index, value);
}

bool dh_set_tinyint_value(void *record, int index, int8_t value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetTinyint(index, value);
}

bool dh_set_smallint_value(void *record, int index, int16_t value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetSmallint(index, value);
}

bool dh_set_bigint_value(void *record, int index, int64_t value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetBigint(index, value);
}

bool dh_set_integer_value(void *record, int index, int32_t value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetInteger(index, value);
}

bool dh_set_float_value(void *record, int index, float value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetFloat(index, value);
}

bool dh_set_double_value(void *record, int index, double value)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetDouble(index, value);
}

bool dh_set_json_value(void *record, int index, const char *value, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetJson(index, std::string(value, len));
}

bool dh_set_blob_value(void *record, const char *buf, int len)
{
    RecordEntry* e = (RecordEntry*)record;
    return e->SetData(buf, len);
}

bool dh_put_records(void *client, const char *project, const char *topic, void* entries, dh_error_t* error)
{
    if (client == NULL || project == NULL || topic == NULL || entries == NULL)
    {
        dh_set_error(DH_CLIENT_ERROR, "Invalid parameter", error);
        return false;
    }

    DatahubClient *c = (DatahubClient*)client;
    std::vector<RecordEntry>* e = (std::vector<RecordEntry>*)entries;
    try
    {
        const PutRecordResult& result = c->PutRecord(project, topic, *e);
        if (result.GetFailedRecordCount() == 0)
        {
            dh_set_error(DH_SUCCESS, NULL, error);
            return true;
        }
        else
        {
            std::string message = result.GetErrorEntries().front().GetErrorCode();
            message.append(",").append(result.GetErrorEntries().front().GetErrorMessage());
            dh_set_error(DH_SERVER_ERROR, message.c_str(), error);
            return false;
        }
    }
    catch (DatahubException& e)
    {
        dh_set_error(e.GetHttpCode() >= 500 ? DH_SERVER_ERROR : DH_CLIENT_ERROR, e.GetMessage().c_str(), error);
        return false;
    }
    catch (std::exception& e)
    {
        dh_set_error(DH_SERVER_ERROR, e.what(), error);
        return false;
    }
}
