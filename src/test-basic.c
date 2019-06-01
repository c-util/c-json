
#undef NDEBUG
#include <assert.h>
#include <c-stdaux.h>
#include <string.h>
#include "c-json.h"

static void test_basic(void) {
        static CJson *json = NULL;
        _c_cleanup_(c_freep) char *string = NULL;
        const char *number;
        size_t n_number;
        bool b = false;

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "\"foo \\u00e4bc\"");
        assert(!c_json_read_string(json, &string));
        assert(string && !strcmp(string, "foo äbc"));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "12345678");
        assert(!c_json_read_number(json, &number, &n_number));
        assert(strcmp(number, "12345678") == 0);
        assert(n_number == strlen("12345678"));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "-1");
        assert(!c_json_read_number(json, &number, &n_number));
        assert(strcmp(number, "-1") == 0);
        assert(n_number == strlen("-1"));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "-3.14");
        assert(!c_json_read_number(json, &number, &n_number));
        assert(strcmp(number, "-3.14") == 0);
        assert(n_number == strlen("-3.14"));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "true");
        assert(!c_json_read_bool(json, &b));
        assert(b == true);
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "null");
        assert(!c_json_read_null(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);
}

static void test_array(void) {
        static CJson *json = NULL;

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "[]");
        assert(!c_json_enter_array(json));
        assert(!c_json_more(json));
        assert(!c_json_exit_array(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "[ 1, 2, 3, 4, 5, 6 ]");
        assert(!c_json_enter_array(json));
        for (uint64_t i = 1; i < 7; i += 1) {
                const char *number;
                unsigned long long n;

                assert(c_json_more(json));
                assert(!c_json_read_number(json, &number, NULL));
                n = strtoull(number, NULL, 0);
                assert(n == i);
        }
        assert(!c_json_exit_array(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);
}

static void test_object(void) {
        static CJson *json = NULL;
        static struct { const char *key; uint64_t value; } expected[] = {
                { "foo", 42 },
                { "bar", 43 }
        };

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "{}");
        assert(!c_json_enter_object(json));
        assert(!c_json_more(json));
        assert(!c_json_exit_object(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "{ \"foo\": 42, \"bar\": 43 }");
        assert(!c_json_enter_object(json));
        for (uint64_t i = 0; i < 2; i += 1) {
                _c_cleanup_(c_freep) char *key = NULL;
                const char *number;
                uint64_t n;

                assert(c_json_more(json));
                assert(!c_json_read_string(json, &key));
                assert(!strcmp(expected[i].key, key));
                assert(!c_json_read_number(json, &number, NULL));
                n = strtoull(number, NULL, 0);
                assert(expected[i].value == n);
        }
        assert(!c_json_exit_object(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);
}

static void test_peek(void) {
        static CJson *json = NULL;

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "{ \"foo\": \"bar\", \"bar\": 42, \"baz\": true }");

        assert(c_json_peek(json) == C_JSON_TYPE_OBJECT);
        assert(!c_json_enter_object(json));

        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));
        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));

        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));
        assert(c_json_peek(json) == C_JSON_TYPE_NUMBER);
        assert(!c_json_read_number(json, NULL, NULL));

        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));
        assert(c_json_peek(json) == C_JSON_TYPE_BOOLEAN);
        assert(!c_json_read_bool(json, NULL));

        assert(c_json_peek(json) == -1);
        assert(!c_json_exit_object(json));
        json = c_json_free(json);
}

int main(int argc, char **argv) {
        test_basic();
        test_array();
        test_object();
        test_peek();
        return 0;
}
