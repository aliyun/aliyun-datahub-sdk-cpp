#ifndef INCLUDE_DATAHUB_WRAPPER_H
#define INCLUDE_DATAHUB_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*----------------------------------------------------------
This header file is provided for c programming of datahub sdk
----------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dh_ecode_t {
    DH_CLIENT_ERROR = -499,
    DH_SERVER_ERROR = -599,
    DH_SUCCESS = 0
} dh_ecode_t;

typedef struct dh_error_t {
    dh_ecode_t code;
    const char *message;
} dh_error_t;

/* clear error object
param:
    arg0 error object reference
return:
    return void, just free message buf
*/
void dh_clear_error(dh_error_t* error);

/* create datahub client
param:
    arg0 endpoint of datahub
    arg1 access id which use to visit datahub
    arg2 access key which use to visit datahub
return:
    object of client, thread safe and use only one object in one process
    return null when error happened
*/
void *dh_create_client(const char *endpoint, const char *accessid, const char *accesskey, dh_error_t *error);

/* destroy datahub client
param:
    arg0 object of client
return:
    void
*/
void dh_destroy_client(void *client);

typedef enum dh_field_type_t
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
} dh_field_type_t;

/* get schema from server
param:
    arg0 object of client
    arg1 project name which to get schema
    arg2 topic name which to get schema
return:
    object of schema
    return null when error happened
*/
void *dh_get_schema(void *client, const char *project, const char *topic, dh_error_t* error);

/* destroy topic schema
param:
    arg0 object of schema
return:
    void
*/
void dh_destroy_schema(void *schema);

/* get field count
param:
    arg0 object of schema
return:
    field count, zeor means blob
*/
int dh_get_field_count(void *schema);

/* get field type
param:
    arg0 object of schema
    arg0 field index
return:
    field type
*/
dh_field_type_t dh_get_field_type(void *schema, int index);

/* get field name
param:
    arg0 object of schema
    arg0 field index
return:
    field name, do not to free
*/
const char* dh_get_field_name(void *schema, int index);

/* create record entries
param:
    none
return:
    object of record entries
*/
void *dh_create_record_entries(void);

/* destroy record entries, all record in entries will be released
param:
    arg0 object of record entries
return:
    void
*/
void dh_destroy_record_entries(void *entries);


/* append one record to record entries
param:
    arg0 record entries
    arg1 project name which to get schema
    arg2 topic name which to get schema
return:
    object of new record
    return null when error happened
*/
void *dh_append_record(void *entries, void *schema);

/* set record shard id
param:
    arg0 record object
    arg1 id buf pointer
    arg2 id buf length
return:
    void
*/
void dh_set_record_shard_id(void *record, const char *id, int len);

/* set record parititon key
param:
    arg0 record object
    arg1 pk buf pointer
    arg2 pk buf length
return:
    void
*/
void dh_set_record_partition_key(void *record, const char *pk, int len);

/* set record hash value
param:
    arg0 record object
    arg1 hash buf pointer
    arg2 hash buf length
return:
    void
*/
void dh_set_record_hash_value(void *record, const char *hv, int len);

/* set record property
param:
    arg0 record object
    arg1 key buf pointer
    arg2 key buf length
    arg3 value buf pointer
    arg4 value buf length
return:
    void
*/
void dh_set_record_property(void *record, const char* key, int klen, const char* value, int vlen);

/* set field value of string type
param:
    arg0 record object
    arg1 column index
    arg2 string pointer
    arg3 string length
return:
    return true if success else false
*/
bool dh_set_string_value(void *record, int index, const char *value, int len);

/* set field value of timestamp type
param:
    arg0 record object
    arg1 column index
    arg2 timestamp value, default microseconds
return:
    return true if success else false
*/
bool dh_set_timestamp_value(void *record, int index, int64_t value);

/* set field value of bool type
param:
    arg0 record object
    arg1 column index
    arg2 bool value
return:
    return true if success else false
*/
bool dh_set_boolean_value(void *record, int index, bool value);

/* set field value of decimal type
param:
    arg0 record object
    arg1 column index
    arg2 decimal pointer
    arg3 decimal length
return:
    return true if success else false
*/
bool dh_set_decimal_value(void *record, int index, const char *value, int len);

/* set field value of tinyint type
param:
    arg0 record object
    arg1 column index
    arg2 tinyint value
return:
    return true if success else false
*/
bool dh_set_tinyint_value(void *record, int index, int8_t value);

/* set field value of smallint type
param:
    arg0 record object
    arg1 column index
    arg2 smallint value
return:
    return true if success else false
*/
bool dh_set_smallint_value(void *record, int index, int16_t value);

/* set field value of bigint type
param:
    arg0 record object
    arg1 column index
    arg2 bigint value
return:
    return true if success else false
*/
bool dh_set_bigint_value(void *record, int index, int64_t value);

/* set field value of integer type
param:
    arg0 record object
    arg1 column index
    arg2 integer value
return:
    return true if success else false
*/
bool dh_set_integer_value(void *record, int index, int32_t value);

/* set field value of float type
param:
    arg0 record object
    arg1 column index
    arg2 float value
return:
    return true if success else false
*/
bool dh_set_float_value(void *record, int index, float value);

/* set field value of double type
param:
    arg0 record object
    arg1 column index
    arg2 double value
return:
    return true if success else false
*/
bool dh_set_double_value(void *record, int index, double value);

/* set field value of json type
param:
    arg0 record object
    arg1 column index
    arg2 json pointer
    arg3 json length
return:
    return true if success else false
*/
bool dh_set_json_value(void *record, int index, const char *value, int len);

/* set field value of double type
param:
    arg0 record object
    arg1 column index
    arg2 double value
return:
    return true if success else false
*/
bool dh_set_blob_value(void *record, const char *buf, int len);

/* get size of one record
param:
    arg0 record object
return:
    return size
*/
int dh_get_record_size(void *record);

/* get schema from server
param:
    arg0 object of client
    arg1 project name which to put record
    arg2 topic name which to put record
    arg3 record entries
return:
    return true if success else false
*/
bool dh_put_records(void *client, const char *project, const char *topic, void *entries, dh_error_t* error);

#ifdef __cplusplus
}
#endif
#endif
