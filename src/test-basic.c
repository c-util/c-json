
#undef NDEBUG
#include <assert.h>
#include <c-stdaux.h>
#include <string.h>
#include "c-json.h"

static void test_basic(void) {
        static CJson *json = NULL;
        _c_cleanup_(c_freep) char *string = NULL;
        uint64_t u64 = 0;
        double f64 = 0;
        bool b = false;

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "\"foo \\u00e4bc\"");
        assert(!c_json_read_string(json, &string));
        assert(string && !strcmp(string, "foo äbc"));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "12345678");
        assert(!c_json_read_u64(json, &u64));
        assert(u64 == 12345678);
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "-1");
        assert(c_json_read_u64(json, &u64) == C_JSON_E_INVALID_TYPE);
        assert(c_json_end_read(json) == C_JSON_E_INVALID_TYPE);
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "-3.14");
        assert(!c_json_read_f64(json, &f64));
        assert(f64 == -3.14);
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
        assert(!c_json_open_array(json));
        assert(!c_json_more(json));
        assert(!c_json_close_array(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "[ 1, 2, 3, 4, 5, 6 ]");
        assert(!c_json_open_array(json));
        for (uint64_t i = 1; i < 7; i += 1) {
                uint64_t n;
                assert(c_json_more(json));
                assert(!c_json_read_u64(json, &n));
                        assert(n == i);
        }
        assert(!c_json_close_array(json));
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
        assert(!c_json_open_object(json));
        assert(!c_json_more(json));
        assert(!c_json_close_object(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "{ \"foo\": 42, \"bar\": 43 }");
        assert(!c_json_open_object(json));
        for (uint64_t i = 0; i < 2; i += 1) {
                _c_cleanup_(c_freep) char *key = NULL;
                uint64_t n;

                assert(c_json_more(json));
                assert(!c_json_read_string(json, &key));
                assert(!strcmp(expected[i].key, key));
                assert(!c_json_read_u64(json, &n));
                assert(expected[i].value == n);
        }
        assert(!c_json_close_object(json));
        assert(!c_json_end_read(json));
        json = c_json_free(json);
}

static void test_peek(void) {
        static CJson *json = NULL;

        assert(!c_json_new(&json, 256));
        c_json_begin_read(json, "{ \"foo\": \"bar\", \"bar\": 42, \"baz\": true }");

        assert(c_json_peek(json) == C_JSON_TYPE_OBJECT);
        assert(!c_json_open_object(json));

        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));
        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));

        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));
        assert(c_json_peek(json) == C_JSON_TYPE_NUMBER);
        assert(!c_json_read_u64(json, NULL));

        assert(c_json_peek(json) == C_JSON_TYPE_STRING);
        assert(!c_json_read_string(json, NULL));
        assert(c_json_peek(json) == C_JSON_TYPE_BOOLEAN);
        assert(!c_json_read_bool(json, NULL));

        assert(c_json_peek(json) == -1);
        assert(!c_json_close_object(json));
        json = c_json_free(json);
}

int main(int argc, char **argv) {
        test_basic();
        test_array();
        test_object();
        test_peek();
        return 0;
}
