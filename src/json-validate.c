
#undef NDEBUG
#include <c-stdaux.h>
#include <stdio.h>
#include "c-json.h"

int read_file(FILE *file, char **contentsp) {
        _c_cleanup_ (c_fclosep) FILE *stream = NULL;
        _c_cleanup_ (c_freep) char *contents = NULL;
        size_t n_input;

        stream = open_memstream(&contents, &n_input);
        if (!stream)
                return -errno;

        for (;;) {
                char buffer[8192];
                size_t n;

                n = fread(buffer, 1, sizeof(buffer), file);
                if (n == 0) {
                        if (ferror(file))
                                return -errno;
                        break;
                }

                if (fwrite(buffer, 1, n, stream) != n)
                        return -errno;
        }

        stream = c_fclose(stream);
        *contentsp = contents;
        contents = NULL;

        return 0;
}

int json_read_value(CJson *json) {
        switch (c_json_peek(json)) {
                case C_JSON_TYPE_NULL:
                        return c_json_read_null(json);

                case C_JSON_TYPE_BOOLEAN:
                        return c_json_read_bool(json, NULL);

                case C_JSON_TYPE_STRING:
                        return c_json_read_string(json, NULL);

                case C_JSON_TYPE_NUMBER:
                        return c_json_read_number(json, NULL, NULL);

                case C_JSON_TYPE_ARRAY:
                        c_json_enter_array(json);
                        while (c_json_more(json)) {
                                int r = json_read_value(json);
                                if (r)
                                        return r;
                        }
                        return c_json_exit_array(json);

                case C_JSON_TYPE_OBJECT:
                        c_json_enter_object(json);
                        while (c_json_more(json)) {
                                int r = json_read_value(json);
                                if (r)
                                        return r;
                        }
                        return c_json_exit_object(json);

                default:
                        return C_JSON_E_INVALID_JSON;
        }
}

int main(int argc, char **argv) {
        _c_cleanup_ (c_fclosep) FILE *file = NULL;
        _c_cleanup_ (c_json_freep) CJson *json = NULL;
        _c_cleanup_ (c_freep) char *input = NULL;
        int r;

        if (argc < 2) {
                r = read_file(stdin, &input);
                if (r)
                        return 1;
        } else {
                file = fopen(argv[1], "r");
                if (!file)
                        return -errno;

                r = read_file(file, &input);
                if (r)
                        return 1;
        }

        c_json_new(&json, 256);
        c_json_begin_read(json, input);
        r = json_read_value(json);
        if (r)
                return r;

        return c_json_end_read(json);
}
