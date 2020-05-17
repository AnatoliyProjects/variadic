// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#define BOOST_TEST_MODULE variadic_unit_test
#include <boost/test/included/unit_test.hpp>

/* varidic */
#include <variadic.hpp>

/* boost/preprocessor */
#include <boost/preprocessor/variadic/to_list.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include <boost/preprocessor/punctuation/comma.hpp>
/* boost/test */
#include <boost/test/data/test_case.hpp>

// Global fixture setup

/*1*/
#define STR(param) BOOST_PP_STRINGIZE(param)
/*2*/
#define PACK(params...) \
    { BOOST_PP_LIST_FOR_EACH( \
            PACK_OP, \
            BOOST_PP_EMPTY(), \
            BOOST_PP_VARIADIC_TO_LIST(params) \
        ) }
#define PACK_OP(r, data, elem) (elem)
/*3*/
#define TYPE_PARAM     0
#define TEMPLATE_PARAM 1
/*4*/
#define PARAM_SET VDC_MAKE_PARAM_SET(TYPE_PARAM, TEMPLATE_PARAM)



/* VDC_MAKE_PARAM_SET */
BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "testing VDC_MAKE_PARAM_SET(params...) "
    "from <variadic/param_set.hpp>"))
BOOST_AUTO_TEST_SUITE(make_param_set)

/*1*/
BOOST_AUTO_TEST_CASE(empty_test,
    * boost::unit_test::label("empty")
    * boost::unit_test::description(
    "'params...' == []"))
{
    // wrong input for VDC_PARAM_LIST!
    BOOST_TEST(STR(VDC_MAKE_PARAM_SET()) == "()");
};

/*2*/
BOOST_AUTO_TEST_CASE(from_type_test,
    * boost::unit_test::description(
    "'params...' == [TYPE]"))
{
    BOOST_TEST(STR(VDC_MAKE_PARAM_SET(TYPE_PARAM)) == "(0)");
};

/*3*/
BOOST_AUTO_TEST_CASE(from_type_and_template_test,
    * boost::unit_test::description(
    "'params...' == [TYPE, TEMPLATE]"))
{
    BOOST_TEST(STR(VDC_MAKE_PARAM_SET(TYPE_PARAM, TEMPLATE_PARAM)) == "(0)(1)");
};

BOOST_AUTO_TEST_SUITE_END() /* VDC_MAKE_PARAM_SET */



/* VDC_PARAM_LIST */
BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "testing VDC_PARAM_LIST(param_set, max_arity, handler) "
    "from <variadic/core.hpp>")
    * boost::unit_test::depends_on("make_param_set"))
BOOST_AUTO_TEST_SUITE(param_list)

// Suite fixture setup
#define PRINT(r, params) { params }

/*1*/
BOOST_AUTO_TEST_CASE(empty_test,
    * boost::unit_test::label("empty")
    * boost::unit_test::description(
    "'param_set' == {}"))
{
    #define IN VDC_PARAM_LIST(BOOST_PP_EMPTY(), 2, PRINT)
    BOOST_TEST(STR(IN) == "");
    #undef IN
};

/*2*/
const char* arity_test_in  [] =
{
    #define ENUM_OP(z, n, data) STR(VDC_PARAM_LIST(PARAM_SET, n, PRINT))
    BOOST_PP_ENUM(4, ENUM_OP, BOOST_PP_EMPTY())
    #undef ENUM_OP
};

const char* arity_test_out [] =
{
    //on param set:
    //{TYPE_PARAM, TEMPLATE_PARAM} == {0, 1}

    //arity 0
    "" ,
    //arity 1
    "{ (0) } { (1) }" ,
    //arity 2
    "{ (0) } { (1) } { (0) (0) } { (0) (1) } { (1) (0) } { (1) (1) }" ,
    //arity 3
    "{ (0) } { (1) } { (0) (0) } { (0) (1) } { (1) (0) } { (1) (1) } "
    "{ (0) (0) (0) } { (0) (0) (1) } { (0) (1) (0) } { (0) (1) (1) } "
    "{ (1) (0) (0) } { (1) (0) (1) } { (1) (1) (0) } { (1) (1) (1) }"
};

BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "'arity' == 0-3"))
BOOST_DATA_TEST_CASE(
    arity_test,
    boost::unit_test::data::make(arity_test_in)^
    boost::unit_test::data::make(arity_test_out),
    in, out)
{
    BOOST_TEST(in == out);
};

// Suite fixture cleanup
#undef PRINT

BOOST_AUTO_TEST_SUITE_END() /* VDC_PARAM_LIST */



/* VDC_EMPTY */
BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "testing VDC_EMPTY(handler) "
    "from <variadic/core.hpp>"))
BOOST_AUTO_TEST_SUITE(empty)

// Suite fixture setup
#define PRINT(r, params) { params }

BOOST_AUTO_TEST_CASE(general_test,
    * boost::unit_test::description(
    "'handler' == PRINT(r, params)"))
{
    #define IN VDC_EMPTY(PRINT)
    BOOST_TEST(STR(IN) == STR({ (VDC_DETAIL_MOCK_PARAM) }));
    #undef IN
};

// Suite fixture cleanup
#undef PRINT

BOOST_AUTO_TEST_SUITE_END() /* VDC_EMPTY */



/* VDC_SIZE_OF */
BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "testing VDC_SIZE_OF(targets, params) "
    "from <variadic/size_of.hpp>"))
BOOST_AUTO_TEST_SUITE(size_of)

// Suite fixture setup
#define EMPTY_SEQ
#define SEQ \
    VDC_MAKE_PARAM_SET(TYPE_PARAM, TEMPLATE_PARAM, TYPE_PARAM)

/*1*/
BOOST_AUTO_TEST_CASE(empty_test,
    * boost::unit_test::label("empty")
    * boost::unit_test::description(
    "'params' == {}"))
{
    #define IN VDC_SIZE_OF(VDC_ANY_PARAM, EMPTY_SEQ)
    BOOST_TEST(IN == 0);
    #undef IN
};

/*2*/
BOOST_AUTO_TEST_CASE(any_target_test,
    * boost::unit_test::description(
    "'targets' == VDC_ANY_PARAM"))
{
    #define IN VDC_SIZE_OF(VDC_ANY_PARAM, SEQ)
    BOOST_TEST(IN == 3);
    #undef IN
};

/*3*/
BOOST_AUTO_TEST_CASE(type_target_test,
    * boost::unit_test::description(
    "'targets' == TYPE_PARAM"))
{
    #define IN VDC_SIZE_OF(TYPE_PARAM, SEQ)
    BOOST_TEST(IN == 2);
    #undef IN
};

/*4*/
BOOST_AUTO_TEST_CASE(template_target_test,
    * boost::unit_test::description(
    "'targets' == TEMPLATE_PARAM"))
{
    #define IN VDC_SIZE_OF(TEMPLATE_PARAM, SEQ)
    BOOST_TEST(IN == 1);
    #undef IN
};

/*5*/
BOOST_AUTO_TEST_CASE(mock_target_test,
    * boost::unit_test::description(
    "'targets' == VDC_DETAIL_MOCK_PARAM"))
{
    #define IN VDC_SIZE_OF(VDC_DETAIL_MOCK_PARAM, SEQ)
    BOOST_TEST(IN == 0);
    #undef IN
};

/*6*/
BOOST_AUTO_TEST_CASE(unary_tuple_target_test,
    * boost::unit_test::description(
    "'targets' == (TYPE_PARAM)"))
{
    #define IN VDC_SIZE_OF((TYPE_PARAM), SEQ)
    BOOST_TEST(IN == 2);
    #undef IN
};

/*7*/
BOOST_AUTO_TEST_CASE(binary_tuple_target_test,
    * boost::unit_test::description(
    "'targets' == (TYPE_PARAM, TEMPLATE_PARAM)"))
{
    #define IN VDC_SIZE_OF((TYPE_PARAM, TEMPLATE_PARAM), SEQ)
    BOOST_TEST(IN == 3);
    #undef IN
};

// Suite fixture cleanup
#undef EMPTY_SEQ
#undef SEQ

BOOST_AUTO_TEST_SUITE_END() /* VDC_SIZE_OF */



/* VDC_EXPAND */
BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "testing VDC_EXPAND(params, targets, expr) "
    "from <variadic/expand.hpp>")
    * boost::unit_test::depends_on("size_of")
    * boost::unit_test::depends_on("param_list"))
