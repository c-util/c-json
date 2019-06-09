#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>

typedef struct CJsonReader CJsonReader;
typedef struct CJsonWriter CJsonWriter;
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

/* readers */
int c_json_reader_new(CJsonReader **readerp, size_t max_depth);
CJsonReader * c_json_reader_free(CJsonReader *reader);

void c_json_reader_begin_read(CJsonReader *reader, const char *string);
int c_json_reader_end_read(CJsonReader *reader);
int c_json_reader_peek(CJsonReader *reader);
int c_json_reader_read_null(CJsonReader *reader);
int c_json_reader_read_string(CJsonReader *reader, char **stringp);
int c_json_reader_read_number(CJsonReader *reader, const char **numberp, size_t *n_numberp);
int c_json_reader_read_bool(CJsonReader *reader, bool *boolp);
bool c_json_reader_more(CJsonReader *reader);
int c_json_reader_enter_array(CJsonReader *reader);
int c_json_reader_exit_array(CJsonReader *reader);
int c_json_reader_enter_object(CJsonReader *reader);
int c_json_reader_exit_object(CJsonReader *reader);

static inline void c_json_reader_freep(CJsonReader **readerp) {
        if (*readerp)
                c_json_reader_free(*readerp);
}

#ifdef __cplusplus
}
#endif
