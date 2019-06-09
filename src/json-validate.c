
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

int json_read_value(CJsonReader *reader) {
        switch (c_json_reader_peek(reader)) {
                case C_JSON_TYPE_NULL:
                        return c_json_reader_read_null(reader);

                case C_JSON_TYPE_BOOLEAN:
                        return c_json_reader_read_bool(reader, NULL);

                case C_JSON_TYPE_STRING:
                        return c_json_reader_read_string(reader, NULL);

                case C_JSON_TYPE_NUMBER:
                        return c_json_reader_read_number(reader, NULL, NULL);

                case C_JSON_TYPE_ARRAY:
                        c_json_reader_enter_array(reader);
                        while (c_json_reader_more(reader)) {
                                int r = json_read_value(reader);
                                if (r)
                                        return r;
                        }
                        return c_json_reader_exit_array(reader);

                case C_JSON_TYPE_OBJECT:
                        c_json_reader_enter_object(reader);
                        while (c_json_reader_more(reader)) {
                                int r = json_read_value(reader);
                                if (r)
                                        return r;
                        }
                        return c_json_reader_exit_object(reader);

                default:
                        return C_JSON_E_INVALID_JSON;
        }
}

int main(int argc, char **argv) {
        _c_cleanup_ (c_fclosep) FILE *file = NULL;
        _c_cleanup_ (c_json_reader_freep) CJsonReader *reader = NULL;
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

        c_json_reader_new(&reader, 256);
        c_json_reader_begin_read(reader, input);
        r = json_read_value(reader);
        if (r)
                return r;

        return c_json_reader_end_read(reader);
}
