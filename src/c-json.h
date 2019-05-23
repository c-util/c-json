#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>

typedef struct CJson CJson;
typedef struct CJsonLevel CJsonLevel;

enum  {
        _C_JSON_E_SUCCESS,
        C_JSON_E_INVALID_JSON,
        C_JSON_E_INVALID_TYPE,
};

struct CJsonLevel {
        CJsonLevel *parent;
        char state; /* [, {, or : */
        bool first;
};

struct CJson {
        const char *input;

        const char *p;
        CJsonLevel *level;

        int poison;
};

#define C_JSON_INIT { 0 }

void c_json_init(CJson *json);
void c_json_deinit(CJson *json);

void c_json_begin_read(CJson *json, const char *string);
int c_json_end_read(CJson *json);
int c_json_read_string(CJson *json, char **stringp);
int c_json_read_u64(CJson *json, uint64_t *numberp);
int c_json_read_f64(CJson *json, double *numberp);
int c_json_read_bool(CJson *json, bool *boolp);
bool c_json_more(CJson *json);
int c_json_open_array(CJson *json);
int c_json_close_array(CJson *json);
int c_json_open_object(CJson *json);
int c_json_close_object(CJson *json);

#ifdef __cplusplus
}
#endif