BOOST_AUTO_TEST_SUITE(expand)

// Suite fixture setup

/*1*/
#define EXPR(pos) \
    VDC_TOKEN_FN_IF(TYPE_PARAM, PRINT_T, pos) \
    VDC_TOKEN_FN_IF(TEMPLATE_PARAM, PRINT_F, pos)
/*2*/
#define PRINT_A(n) BOOST_PP_CAT(A, n) //any param
#define PRINT_T(n) BOOST_PP_CAT(T, n) //type param
#define PRINT_F(n) BOOST_PP_CAT(F, n) //template param
/*3*/
#define PARAM_LIST(handler) \
    STR(VDC_PARAM_LIST(PARAM_SET, 2, handler))

// Testing

/*1*/
BOOST_AUTO_TEST_CASE(empty_test,
    * boost::unit_test::label("empty")
    * boost::unit_test::description(
    "'param_set' in VDC_PARAM_LIST == {}"))
{
    #define IN VDC_PARAM_LIST(BOOST_PP_EMPTY(), 2, HANDLER)
    #define HANDLER(r, params) \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN(A, VDC_PLAIN))
    BOOST_TEST(STR(IN) == "");
    #undef IN
    #undef HANDLER
};

const char* numbering_test_in [] =
{
    #define PLAIN(r, params)   \
        PACK(VDC_EXPAND(params, VDC_ANY_PARAM, EXPR(VDC_PLAIN)))
    #define ABS_POS(r, params) \
        PACK(VDC_EXPAND(params, VDC_ANY_PARAM, EXPR(VDC_ABS_POS)))
    #define REL_POS(r, params) \
        PACK(VDC_EXPAND(params, VDC_ANY_PARAM, EXPR(VDC_REL_POS)))
    PARAM_LIST(PLAIN),
    PARAM_LIST(ABS_POS),
    PARAM_LIST(REL_POS)
    #undef PLAIN
    #undef ABS_POS
    #undef REL_POS
};

const char* numbering_test_out [] =
{
    //on param list:
    //{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }
    
    //VDC_PLAIN
    "{ (T) } { (F) } { (T) (T) } { (T) (F) } { (F) (T) } { (F) (F) }",
    //VDC_ABS_POS
    "{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F1) } { (F0) (T1) } { (F0) (F1) }",
    //VDC_REL_POS
    "{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }"
};

BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "'pos' in VDC_TOKEN == VDC_PLAIN|VDC_ABS_POS|VDC_REL_POS"))
BOOST_DATA_TEST_CASE(
    numbering_test,
    boost::unit_test::data::make(numbering_test_in)^
    boost::unit_test::data::make(numbering_test_out),
    in, out)
{
    BOOST_TEST(in == out);
};

/*2*/
const char* targets_test_in [] =
{
    #define ANY(r, params)      \
        PACK(VDC_EXPAND(params, VDC_ANY_PARAM, EXPR(VDC_REL_POS)))
    #define TYPE(r, params)     \
        PACK(VDC_EXPAND(params, TYPE_PARAM, EXPR(VDC_REL_POS)))
    #define TEMPLATE(r, params) \
        PACK(VDC_EXPAND(params, TEMPLATE_PARAM, EXPR(VDC_REL_POS)))
    #define MULTIPLE1(r, params) \
        PACK(VDC_EXPAND(params, (VDC_ANY_PARAM), EXPR(VDC_REL_POS)))
    #define MULTIPLE2(r, params) \
        PACK( \
            VDC_EXPAND(params, (TEMPLATE_PARAM, TYPE_PARAM), EXPR(VDC_REL_POS)))
    PARAM_LIST(ANY),
    PARAM_LIST(TYPE),
    PARAM_LIST(TEMPLATE),
    PARAM_LIST(MULTIPLE1)
    PARAM_LIST(MULTIPLE2)
    #undef ANY
    #undef TYPE
    #undef TEMPLATE
    #undef MULTIPLE1
    #undef MULTIPLE2
};

