
#undef NDEBUG
#include <assert.h>
#include <c-stdaux.h>
#include <string.h>
#include "c-json.h"

static void test_basic(void) {
        static CJsonReader *reader = NULL;
        _c_cleanup_(c_freep) char *string = NULL;
        const char *number;
        size_t n_number;
        bool b = false;

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "\"foo \\u00e4bc\"");
        assert(!c_json_reader_read_string(reader, &string));
        assert(string && !strcmp(string, "foo äbc"));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "12345678");
        assert(!c_json_reader_read_number(reader, &number, &n_number));
        assert(strcmp(number, "12345678") == 0);
        assert(n_number == strlen("12345678"));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "-1");
        assert(!c_json_reader_read_number(reader, &number, &n_number));
        assert(strcmp(number, "-1") == 0);
        assert(n_number == strlen("-1"));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "-3.14");
        assert(!c_json_reader_read_number(reader, &number, &n_number));
        assert(strcmp(number, "-3.14") == 0);
        assert(n_number == strlen("-3.14"));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "true");
        assert(!c_json_reader_read_bool(reader, &b));
        assert(b == true);
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "null");
        assert(!c_json_reader_read_null(reader));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);
}

static void test_array(void) {
        static CJsonReader *reader = NULL;

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "[]");
        assert(!c_json_reader_enter_array(reader));
        assert(!c_json_reader_more(reader));
        assert(!c_json_reader_exit_array(reader));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "[ 1, 2, 3, 4, 5, 6 ]");
        assert(!c_json_reader_enter_array(reader));
        for (uint64_t i = 1; i < 7; i += 1) {
                const char *number;
                unsigned long long n;

                assert(c_json_reader_more(reader));
                assert(!c_json_reader_read_number(reader, &number, NULL));
                n = strtoull(number, NULL, 0);
                assert(n == i);
        }
        assert(!c_json_reader_exit_array(reader));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);
}

static void test_object(void) {
        static CJsonReader *reader = NULL;
        static struct { const char *key; uint64_t value; } expected[] = {
                { "foo", 42 },
                { "bar", 43 }
        };

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "{}");
        assert(!c_json_reader_enter_object(reader));
        assert(!c_json_reader_more(reader));
        assert(!c_json_reader_exit_object(reader));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "{ \"foo\": 42, \"bar\": 43 }");
        assert(!c_json_reader_enter_object(reader));
        for (uint64_t i = 0; i < 2; i += 1) {
                _c_cleanup_(c_freep) char *key = NULL;
                const char *number;
                uint64_t n;

                assert(c_json_reader_more(reader));
                assert(!c_json_reader_read_string(reader, &key));
                assert(!strcmp(expected[i].key, key));
                assert(!c_json_reader_read_number(reader, &number, NULL));
                n = strtoull(number, NULL, 0);
                assert(expected[i].value == n);
        }
        assert(!c_json_reader_exit_object(reader));
        assert(!c_json_reader_end_read(reader));
        reader = c_json_reader_free(reader);
}

static void test_peek(void) {
        static CJsonReader *reader = NULL;

        assert(!c_json_reader_new(&reader, 256));
        c_json_reader_begin_read(reader, "{ \"foo\": \"bar\", \"bar\": 42, \"baz\": true }");

        assert(c_json_reader_peek(reader) == C_JSON_TYPE_OBJECT);
        assert(!c_json_reader_enter_object(reader));

        assert(c_json_reader_peek(reader) == C_JSON_TYPE_STRING);
        assert(!c_json_reader_read_string(reader, NULL));
        assert(c_json_reader_peek(reader) == C_JSON_TYPE_STRING);
        assert(!c_json_reader_read_string(reader, NULL));

        assert(c_json_reader_peek(reader) == C_JSON_TYPE_STRING);
        assert(!c_json_reader_read_string(reader, NULL));
        assert(c_json_reader_peek(reader) == C_JSON_TYPE_NUMBER);
        assert(!c_json_reader_read_number(reader, NULL, NULL));

        assert(c_json_reader_peek(reader) == C_JSON_TYPE_STRING);
        assert(!c_json_reader_read_string(reader, NULL));
        assert(c_json_reader_peek(reader) == C_JSON_TYPE_BOOLEAN);
        assert(!c_json_reader_read_bool(reader, NULL));

        assert(c_json_reader_peek(reader) == -1);
        assert(!c_json_reader_exit_object(reader));
        reader = c_json_reader_free(reader);
}

int main(int argc, char **argv) {
        test_basic();
        test_array();
        test_object();
        test_peek();
        return 0;
}
