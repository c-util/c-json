
#undef NDEBUG
#include <c-stdaux.h>
#include <stdio.h>
#include "c-json.h"

int json_read_value(CJson *json) {
        int r = 0;

        switch (c_json_peek(json)) {
                case C_JSON_TYPE_NULL:
                        return c_json_read_null(json);

                case C_JSON_TYPE_BOOLEAN:
                        return c_json_read_bool(json, NULL);

                case C_JSON_TYPE_STRING:
                        return c_json_read_string(json, NULL);

                case C_JSON_TYPE_NUMBER:
                        return c_json_read_f64(json, NULL);

                case C_JSON_TYPE_ARRAY:
                        c_json_open_array(json);
                        while (!r && c_json_more(json))
                                r = json_read_value(json);
                        return c_json_close_array(json);

                case C_JSON_TYPE_OBJECT:
                        c_json_open_object(json);
                        while (!r && c_json_more(json))
                                r = json_read_value(json);
                        return c_json_close_object(json);

                default:
                        return C_JSON_E_INVALID_JSON;
        }
}

int main(int argc, char **argv) {
        _c_cleanup_ (c_json_freep) CJson *json = NULL;
        _c_cleanup_ (c_fclosep) FILE *file = NULL;
        _c_cleanup_ (c_fclosep) FILE *stream = NULL;
        _c_cleanup_ (c_freep) char *input = NULL;
        size_t n_input;
        int r;

        if (argc < 2) {
                fprintf(stderr, "Usage: %s filename\n", argv[0]);
                return 1;
        }

        stream = open_memstream(&input, &n_input);
        if (!stream) {
                perror("Error: ");
                return 1;
        }

        file = fopen(argv[1], "r");
        if (!file) {
                perror("Error: ");
                return 1;
        }

        for (;;) {
                char buffer[8192];
                int n;

                n = fread(buffer, 1, sizeof(buffer), file);
                if (n <= 0) {
                        if (ferror(file)) {
                                perror("Error: ");
                                return 1;
                        }
                        break;
                }

                if (fwrite(buffer, 1, n, stream) != n) {
                        perror("Error: ");
                        return 1;
                }
        }

        stream = c_fclose(stream);

        c_json_new(&json);
        c_json_begin_read(json, input);
        json_read_value(json);
        r = c_json_end_read(json);
        if (r) {
                /* JSONTestSuite's test cases start with 'y' (valid), 'n' (invalid), or i (either) */
                switch (r) {
                        case C_JSON_E_INVALID_JSON:
                        case C_JSON_E_DEPTH_OVERFLOW: {
                                char *filename = basename(argv[1]);
                                if (filename[0] == 'n' || filename[0] == 'i')
                                        return 0;
                        }

                        default:
                                return 1;
                }
        }

        return 0;
}
