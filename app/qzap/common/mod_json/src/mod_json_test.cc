

#include "app/qzap/common/mod_json/include/mod_json.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define MOD_TRACE(format, ...)  printf(format "\n", ##__VA_ARGS__)
#define MOD_ASSERT(cond)  do {if (!(cond)) { fprintf(stderr, "[ASSERT:%d] " #cond "\n", __LINE__); abort(); }} while(0)

void TEST_json_case0(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        MODFRAME_NS::json_string str = "{first: {int: 123, float: 1.0, "
            "true:[true, true, true, true], false:[false],  zero:[0,0,0]}, "
            "true:true, false:[false, false, false, false], zero:[0,0]}";

        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val.refer() == 1);

        const MODFRAME_NS::json_value &val2 = val;
        const MODFRAME_NS::json_object &obj2 = val2.as_object();

        MOD_ASSERT(val2.as_object().refer() == 1);
        MOD_ASSERT(obj2.get("first", MODFRAME_NS::json_value()).refer() == 1);
        MOD_ASSERT(obj2.get("first", MODFRAME_NS::json_value()).as_object().refer() == 1);
        MOD_ASSERT(obj2.get("true", MODFRAME_NS::json_value()).refer() == 5);
        MOD_ASSERT(obj2.get("false", MODFRAME_NS::json_value()).as_array().front().refer() == 5);
        MOD_ASSERT(obj2.get("zero", MODFRAME_NS::json_value()).as_array().at(1).refer() == 5);

        const MODFRAME_NS::json_value val3 = val;
        MOD_ASSERT(val3.refer() == 2);
        MOD_ASSERT(val3.as_object().refer() == 1);

        MODFRAME_NS::json_value val4 = val;
        MOD_ASSERT(val4.refer() == 3);
        MOD_ASSERT(val3.refer() == 3);
        MOD_ASSERT(val2.refer() == 3);

        MODFRAME_NS::json_object &obj4 = val4.as_object();
        MOD_ASSERT(obj4.refer() == 2);
        MOD_ASSERT(val4.refer() == 0);
        MOD_ASSERT(val3.refer() == 2);
        MOD_ASSERT(val3.as_object().refer() == 2);
        MOD_ASSERT(val2.refer() == 2);
    }

    {
        MODFRAME_NS::json_object obj;
        MODFRAME_NS::json_array arr;
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_string str;

        MOD_ASSERT(obj.refer() == -1);
        MOD_ASSERT(arr.refer() == -1);
        MOD_ASSERT(val.refer() == -1);
        MOD_ASSERT(str.refer() == -1);

        val = str;
        MOD_ASSERT(val.refer() == 1);

        val = obj;
        MOD_ASSERT(val.refer() == 1);

        val = arr;
        MOD_ASSERT(val.refer() == 1);

        arr.append("acdef");
        MOD_ASSERT(arr.refer() == 1);
        arr.begin();
        MOD_ASSERT(arr.refer() == 0);
        arr.end();
        MOD_ASSERT(arr.refer() == 0);

        MODFRAME_NS::json_array arr1 = arr;
        MODFRAME_NS::json_array arr2 = arr1;
        MODFRAME_NS::json_array arr3 = arr;
        MOD_ASSERT(arr1.refer() == 2);
        MOD_ASSERT(arr2.refer() == 2);
        MOD_ASSERT(arr3.refer() == 1);
        MOD_ASSERT(arr.refer() == 0);

        obj.set("1111", "null");
        MOD_ASSERT(obj.refer() == 1);
        obj.rbegin();
        MOD_ASSERT(obj.refer() == 0);
        obj.rend();
        MOD_ASSERT(obj.refer() == 0);

        MODFRAME_NS::json_object obj1 = obj;
        MODFRAME_NS::json_object obj2 = obj1;
        MODFRAME_NS::json_object obj3 = obj;
        MOD_ASSERT(obj1.refer() == 2);
        MOD_ASSERT(obj2.refer() == 2);
        MOD_ASSERT(obj3.refer() == 1);
        MOD_ASSERT(obj.refer() == 0);
    }

    {
        MODFRAME_NS::json_parser parser;
        MODFRAME_NS::json_string str = "{ 0:0, 1: 1, 2:2, 3:3, 4: 4, 5:5}";
        MODFRAME_NS::json_value val;

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val.refer() == 1);

        MODFRAME_NS::json_value val2 = val;
        MOD_ASSERT(val2.refer() == 2);

        MODFRAME_NS::json_object &obj2 = val2.as_object();
        MOD_ASSERT(val2.refer() == 0);
        MOD_ASSERT(obj2.refer() == 2);
        MOD_ASSERT(obj2["0"].refer() == 2);
        MOD_ASSERT(obj2["1"].refer() == 2);
        MOD_ASSERT(obj2["2"].refer() == 2);
        MOD_ASSERT(obj2["3"].refer() == 2);
        MOD_ASSERT(obj2["4"].refer() == 2);
        MOD_ASSERT(obj2["5"].refer() == 2);
        MOD_ASSERT(obj2.refer() == 0);

        MODFRAME_NS::json_value val3 = val;
        MOD_ASSERT(val3.refer() == 2);

        MODFRAME_NS::json_object::const_iterator iter = obj2.begin();
        MOD_ASSERT(iter->key().refer() == 2);
        MOD_ASSERT(iter->value().refer() == 2);
    }

    {
        MODFRAME_NS::json_parser parser;
        MODFRAME_NS::json_string str = "[0, 1, 2, 3, 4, 5]";
        MODFRAME_NS::json_value val;

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val.refer() == 1);

        MODFRAME_NS::json_value val2 = val;
        MOD_ASSERT(val2.refer() == 2);

        MODFRAME_NS::json_array &arr2 = val2.as_array();
        MOD_ASSERT(val2.refer() == 0);
        MOD_ASSERT(arr2.refer() == 2);
        MOD_ASSERT(arr2[0].refer() == 2);
        MOD_ASSERT(arr2[1].refer() == 2);
        MOD_ASSERT(arr2[2].refer() == 2);
        MOD_ASSERT(arr2[3].refer() == 2);
        MOD_ASSERT(arr2[4].refer() == 2);
        MOD_ASSERT(arr2[5].refer() == 2);
        MOD_ASSERT(arr2.refer() == 0);

        MODFRAME_NS::json_value val3 = val;
        MOD_ASSERT(val3.refer() == 2);

        MODFRAME_NS::json_array::const_iterator iter = arr2.begin();
        MOD_ASSERT(iter->refer() == 2);
    }

}

