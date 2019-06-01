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
        C_JSON_E_DEPTH_OVERFLOW,
};

enum {
        C_JSON_TYPE_NULL,
        C_JSON_TYPE_BOOLEAN,
        C_JSON_TYPE_STRING,
        C_JSON_TYPE_NUMBER,
        C_JSON_TYPE_ARRAY,
        C_JSON_TYPE_OBJECT,
};

int c_json_new(CJson **jsonp, size_t max_depth);
CJson * c_json_free(CJson *json);

void c_json_begin_read(CJson *json, const char *string);
int c_json_end_read(CJson *json);
int c_json_peek(CJson *json);
int c_json_read_null(CJson *json);
int c_json_read_string(CJson *json, char **stringp);
int c_json_read_number(CJson *json, const char **numberp, size_t *n_numberp);
int c_json_read_bool(CJson *json, bool *boolp);
bool c_json_more(CJson *json);
int c_json_enter_array(CJson *json);
int c_json_exit_array(CJson *json);
int c_json_enter_object(CJson *json);
int c_json_exit_object(CJson *json);

static inline void c_json_freep(CJson **jsonp) {
        if (*jsonp)
                c_json_free(*jsonp);
}

#ifdef __cplusplus
}
#endif