const char* targets_test_out [] =
{
    //on param list:
    //{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }
    
    //VDC_ANY_PARAM
    "{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }",
    //TYPE_PARAM
    "{ (T0) } { () } { (T0) (T1) } { (T0) } { (T0) } { () }",
    //TEMPLATE_PARAM
    "{ () } { (F0) } { () } { (F0) } { (F0) } { (F0) (F1) }",
    //(VDC_ANY_PARAM)
    "{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }"
    //(TYPE_PARAM, TEMPLATE_PARAM)
    "{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }"
};

BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "'targets' == TYPE_PARAM|TEMPLATE_PARAM| "
    "(VDC_ANY_PARAM)|(TYPE_PARAM, TEMPLATE_PARAM)"))
BOOST_DATA_TEST_CASE(
    targets_test,
    boost::unit_test::data::make(targets_test_in)^
    boost::unit_test::data::make(targets_test_out),
    in, out)
{
    BOOST_TEST(in == out);
};

/*3*/
const char* token_test_in [] =
{
    #define TOKEN(r, params)    \
        PACK(VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN(A, VDC_ABS_POS)))
    #define TOKEN_IF_TYPE(r, params) \
        PACK( \
            VDC_EXPAND( \
                params, \
                VDC_ANY_PARAM, \
                VDC_TOKEN_IF(TYPE_PARAM, T, VDC_REL_POS) \
            ) \
        )
    #define TOKEN_IF_TEMPLATE(r, params) \
        PACK( \
            VDC_EXPAND( \
                params, \
                VDC_ANY_PARAM, \
                VDC_TOKEN_IF(TEMPLATE_PARAM, F, VDC_REL_POS) \
            ) \
        )
    #define TOKEN_FN(r, params)  \
        PACK( \
            VDC_EXPAND( \
                params, \
                VDC_ANY_PARAM, \
                VDC_TOKEN_FN(PRINT_A, VDC_ABS_POS) \
            ) \
        )
    #define TOKEN_FN_IF(r, params) \
        PACK( \
            VDC_EXPAND( \
                params, \
                VDC_ANY_PARAM, \
                VDC_TOKEN_FN_IF(TYPE_PARAM, PRINT_T, VDC_REL_POS)     \
                VDC_TOKEN_FN_IF(TEMPLATE_PARAM, PRINT_F, VDC_REL_POS) \
            ) \
        )
    PARAM_LIST(TOKEN),
    PARAM_LIST(TOKEN_IF_TYPE),
    PARAM_LIST(TOKEN_IF_TEMPLATE),
    PARAM_LIST(TOKEN_FN),
    PARAM_LIST(TOKEN_FN_IF)
    #undef TOKEN
    #undef TOKEN_IF_TYPE
    #undef TOKEN_IF_TEMPLATE
    #undef TOKEN_FN
    #undef TOKEN_FN_IF
};

const char* token_test_out [] =
{
    //on param list:
    //{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }
    
    //VDC_TOKEN(A, VDC_ABS_POS)
    "{ (A0) } { (A0) } { (A0) (A1) } { (A0) (A1) } { (A0) (A1) } { (A0) (A1) }",
    //VDC_TOKEN_IF(TYPE_PARAM, T, VDC_REL_POS)
    "{ (T0) } { () } { (T0) (T1) } { (T0) () } { () (T0) } { () () }",
    //VDC_TOKEN_IF(TEMPLATE_PARAM, F, VDC_REL_POS)
    "{ () } { (F0) } { () () } { () (F0) } { (F0) () } { (F0) (F1) }",
    //VDC_TOKEN_FN(PRINT_A, VDC_ABS_POS)
    "{ (A0) } { (A0) } { (A0) (A1) } { (A0) (A1) } { (A0) (A1) } { (A0) (A1) }",
    //VDC_TOKEN_FN_IF(TYPE_PARAM, PRINT_T, VDC_REL_POS)
    //VDC_TOKEN_FN_IF(TEMPLATE_PARAM, PRINT_F, VDC_REL_POS)
    "{ (T0) } { (F0) } { (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }"
};

BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "'expr' == TOKEN|TOKEN_IF|TOKEN_FN|TOKEN_FN_IF"))
BOOST_DATA_TEST_CASE(
    token_test,
    boost::unit_test::data::make(targets_test_in)^
    boost::unit_test::data::make(targets_test_out),
    in, out)
{
    BOOST_TEST(in == out);
};