void TEST_json_case1(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{\"req\": {\"aid\": \"\", \"friendqq\": \"1234567890\", \"uintype\": 0}}";
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(!parser.parse(val, ""));

        MODFRAME_NS::json_value tmp;
        MOD_ASSERT(tmp.parse(str));
        MOD_ASSERT(tmp == val);
        MOD_ASSERT(!(tmp != val));

        const MODFRAME_NS::json_value &req = val["req"];
        MOD_ASSERT(req.is_object());
        MOD_ASSERT(req["qqshow"].as_uint() == 0);
        MOD_ASSERT(req["friendqq"].as_uint() == 1234567890);
        MOD_ASSERT(req[1].is_null());
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "[true, false, 0, 1, 2, \"3\"]";
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val[(MODFRAME_NS::size_type)0].as_bool());
        MOD_ASSERT(!val[1].as_bool());
        MOD_ASSERT(val[2].as_int() == 0);
        MOD_ASSERT(val[3].as_int() == 1);
        MOD_ASSERT(val[4].as_int() == 2);
        MOD_ASSERT(val[5].as_int() == 3);

        MODFRAME_NS::json_value tmp;
        MOD_ASSERT(tmp.parse(str));
        MOD_ASSERT(tmp == val);
        MOD_ASSERT(!(tmp != val));

        const MODFRAME_NS::json_value val2 = val;;
        MOD_ASSERT(val2[(MODFRAME_NS::size_type)0].as_bool());
        MOD_ASSERT(!val2[1].as_bool());
        MOD_ASSERT(val2[2].as_int() == 0);
        MOD_ASSERT(val2[3].as_int() == 1);
        MOD_ASSERT(val2[4].as_int() == 2);
        MOD_ASSERT(val2[5].as_int() == 3);
        MOD_ASSERT(val2[6].is_null());
        MOD_ASSERT(val2[-1].as_int() == 0);
    }

    {
        MODFRAME_NS::json_string a("abcdefg");
        MODFRAME_NS::json_string b("abcdefl");
        MODFRAME_NS::json_string c("abcdefg");
        MOD_ASSERT(a == c);
        MOD_ASSERT(b != c);
        MOD_ASSERT(b != a);

        MOD_ASSERT(a.compare(c) == 0);
        MOD_ASSERT(b.compare(c) != 0);
        MOD_ASSERT(b.compare(c) != 0);
    }

    {
        MODFRAME_NS::json_string a("abcdefg\"");
        MODFRAME_NS::json_string b("abcd");
        MODFRAME_NS::json_string c("abcdefg");
        MOD_ASSERT(a != c);
        MOD_ASSERT(b != c);
        MOD_ASSERT(b != a);
    }

    {
        MODFRAME_NS::json_string a("abcd\0efg");
        MODFRAME_NS::json_string b("abcd");
        MODFRAME_NS::json_string c("abcdefg\0");
        MOD_ASSERT(a != c);
        MOD_ASSERT(b != c);
        MOD_ASSERT(b == a);
        MOD_ASSERT(a.compare(b) == 0);
    }

    {
        MODFRAME_NS::json_string a("abcd\0efg", 8);
        MODFRAME_NS::json_string b("abcd");
        MODFRAME_NS::json_string c("abcdefg\0");
        MOD_ASSERT(a != c);
        MOD_ASSERT(b != c);
        MOD_ASSERT(b != a);

        MOD_ASSERT(a.compare("abcd") == 0);
        MOD_ASSERT(b.compare("abcd\0") == 0);
        MOD_ASSERT(c.compare("abcdefg") == 0);
    }

    {
        MODFRAME_NS::json_string a("abcd\0efg", 8);
        MODFRAME_NS::json_string b("abcd");
        MODFRAME_NS::json_string c("abcd\0efg", 8);
        MOD_ASSERT(a == c);
        MOD_ASSERT(b != c);
        MOD_ASSERT(b != a);
    }

    {
        MODFRAME_NS::json_string a;
        MODFRAME_NS::json_string b("\0");
        MODFRAME_NS::json_string c(NULL);
        MOD_ASSERT(a == c);
        MOD_ASSERT(b == c);
        MOD_ASSERT(b == a);
    }

    {
        MODFRAME_NS::json_string a;
        MODFRAME_NS::json_string b("\0", 1);
        MODFRAME_NS::json_string c(NULL);
        MOD_ASSERT(a == c);
        MOD_ASSERT(b != c);
        MOD_ASSERT(b != a);
    }

    {
        MODFRAME_NS::json_array arr1;
        MODFRAME_NS::json_array arr2;
        MODFRAME_NS::json_array arr3;
        MODFRAME_NS::json_array::iterator iter1;

        MOD_ASSERT(arr1.append("123456"));
        arr2 = arr1;
        iter1 = arr1.begin();
        arr3 = arr1;
        *iter1 = "abcdefg";
        MOD_ASSERT(arr1[0].as_string() == "abcdefg");
        MOD_ASSERT(arr2[0].as_string() == "123456");
        MOD_ASSERT(arr3[0].as_string() == "123456");
    }

    {
        MODFRAME_NS::json_array arr1;
        MODFRAME_NS::json_array arr2;
        MODFRAME_NS::json_array arr3;

        MOD_ASSERT(arr1.append("123456"));
        arr2 = arr1;

        MODFRAME_NS::json_value &val1 = arr1.front();
        arr3 = arr1;
        val1 = "abcdefg";
        MOD_ASSERT(arr1[0].as_string() == "abcdefg");
        MOD_ASSERT(arr2[0].as_string() == "123456");
        MOD_ASSERT(arr3[0].as_string() == "123456");
    }

    {
        MODFRAME_NS::json_object obj1;
        MODFRAME_NS::json_object obj2;
        MODFRAME_NS::json_object obj3;
        MODFRAME_NS::json_object::iterator iter1;

        MOD_ASSERT(obj1.set("aaa", "123456"));
        obj2 = obj1;
        iter1 = obj1.begin();
        obj3 = obj1;
        iter1->value() = "abcdefg";
        MOD_ASSERT(obj1["aaa"].as_string() == "abcdefg");
        MOD_ASSERT(obj2["aaa"].as_string() == "123456");
        MOD_ASSERT(obj3["aaa"].as_string() == "123456");
    }

    {
        MODFRAME_NS::json_value val1;
        MODFRAME_NS::json_value val2;
        MODFRAME_NS::json_value val3;
        MODFRAME_NS::json_value val4;

        val1 = "abcdef";
        val2 = val1;
        val3 = val1;

        MOD_ASSERT(val1.refer() == 3);
        MOD_ASSERT(val2.refer() == 3);
        MOD_ASSERT(val3.refer() == 3);
        MOD_ASSERT(val3.as_stlstring() == "abcdef");

        MODFRAME_NS::json_string &str = val1.as_string();
        MOD_ASSERT(str.refer() == 2);
        val4 = val1;
        str = "123456";

        MOD_ASSERT(val1.refer() == 0);
        MOD_ASSERT(val2.refer() == 2);
        MOD_ASSERT(val3.refer() == 2);
        MOD_ASSERT(val4.refer() == 1);
        MOD_ASSERT(val1.as_stlstring() == "123456");
        MOD_ASSERT(val2.as_stlstring() == "abcdef");
        MOD_ASSERT(val3.as_stlstring() == "abcdef");
        MOD_ASSERT(val4.as_stlstring() == "abcdef");
    }

    {
        MODFRAME_NS::json_value val1;
        MODFRAME_NS::json_value val2;
        MODFRAME_NS::json_value val3;

        val1["abcd"] = "1234";
        val2 = val1.as_object();
        val3 = val2;

        MOD_ASSERT(val1.refer() == 0);
        MOD_ASSERT(val2.refer() == 2);
        MOD_ASSERT(val3.refer() == 2);
        MOD_ASSERT(val1.as_object().refer() == 0);
        MOD_ASSERT(val2.as_object().refer() == 2);
        MOD_ASSERT(val3.as_object().refer() == 2);
    }

    {
        MODFRAME_NS::json_object obj1;

        obj1.set("FTitle", "123456789");
        obj1.set("FDesc", "abcdef");

        const MODFRAME_NS::json_object &obj2 = obj1;
        MOD_ASSERT(obj1["FTitle"].as_stlstring() == "123456789");
        MOD_ASSERT(obj1["FDesc"].as_stlstring() == "abcdef");
        MOD_ASSERT(obj2["FTitle"].as_stlstring() == "123456789");
        MOD_ASSERT(obj2["FDesc"].as_stlstring() == "abcdef");
    }

    {
        MODFRAME_NS::json_string str = "[ 15, true, null,\'\\u9701abcd \\u38981515\\u89454845\\uabcd\\uef12\',]";
        MODFRAME_NS::json_parser parser;
        MODFRAME_NS::json_value val(true);

        parser.set_squote();
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val[1].as_bool());
        val[1] = val[2];
        MOD_ASSERT(!val[1].as_bool());
    }

    {
        MODFRAME_NS::json_string str = "[ true,,\'\\u9701abcd \\u38981515\\u89454845\\uabcd\\uef12\'";
        MODFRAME_NS::json_parser parser;
        MODFRAME_NS::json_value val = MODFRAME_NS::json_value();

        MODFRAME_NS::json_value tmp;
        MOD_ASSERT(!tmp.parse(str));

        parser.set_squote();
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }
}

