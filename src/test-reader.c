
#undef NDEBUG
#include <c-stdaux.h>
#include <stdio.h>
#include "c-json.h"

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

        r = 0;
        while (!r && c_json_more(json)) {
                switch (c_json_peek(json)) {
                        case C_JSON_TYPE_NULL:
                                r = c_json_read_null(json);
                                break;

                        case C_JSON_TYPE_BOOLEAN:
                                r = c_json_read_bool(json, NULL);
                                break;

                        case C_JSON_TYPE_STRING:
                                r = c_json_read_string(json, NULL);
                                break;

                        case C_JSON_TYPE_NUMBER:
                                r = c_json_read_f64(json, NULL);
                                break;

                        case C_JSON_TYPE_ARRAY:
                                r = c_json_open_array(json);
                                break;

                        case C_JSON_TYPE_ARRAY_END:
                                r = c_json_close_array(json);
                                break;

                        case C_JSON_TYPE_OBJECT:
                                r = c_json_open_object(json);
                                break;

                        case C_JSON_TYPE_OBJECT_END:
                                r = c_json_close_object(json);
                                break;

                        default:
                                r = C_JSON_E_INVALID_JSON;
                                break;
                }
        }

        r = c_json_end_read(json);
        if (r) {
                /* JSONTestSuite's test cases start with 'y' (valid) or 'n' (invalid) */
                if (r == C_JSON_E_INVALID_JSON)
                        return argv[0][1] == 'n';
                return 1;
        }

        return 0;
}