/*4*/
BOOST_AUTO_TEST_CASE(q_comma_str_test,
    * boost::unit_test::description(
    "'expr' contains VDC_TOKEN, which expands to quoted comma"))
{
    #define IN PARAM_LIST(COMMA_HANDLER)
    #define COMMA_HANDLER(r, params) \
        PACK(VDC_EXPAND( \
            params,  \
            VDC_ANY_PARAM, \
            VDC_TOKEN((,)comma , VDC_PLAIN) \
        ))
    BOOST_TEST(IN ==
         "{ ((,)comma) } { ((,)comma) } "
         "{ ((,)comma) ((,)comma) } { ((,)comma) ((,)comma) } "
         "{ ((,)comma) ((,)comma) } { ((,)comma) ((,)comma) }");
    #undef IN
    #undef COMMA_HANDLER
};

/*5*/
BOOST_AUTO_TEST_CASE(q_comma_fn_test,
    * boost::unit_test::description(
    "'expr' contains VDC_TOKEN_FN, which expands to quoted comma"))
{
    #define IN PARAM_LIST(COMMA_HANDLER)
    #define COMMA_HANDLER(r, params) \
        PACK(VDC_EXPAND( \
            params,  \
            VDC_ANY_PARAM, \
            VDC_TOKEN_FN(Q_COMMA, VDC_ABS_POS) \
        ))
    #define Q_COMMA(n) (,)
    BOOST_TEST(IN ==
         "{ ((,)) } { ((,)) } "
         "{ ((,)) ((,)) } { ((,)) ((,)) } { ((,)) ((,)) } { ((,)) ((,)) }");
    #undef IN
    #undef COMMA_HANDLER
    #undef Q_COMMA
};

// Macro fixture cleanup
#undef EXPR
#undef PRINT_T
#undef PRINT_F
#undef PRINT_A
#undef PARAM_LIST

BOOST_AUTO_TEST_SUITE_END() /* VDC_EXPAND */


/* VDC_EXPAND_R */
BOOST_TEST_DECORATOR(
    * boost::unit_test::description(
    "testing VDC_EXPAND_R(r, params, targets, expr) "
    "from <variadic/expand.hpp>")
    * boost::unit_test::depends_on("size_of")
    * boost::unit_test::depends_on("param_list")
    * boost::unit_test::depends_on("expand"))
BOOST_AUTO_TEST_SUITE(expand_r)

// Suite fixture setup

/*1*/
#define EXPR(pos) \
    VDC_TOKEN_FN_IF(TYPE_PARAM, PRINT_T, pos) \
    VDC_TOKEN_FN_IF(TEMPLATE_PARAM, PRINT_F, pos)
/*2*/
#define PRINT_T(n) BOOST_PP_CAT(T, n) //type param
#define PRINT_F(n) BOOST_PP_CAT(F, n) //template param
/*3*/
#define HANDLER(r, params)  \
    PACK(VDC_EXPAND_R(r, params, VDC_ANY_PARAM, EXPR(VDC_REL_POS)))

// Testing

/*1*/
BOOST_AUTO_TEST_CASE(empty_test,
    * boost::unit_test::label("empty")
    * boost::unit_test::description(
    "'param_set' in VDC_PARAM_LIST == {}"))
{
    #define IN VDC_PARAM_LIST(BOOST_PP_EMPTY(), 2, HANDLER)
    BOOST_TEST(STR(IN) == "");
    #undef IN
};


/*2*/
BOOST_AUTO_TEST_CASE(general_test,
    * boost::unit_test::description(
    "'params' == param_list of arity 2"))
{
    #define IN VDC_PARAM_LIST(PARAM_SET, 2, HANDLER)
    BOOST_TEST(STR(IN) ==
        "{ (T0) } { (F0) } "
        "{ (T0) (T1) } { (T0) (F0) } { (F0) (T0) } { (F0) (F1) }");
    #undef IN
};

// Suite fixture cleanup
#undef EXPR
#undef PRINT_T
#undef PRINT_F
#undef HANDLER

BOOST_AUTO_TEST_SUITE_END() /* VDC_EXPAND_R */

// Global fixture cleanup
#undef STR
#undef PACK
#undef PACK_OP
#undef TYPE_PARAM
#undef TEMPLATE_PARAM
#undef PARAM_SET