void TEST_json_case2()
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    MODFRAME_NS::json_parser parser;
    MODFRAME_NS::json_value val1, val2, val3, val4;

    MOD_ASSERT(parser.parse(val1, 
        "[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, "
        "6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,"
        "17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,"
        "\"32\",\"33\",\"34\",\"35\",\"36\","
        "{\"5\":5,\"4\":4,\"3\":3,\"2\":2,\"1\":1,\"0\":0,\"-1\":-1}]"
        ));
    MOD_ASSERT(parser.parse(val2, 
        "[\"0\",\"1\",\"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\","
        "\"9\",\"10\",\"11\",\"12\",\"13\",\"14\",\"15\",\"16\","
        "\"17\",\"18\",\"19\",\"20\",\"21\",\"22\",\"23\",\"24\","
        "\"25\",\"26\",\"27\",\"28\",\"29\",\"30\",\"31\","
        "\"32\",\"33\",\"34\",\"35\",\"36\","
        "{\"-2\":\"-2\",\"-1\":\"-1\",\"1\":\"1\",\"2\":\"2\","
        "\"3\":\"3\",\"4\":\"4\",\"5\":\"5\",\"6\":\"6\"},"
        "[],null,true,false,0.0,1.0,9.999,-1]"
        ));
    MOD_ASSERT(parser.parse(val3, 
        "[\"0\",\"1\",\"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\","
        "\"9\",\"10\",\"11\",\"12\",\"13\",\"14\",\"15\",\"16\","
        "\"17\",\"18\",\"19\",\"20\",\"21\",\"22\",\"23\",\"24\","
        "\"25\",\"26\",\"27\",\"28\",\"29\",\"30\",\"31\","
        "\"32\",\"33\",\"34\",\"35\",\"36\","
        "{\"5\":\"5\",\"4\":\"4\",\"3\":\"3\","
        "\"2\":\"2\",\"1\":\"1\",\"0\":0,"
        "\"-1\":\"-1\",\"-2\":\"-2\",\"6\":\"6\"},"
        "[],null,true,false,0.0,1.0,9.999,-1]"
        ));
    MOD_ASSERT(parser.parse(val4, 
        "[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, "
        "6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,"
        "17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,"
        "\"32\",\"33\",\"34\",\"35\",\"36\","
        "{\"-2\":\"-2\",\"-1\":-1,\"1\":1,\"2\":2,"
        "\"3\":3,\"4\":4,\"5\":5,\"6\":\"6\",\"0\":0},"
        "[],null,true,false,0.0,1.0,9.999,-1]"
        ));

    MODFRAME_NS::json_value tmp1 = val1;
    MOD_ASSERT(tmp1.merge(val2));

    MODFRAME_NS::json_value tmp2 = val2;
    MOD_ASSERT(tmp2.merge(val1));

    MODFRAME_NS::json_dumper dumper;
    MOD_ASSERT(dumper.dump(val1));
    MOD_ASSERT(dumper.dump(val2));
    MOD_ASSERT(dumper.dump(val3));
    MOD_ASSERT(dumper.dump(val4));
    MOD_ASSERT(dumper.dump(tmp1));

    MOD_ASSERT(tmp1.as_jsonstring() == val3.as_jsonstring());
    MOD_ASSERT(tmp2.as_jsonstring() == val4.as_jsonstring());

    {
        MODFRAME_NS::json_array arr;

        MOD_ASSERT(arr.resize(1023));
        MOD_ASSERT(arr.size() == 1023);
        MOD_ASSERT(arr.capacity() == 1024);
        MOD_ASSERT(arr[0].is_null());
        MOD_ASSERT(arr[1022].is_null());
    }

    {
        MODFRAME_NS::json_array arr;

        MOD_ASSERT(arr.capacity() == 0);
        MOD_ASSERT(arr.resize(0));
        MOD_ASSERT(arr.capacity() == 32);
        MOD_ASSERT(arr.append(0));
        MOD_ASSERT(arr.capacity() == 32);
        MOD_ASSERT(arr.size() == 1);
        MOD_ASSERT(arr.resize(0));
        MOD_ASSERT(arr.size() == 0);
        MOD_ASSERT(arr.resize(1));
        MOD_ASSERT(arr.capacity() == 32);
        MOD_ASSERT(arr.size() == 1);
    }

    {
        MODFRAME_NS::json_array arr;
        MODFRAME_NS::json_value val(666);

        MOD_ASSERT(arr.append("0"));
        MOD_ASSERT(arr.append(MODFRAME_NS::json_value(1)));
        MOD_ASSERT(arr.append(MODFRAME_NS::json_value(2)));
        MOD_ASSERT(arr.append("3"));
        MOD_ASSERT(arr.append("4"));
        MOD_ASSERT(arr.append("5"));
        MOD_ASSERT(arr.append("6"));
        MOD_ASSERT(arr.append(MODFRAME_NS::json_value(7.0)));
        MOD_ASSERT(arr.size() == 8);
        MOD_ASSERT(arr.capacity() == 32);
        MOD_ASSERT(arr[0].as_string() == "0");
        MOD_ASSERT(arr[1].as_int() == 1);
        MOD_ASSERT(arr[2].as_int() == 2);
        MOD_ASSERT(arr[3].as_int() == 3);
        MOD_ASSERT(arr[4].as_int() == 4);
        MOD_ASSERT(arr[5].as_int() == 5);
        MOD_ASSERT(arr[6].as_int() == 6);
        MOD_ASSERT(arr[7].as_int() == 7);
        MOD_ASSERT(arr.resize(20, val));
        MOD_ASSERT(arr.size() == 20);
        MOD_ASSERT(arr.resize(5, val));
        MOD_ASSERT(arr.size() == 5);
        MOD_ASSERT(arr[0].as_string() == "0");
        MOD_ASSERT(arr[1].as_int() == 1);
        MOD_ASSERT(arr[2].as_int() == 2);
        MOD_ASSERT(arr[3].as_string() == "3");
        MOD_ASSERT(arr[4].as_string() == "4");
        MOD_ASSERT(val.as_uint() == 666);
    }
}

