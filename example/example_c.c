#include "datahub/datahub_wrapper.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define PROG_VERSION "1.0"
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

void usage(const char* prog)
{
    const char *start = strrchr(prog, '/');
    start = (start != NULL) ? start + 1 : prog;
    fprintf(stdout,
        "%s Ver %s\n"
        "Usage: <programme> [options...]\n"
        "   -e --endpoint  <string>     set endpoint\n"
        "   -p --project   <string>     set project\n"
        "   -t --topic     <string>     set topic\n"
        "   -k --accesskey <string>     set access key\n"
        "   -i --accessid  <string>     set access id\n"
        "   -n --records   <integer>    set record count per request\n"
        "   -c --count     <integer>    set request number\n"
        "   -T --thread    <integer>    set thread number\n"
        "   -h --help                   show help message\n", start, PROG_VERSION);
}

typedef struct client_info_t {
    void *client;
    void *schema;
    char *endpoint;
    char *accessid;
    char *accesskey;
    char *project;
    char *topic;
    int count;
    int records;
    int threads;
} client_info_t;

static client_info_t g_client;

char *clone(const char* str)
{
    int len = strlen(str);
    char *ptr = (char*)malloc(len+1);
    memcpy(ptr, str, len);
    ptr[len] = '\0';
    return ptr;
}

void parse_args(int argc, char *argv[])
{
    client_info_t client = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    g_client = client;

    int opt_id;
    static struct option long_opts[] =
    {
        { "endpoint", 1, 0, 'e' },
        { "project", 1, 0, 'p' },
        { "topic", 1, 0, 't' },
        { "accessid", 1, 0, 'i'},
        { "accesskey", 1, 0, 'k'},
        { "count", 1, 0, 'c' },
        { "records", 1, 0, 'n' },
        { "thread", 1, 0, 'T' },
        { "version", 0, 0, 'v' },
        { "help", 0, 0, 'h' },
        { NULL, 0, 0, 0 }
    };

    while (1)
    {
        int ret = getopt_long(argc, argv, "e:p:t:i:k:c:n:T:vh", long_opts, &opt_id);
        if( ret == -1 )
            break;

        switch(ret)
        {
        case 'e' :
            g_client.endpoint = clone(optarg);
            break;
        case 'p' :
            g_client.project = clone(optarg);
            break;
        case 't' :
            g_client.topic = clone(optarg);
            break;
        case 'i' :
            g_client.accessid = clone(optarg);
            break;
        case 'k' :
            g_client.accesskey = clone(optarg);
            break;
        case 'c' :
            g_client.count = atoi(optarg);
            break;
        case 'n' :
            g_client.records = atoi(optarg);
            break;
        case 'T' :
            g_client.threads = atoi(optarg);
            break;
        case 'h' :
            usage(argv[0]);
            exit(0);
            break;
        case ':' :
            fprintf(stderr, "no argument value %s\n", optarg);
            exit(1);
            break;
        case '?':
            fprintf(stderr, "invalid argument %s\n", optarg);
            exit(1);
            break;
        default:
            break;
        }
    }

    if (g_client.endpoint == NULL || g_client.project == NULL ||
        g_client.topic == NULL || g_client.accessid == NULL ||
        g_client.accesskey == NULL)
    {
        fprintf(stderr, "invalid parameter count\n");
        exit(1);
    }
};

void mock_record(void *record, void *schema)
{
    int count = dh_get_field_count(schema);
    uint32_t tm = time(NULL);
    for (int i = 0; i < count; i++)
    {
        //tuple topic
        dh_field_type_t type = dh_get_field_type(schema, i);
        switch (type)
        {
        case BOOLEAN:
            dh_set_boolean_value(record, i, (bool)i%2);
            break;
        case TINYINT:
            dh_set_tinyint_value(record, i, (int8_t)i);
            break;
        case SMALLINT:
            dh_set_smallint_value(record, i, (int16_t)i);
            break;
        case INTEGER:
            dh_set_integer_value(record, i, (int32_t)i);
            break;
        case BIGINT:
            dh_set_bigint_value(record, i, (int64_t)i);
            break;
        case FLOAT:
            dh_set_float_value(record, i, (float)i);
            break;
        case DOUBLE:
            dh_set_double_value(record, i, (double)i);
            break;
        case DECIMAL:
            dh_set_decimal_value(record, i, "1.111111", 8);
            break;
        case STRING:
            dh_set_string_value(record, i, "hello world", 11);
            break;
        case TIMESTAMP:
            dh_set_timestamp_value(record, i, tm*1000000l);
            break;
        default:
            break;
        }
    }

    // blob topic
    if (count == 0)
    {
        dh_set_blob_value(record, "hello world", 11);
    }

    //if use meta time to sync maxcompute, add this property
    char buf[32] = {0};
    int len = snprintf(buf, sizeof(buf), "%ld", tm * 1000l); //ms
    dh_set_record_property(record, "__dh_meta_time__", sizeof("__dh_meta_time__") - 1, buf, len);
}

void clear(void)
{
    free(g_client.endpoint);
    free(g_client.accessid);
    free(g_client.accesskey);
    free(g_client.project);
    free(g_client.topic);
    dh_destroy_client(g_client.client);
    dh_destroy_schema(g_client.schema);
}

void* run_task(void *arg)
{
    client_info_t *client = (client_info_t*)arg;
    dh_error_t error = {DH_SUCCESS, NULL};
    void *entries = NULL;
    for (int i = 0; i < client->count && error.code == DH_SUCCESS; i++)
    {
        if (entries == NULL)
        {
            entries = dh_create_record_entries();
            for (int i = 0; i < client->records; i++)
            {
                void *record = dh_append_record(entries, client->schema);
                mock_record(record, client->schema);
            }
        }

        //if success, continue
        if (dh_put_records(client->client, client->project, client->topic, entries, &error))
        {
            dh_destroy_record_entries(entries);
            entries = NULL;
            continue;
        }

        // if client error, exit
        if (error.code == DH_CLIENT_ERROR)
        {
            fprintf(stderr, "put record failed: %s\n", error.message);
            break;
        }

        //if server error, wait 1 second and resend last entries
        fprintf(stderr, "put record failed: %s\n", error.message);
        sleep(1);
    }

    dh_clear_error(&error);
    return NULL;
}

int main(int argc, char* argv[])
{
    parse_args(argc, argv);

    dh_error_t error = {DH_SUCCESS, NULL};
    //one client can use to visit different topic
    g_client.client = dh_create_client(g_client.endpoint, g_client.accessid, g_client.accesskey, &error);

    if (g_client.client == NULL)
    {
        fprintf(stderr, "create client failed: %s\n", error.message);
        dh_clear_error(&error);
        clear();
        return -1;
    }

    g_client.schema = dh_get_schema(g_client.client, g_client.project, g_client.topic, &error);
    if (g_client.schema == NULL)
    {
        fprintf(stderr, "get schema failed: %s\n", error.message);
        dh_clear_error(&error);
        clear();
        return -1;
    }

    //just for demo
    g_client.threads = min(max(1, g_client.threads), 64);
    g_client.records = min(max(1, g_client.records), 2000);
    g_client.count = min(max(1, g_client.count), 10000);

    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t)*g_client.threads);
    for (int i = 0; i < g_client.threads; i++)
    {
        pthread_create(threads + i, NULL, run_task, (void*)&g_client);
    }

    for (int i = 0; i < g_client.threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    dh_clear_error(&error);
    clear();
    return 0;
}
