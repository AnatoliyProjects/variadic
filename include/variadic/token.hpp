// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_TOKEN_HPP
#define VARIADIC_TOKEN_HPP

/* variadic */
#include <variadic/param_set.hpp>   //VDC_ANY_PARAM
#include <variadic/detail/list.hpp> //VDC_DETAIL_1...
#include <variadic/detail/add_parens.hpp>

/* boost */
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/control/if.hpp>

// position flags
#define VDC_PLAIN   0
#define VDC_ABS_POS 1
#define VDC_REL_POS 2

// tokens
#define VDC_TOKEN(str, pos)   \
    ((VDC_DETAIL_TOKEN_FLAG)(str)(pos)(VDC_ANY_PARAM))
#define VDC_TOKEN_FN(fn, pos) \
    ((VDC_DETAIL_TOKEN_FN_FLAG)(fn)(pos)(VDC_ANY_PARAM))
#define VDC_TOKEN_IF(targets, str, pos)   \
    ((VDC_DETAIL_TOKEN_FLAG)(str)(pos)(targets))
#define VDC_TOKEN_FN_IF(targets, fn, pos) \
((VDC_DETAIL_TOKEN_FN_FLAG)(fn)(pos)(targets))

// implementaion details
#define VDC_DETAIL_TOKEN_FN_FLAG 0
#define VDC_DETAIL_TOKEN_FLAG    1

#define VDC_DETAIL_GET_FLAG(token)    VDC_DETAIL_1(token)
#define VDC_DETAIL_GET_STR(token)     VDC_DETAIL_2(token)
#define VDC_DETAIL_GET_FN(token)      VDC_DETAIL_2(token)
#define VDC_DETAIL_IS_NUMBERED(token) VDC_DETAIL_3(token)
#define VDC_DETAIL_GET_TARGETS(token) \
    VDC_DETAIL_ADD_PARENS_COND(VDC_DETAIL_4(token))

#define VDC_DETAIL_HAS_ABS_POS(token) \
    BOOST_PP_EQUAL(VDC_DETAIL_3(token), VDC_ABS_POS)
#define VDC_DETAIL_HAS_REL_POS(token) \
    BOOST_PP_EQUAL(VDC_DETAIL_3(token), VDC_REL_POS)

#define VDC_DETAIL_PACK_POS(abs, rel) VDC_DETAIL_MAKE_PAIR(abs, rel)
#define VDC_DETAIL_GET_ABS(pos) VDC_DETAIL_1(pos)
#define VDC_DETAIL_GET_REL(pos) VDC_DETAIL_2(pos)

#define VDC_DETAIL_GET_POS(token, pos) \
    BOOST_PP_IF( \
        VDC_DETAIL_HAS_ABS_POS(token), \
        VDC_DETAIL_GET_ABS(pos),  \
        VDC_DETAIL_GET_REL(pos) \
    ) \

#endif /* VARIADIC_TOKEN_HPP */