void TEST_json_case3(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{abcd:\"1234\"}";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == std::string("1234"));

        parser.set_unstrict(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_uint() == 1234);
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "/*comments*/ { abcd\t  :  /* //comments */\"1234\" }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        parser.set_comment(false);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        parser.set_comment(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == std::string("1234"));

        parser.set_unstrict(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_uint() == 1234);
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ abcd/*  fff*/  :  /* //comments */\"1234\" }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        parser.set_comment(false);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        parser.set_comment(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == std::string("1234"));

        parser.set_unstrict(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_uint() == 1234);
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ \"abcd\\\"/*  fff*/  :  /* //comments */\"1234\" , {, [,  ]}}";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_unstrict(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ abcd///comments */\"1234\", [] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ abcd/*//*/ : \t  \"1234\" }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == NULL);
        MOD_ASSERT(val["abcd/*//*/"].as_cstring() == std::string("1234"));

        parser.set_comment(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == std::string("1234"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ ,{}, \"abcd/*//*/ : \t  \"1234\", }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ ccdd: [], abcd\" /*//*/ \n: \t  \"1234\" }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_comment(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == NULL);
        MOD_ASSERT(val["abcd\""].as_cstring() == std::string("1234"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{,, \"\" \n: \t  \"1234\" }";
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val[""].as_cstring() == std::string("1234"));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val[""].as_cstring() == std::string("1234"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ ,  \n: \t  \"1234\" }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));
        MOD_ASSERT(val[""].as_cstring() == NULL);

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ \'ccdd\': [], \'abcd\' /*//*/ \n: \t  \"1234\" }";

        parser.set_comment(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["\'abcd\'"].as_cstring() == std::string("1234"));

        parser.set_squote(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["abcd"].as_cstring() == std::string("1234"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ 1234 : \'abcd\', \'5678\' : [5, \'5678\'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["1234"].as_cstring() == std::string("abcd"));
        MOD_ASSERT(val["5678"].as_array().at(1).as_cstring() == std::string("5678"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ 1234 : \'ab\"cd\', \'5678\' : [\"5\", \'5678\'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["1234"].as_cstring() == std::string("ab\"cd"));
        MOD_ASSERT(val["5678"].as_array().at(1).as_cstring() == std::string("5678"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ 1234 : \'ab\\\'cd\', \'5678\' : [\"5\", \'5678\'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_unstrict(true);
        MOD_ASSERT(parser.parse(val, str.c_str()));
        MOD_ASSERT(val["1234"].as_cstring() == std::string("ab\\\'cd"));
        MOD_ASSERT(val["5678"].as_array().at(1).as_cstring() == std::string("5678"));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ \'1234\'\' : \'abcd\', \'5678\' : [\"5\", \'5678\'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_unstrict(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ \'1234\' : \'abcd\' \", \'5678\' : [\"5\", \'5678\'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_unstrict(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ \'1234\' : \'abcd\' , \'5678\' : [\"5\" \", \'5678\'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_unstrict(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_value val;
        MODFRAME_NS::json_parser parser;

        std::string str = "{ \'1234\' : \'abcd\' , \'5678\' : [\"5\" , \'5678\' \'] }";
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_squote(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_simple(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));

        parser.set_unstrict(true);
        MOD_ASSERT(!parser.parse(val, str.c_str()));
    }

    {
        MODFRAME_NS::json_array jarr;

        MOD_ASSERT(jarr.capacity() == 0);
        MOD_ASSERT(jarr.size() == 0);
        MOD_ASSERT(jarr.reserve(21));
        MOD_ASSERT(jarr.capacity() == 32);
        MOD_ASSERT(jarr.size() == 0);
        MOD_ASSERT(jarr.reserve(2));
        MOD_ASSERT(jarr.capacity() == 32);
        MOD_ASSERT(jarr.size() == 0);
        MOD_ASSERT(jarr.reserve(33));
        MOD_ASSERT(jarr.capacity() == 64);
        MOD_ASSERT(jarr.size() == 0);
    }

    {
        MODFRAME_NS::json_string jstr;

        MOD_ASSERT(jstr.capacity() == 0);
        MOD_ASSERT(jstr.size() == 0);
        MOD_ASSERT(jstr.reserve(21));
        MOD_ASSERT(jstr.capacity() == 32-1);
        MOD_ASSERT(jstr.size() == 0);
        MOD_ASSERT(jstr.reserve(2));
        MOD_ASSERT(jstr.capacity() == 32-1);
        MOD_ASSERT(jstr.size() == 0);
        MOD_ASSERT(jstr.reserve(32));
        MOD_ASSERT(jstr.capacity() == 64-1);
        MOD_ASSERT(jstr.size() == 0);

        MOD_ASSERT(jstr.assign("", 1000));
        MOD_ASSERT(jstr.capacity() == 1024-1);
        MOD_ASSERT(jstr.length() == 1000);
        MOD_ASSERT(MODFRAME_NS::json_string(jstr.c_str()) == "");

        MOD_ASSERT(jstr.assign("abcdef", 200));
        MOD_ASSERT(jstr.capacity() == 1024-1);
        MOD_ASSERT(jstr.length() == 200);
        MOD_ASSERT(MODFRAME_NS::json_string(jstr.c_str()) == "abcdef");
    }

    {
        MODFRAME_NS::json_value jval;

        MOD_ASSERT(jval.assign("aaaaaaaaaaaa"));
        MOD_ASSERT(jval.assign("122326263", 200));
        MOD_ASSERT(jval.assign(200));
        MOD_ASSERT(jval.assign(0xffffffffffff));
    }

    {
        MODFRAME_NS::json_object jobj;

        for (int i=0; i < 1000; ++i)
        {
            MODFRAME_NS::json_value key(i);
            jobj.set(key.as_jsonstring().c_str(), MODFRAME_NS::json_value((float)i));

            MODFRAME_NS::json_value val = jobj.get(key.as_jsonstring(), MODFRAME_NS::json_value());
            MOD_ASSERT(val.as_int() == i);
        }

        for (int i=0; i < 1000; ++i)
        {
            MODFRAME_NS::json_value key(i);
            jobj.unset(key.as_jsonstring().c_str());

            MODFRAME_NS::json_value val = jobj.get(key.as_jsonstring(), MODFRAME_NS::json_value());
            MOD_ASSERT(val.as_int() == 0);
        }
    }
}

void TEST_json_case4(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    {
        MODFRAME_NS::json_string str1("1234567890abcdefghijklmn");
        MOD_ASSERT(str1 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\\"1234\\\\567890abcdefghijklmn\\t");
        MODFRAME_NS::json_string str2 = "\"1234\\567890abcdefghijklmn\t";
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1(" \\/ \\\\ \\\" \\b \\f \\n \\r \\t ");
        MODFRAME_NS::json_string str2 = " / \\ \" \b \f \n \r \t ";
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\n\\r \\u8096 \\u5141 \\u950B \\u000a \\u000d");
        MODFRAME_NS::json_string str2("\n\r \xE8\x82\x96 \xE5\x85\x81 \xE9\x94\x8B \n \r");
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\u007f");
        MODFRAME_NS::json_string str2("\x7F");
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\u0080");
        MODFRAME_NS::json_string str2("\xC2\x80");
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\u07FF");
        MODFRAME_NS::json_string str2("\xDF\xBF");
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\u0800");
        MODFRAME_NS::json_string str2("\xE0\xA0\x80");
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1("\\uFFFF");
        MODFRAME_NS::json_string str2("\xEF\xBF\xBF");
        MOD_ASSERT(str2 == str1.decode());
    }

    {
        MODFRAME_NS::json_string str1(
            "author:\\u8096\\u5141\\u950b;\\r\\ntest:\\u007f \\u0080 \\u07ff \\u0800 \\uffff"
            );
        MODFRAME_NS::json_string str2(
            "author:\xE8\x82\x96\xE5\x85\x81\xE9\x94\x8B;\r\ntest:\x7F \xC2\x80 \xDF\xBF \xE0\xA0\x80 \xEF\xBF\xBF"
            );
        MODFRAME_NS::json_string str3(
            "author:\xE8\x82\x96\xE5\x85\x81\xE9\x94\x8B;\\r\\ntest:\x7F \xC2\x80 \xDF\xBF \xE0\xA0\x80 \xEF\xBF\xBF"
            );
        MOD_ASSERT(str2 == str1.decode());
        MOD_ASSERT(str2.encode() == str3);
    }

    {
        MODFRAME_NS::json_string str1(
            "\\007f \\0080 \\u07ff \\u0800 \\uffff"
            );
        MODFRAME_NS::json_string str2(
            "\\u008\\u07ff \\u0800 \\uffff"
            );
        MOD_ASSERT(!str1.decode().is_valid());
        MOD_ASSERT(!str2.decode().is_valid());
    }

    {
        MODFRAME_NS::json_string str1(" \x1f \x0e \x01 \x1e / \\ AAA\" AAA\b \f \n \r \t ");
        MODFRAME_NS::json_string str2(" \\u001f \\u000e \\u0001 \\u001e / \\\\ AAA\\\" AAA\\b \\f \\n \\r \\t ");
        MOD_ASSERT(str1.encode() == str2);
        MOD_ASSERT(str1 == str2.decode());
    }
}

void TEST_json_case5(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    MODFRAME_NS_USING;

    {
        MOD_ASSERT(json_value(true) == json_value(true));
        MOD_ASSERT(json_value(false) == json_value(false));
        MOD_ASSERT(json_value((char)'\r') == json_value(0xd));
        MOD_ASSERT(json_value((char)'\r') == json_value('\r'));
        MOD_ASSERT(json_value(10000) == json_value(10000));
        MOD_ASSERT(json_value(0xffff) == json_value(0xffff));
        MOD_ASSERT(json_value(0x10000) == json_value(0x10000));
        MOD_ASSERT(json_value(0xffffffff) == json_value(0xffffffff));
        MOD_ASSERT(json_value(0x100000000) == json_value(0x100000000));
        MOD_ASSERT(json_value(0xffffffffffffffff) == json_value(0xffffffffffffffff));
        MOD_ASSERT(json_value(0.999999) == json_value(0.999999));
        MOD_ASSERT(json_value(false) != json_value(0.0));
        MOD_ASSERT(json_value(0.0) != json_value(0));
        MOD_ASSERT(json_value("0.0") != json_value(0));
        MOD_ASSERT(json_value("0.0") == json_value("0.0"));
    }

    {
        MOD_ASSERT(json_string() == json_string());
        MOD_ASSERT(json_value() == json_value());
        MOD_ASSERT(json_object() == json_object());
        MOD_ASSERT(json_array() == json_array());

        MOD_ASSERT(!(json_string() != json_string()));
        MOD_ASSERT(!(json_value() != json_value()));
        MOD_ASSERT(!(json_object() != json_object()));
        MOD_ASSERT(!(json_array() != json_array()));

        MOD_ASSERT(json_string() != json_value());
        MOD_ASSERT(json_object() != json_value());
        MOD_ASSERT(json_array() != json_value());
        MOD_ASSERT(json_value() != json_string());
        MOD_ASSERT(json_value() != json_object());
        MOD_ASSERT(json_value() != json_array());

        MOD_ASSERT(!(json_string() == json_value()));
        MOD_ASSERT(!(json_object() == json_value()));
        MOD_ASSERT(!(json_array() == json_value()));
        MOD_ASSERT(!(json_value() == json_string()));
        MOD_ASSERT(!(json_value() == json_object()));
        MOD_ASSERT(!(json_value() == json_array()));

        MOD_ASSERT(json_string() == std::string());
        MOD_ASSERT(std::string() == json_string());
        MOD_ASSERT(!(json_string() != std::string()));
        MOD_ASSERT(!(std::string() != json_string()));

        MOD_ASSERT(json_string() == std::string(""));
        MOD_ASSERT(std::string("") == json_string());
        MOD_ASSERT(!(json_string() != std::string("")));
        MOD_ASSERT(!(std::string("") != json_string()));

        MOD_ASSERT(json_string("") == std::string());
        MOD_ASSERT(std::string() == json_string(""));
        MOD_ASSERT(!(json_string("") != std::string()));
        MOD_ASSERT(!(std::string() != json_string("")));
    }

    {
        json_array arr1;
        arr1.append(json_value(0.0));
        arr1.append(json_value(2));
        arr1.append("2");
        arr1.append(json_value(true));
        arr1.append(json_array());
        arr1.append(json_object());
        arr1.append(json_value());
        arr1.append(json_string());

        json_array arr2 = arr1;
        MOD_ASSERT(arr2 == arr1);

        json_array arr3;
        arr3.append(json_value(0.0));
        arr3.append(json_value(2));
        arr3.append("2");
        arr3.append(json_value(true));
        arr3.append(json_array());
        arr3.append(json_object());
        arr3.append(json_value());
        arr3.append(json_string());
        MOD_ASSERT(arr2 == arr3);
        MOD_ASSERT(arr1 == arr3);

        arr2.append(json_object());
        MOD_ASSERT(arr2 != arr3);
        MOD_ASSERT(arr2 != arr1);
        MOD_ASSERT(arr1 == arr3);
    }

    {
        json_value val1;
        json_value val2;
        json_string str = "{\"a\":1, \"b\":2, \"c\":3, \"string\":  \"string\", "
            "\"array\": [null, true, false, "", 0], \"object\": {\"a\":1.0, \"b\":2.0, \"c\":3.0}, "
            "\"true\": true, \"null\": null}";

        MOD_ASSERT(val1.parse(str));
        MOD_ASSERT(val2.parse(str));
        MOD_ASSERT(val2 == val1);
        MOD_ASSERT(!(val2 != val1));
        MOD_ASSERT(val2.as_object() == val1);
        MOD_ASSERT(val2 == val1.as_object());
        MOD_ASSERT(val2.as_object() == val1.as_object());
        MOD_ASSERT(val1["string"] == val2["string"]);
    }
}

void TEST_json_case6(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    MODFRAME_NS_USING;

    {
        json_value val;
        MOD_ASSERT(val.parse("[0,[0]]"));
        MOD_ASSERT(val.refer() == 1);
        MOD_ASSERT(val.as_array().refer() == 1);
        MOD_ASSERT(val.refer() == 0);

        MOD_ASSERT(val.as_array().append(val));
        MOD_ASSERT(val.as_jsonstring() == "[0,[0],[0,[0]]]");

        val.as_array().pop();
        MOD_ASSERT(val.as_array().push(val));
        MOD_ASSERT(val.as_jsonstring() == "[0,[0],[0,[0]]]");

        val.as_array().pop();
        val.as_array().pop();
        val.as_array().pop();
        MOD_ASSERT(val.as_array().push(val));
        MOD_ASSERT(val.as_jsonstring() == "[[]]");
    }

    {
        json_value val;
        MOD_ASSERT(val.parse("[0,[0]]"));
        MOD_ASSERT(val.refer() == 1);
        MOD_ASSERT(val.as_array().refer() == 1);
        MOD_ASSERT(val.refer() == 0);

        val.as_array()[0] = val;
        MOD_ASSERT(val.as_jsonstring() == "[[0,[0]],[0]]");
    }

    {
        json_value val;
        MOD_ASSERT(val.parse("{\"0\":[0]}"));
        MOD_ASSERT(val.refer() == 1);
        MOD_ASSERT(val.as_object().refer() == 1);
        MOD_ASSERT(val.refer() == 0);

        val.as_object()["1"].assign(val.as_object());
        MOD_ASSERT(val.as_jsonstring() == "{\"0\":[0],\"1\":{\"0\":[0]}}");
    }

    {
        json_value val;
        MOD_ASSERT(val.parse("{\"0\":[0]}"));
        MOD_ASSERT(val.refer() == 1);
        MOD_ASSERT(val.as_object().refer() == 1);
        MOD_ASSERT(val.refer() == 0);

        MOD_ASSERT(val.as_object().set("1", val));
        MOD_ASSERT(val.as_jsonstring() == "{\"0\":[0],\"1\":{\"0\":[0]}}");
    }

    {
        json_value val;
        MOD_ASSERT(val.parse("{\"0\":[0]}"));
        MOD_ASSERT(val.refer() == 1);
        MOD_ASSERT(val.as_object().refer() == 1);
        MOD_ASSERT(val.refer() == 0);

        val.as_object()["1"] = val;
        MOD_ASSERT(val.as_jsonstring() == "{\"0\":[0],\"1\":{\"0\":[0]}}");
    }

    {
        json_value val;
        MOD_ASSERT(val.parse("{\"0\":[0]}"));
        MOD_ASSERT(val.refer() == 1);
        MOD_ASSERT(val.as_object().refer() == 1);
        MOD_ASSERT(val.refer() == 0);

        val.as_object()["1"] = val.as_object();
        MOD_ASSERT(val.as_jsonstring() == "{\"0\":[0],\"1\":{\"0\":[0]}}");
    }
}

void TEST_json_case7(void)
{
    MOD_TRACE("ENTRY %s()", __FUNCTION__);

    MODFRAME_NS_USING;
	{
		json_value val;
		MOD_ASSERT(val.parse("[0, 1]"));
		MOD_ASSERT(val.as_array().front() == 0);
		MOD_ASSERT(val.as_array().front().as_uint() == 0);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[-1]"));
		MOD_ASSERT(val.as_array().front() == -1);
		MOD_ASSERT(val.as_array().front().as_int() == -1);
		MOD_ASSERT(val.as_array().front().as_long() == -1);
		MOD_ASSERT(val.as_array().front().as_llong() == -1);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[4294967295]"));
		MOD_ASSERT(val.parse("[+4294967295]"));
		MOD_ASSERT(val.as_array().front() == 4294967295);
		MOD_ASSERT(val.as_array().front().as_int() == -1);
		MOD_ASSERT(val.as_array().front().as_long() == 4294967295);
		MOD_ASSERT(val.as_array().front().as_llong() == 4294967295);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[ 2147483647 ]"));
		MOD_ASSERT(val.parse("[ +2147483647 ]"));
		MOD_ASSERT(val.as_array().front() == 2147483647);
		MOD_ASSERT(val.as_array().front().as_int() == 2147483647);
		MOD_ASSERT(val.as_array().front().as_long() == 2147483647);
		MOD_ASSERT(val.as_array().front().as_llong() == 2147483647);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[ -2147483647 ]"));
		MOD_ASSERT(val.as_array().front() == -2147483647);
		MOD_ASSERT(val.as_array().front().as_int() == -2147483647);
		MOD_ASSERT(val.as_array().front().as_long() == -2147483647);
		MOD_ASSERT(val.as_array().front().as_llong() == -2147483647);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[9223372036854775807]"));
		MOD_ASSERT(val.parse("[+9223372036854775807]"));
		MOD_ASSERT(val.as_array().front() == 9223372036854775807uLL);
		MOD_ASSERT(val.as_array().front().as_int() == -1);
		MOD_ASSERT(val.as_array().front().as_llong() == 9223372036854775807uLL);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[-9223372036854775807]"));
		MOD_ASSERT(val.as_array().front() == -9223372036854775807uLL);
		MOD_ASSERT(val.as_array().front().as_llong() == -9223372036854775807uLL);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[ 1844674407370955161 ]"));
		MOD_ASSERT(val.parse("[ +1844674407370955161 ]"));
		MOD_ASSERT(val.as_array().front() == 1844674407370955161uLL);
		MOD_ASSERT(val.as_array().front().as_llong() == 1844674407370955161uLL);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[ 18446744073709551615 ]"));
		MOD_ASSERT(val.parse("[ +18446744073709551615 ]"));
		MOD_ASSERT(val.as_array().front() == 18446744073709551615uLL);
		MOD_ASSERT(val.as_array().front().as_int() == -1);
		MOD_ASSERT(val.as_array().front().as_llong() == 18446744073709551615uLL);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[ 18446744073709551616 ]"));
		MOD_ASSERT(!val.as_array().front().is_integer());
		MOD_ASSERT(val.as_array().front() == 18446744073709551616.0);
		MOD_ASSERT(val.as_array().front().as_float() == 18446744073709551616.0);
	}

	{
		json_value val;
		MOD_ASSERT(val.parse("[ 1e+30, 1.3e12 ]"));
		MOD_ASSERT(val.as_array().front() == 1e+30);
		MOD_ASSERT(val.as_array().back() == 1.3e12);
	}

    {
        short a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        unsigned short a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        int a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        unsigned int a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        long a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        unsigned long a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        long long a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        unsigned long long a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        float a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }

    {
        double a = 1;
        enum { A = 1, B = 2 };
        MOD_ASSERT(a == A);
        MOD_ASSERT(A == a);
        MOD_ASSERT(a != B);
        MOD_ASSERT(B != a);
    }
}

int main()
{
    TEST_json_case0();
    TEST_json_case1();
    TEST_json_case2();
    TEST_json_case3();
    TEST_json_case4();
    TEST_json_case5();
    TEST_json_case6();
	TEST_json_case7();

    return 0;
}
