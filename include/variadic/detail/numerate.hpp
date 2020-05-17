// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_NUMERATE_HPP
#define VARIADIC_DETAIL_NUMERATE_HPP

/* variadic */
#include <variadic/token.hpp>    //token getters

/* boost */
#include <boost/preprocessor/seq/to_list.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/cat.hpp>

// Conversion to the list is necessary to bypass the restriction on the depth
// of nesting BOOST_PP_FOR loop (especially in case where VDC_EXPAND calls
// directly from VDC_HANDLER)
#define VDC_DETAIL_NUMERATE(expr, pos) \
    BOOST_PP_LIST_FOR_EACH( \
        VDC_DETAIL_NUMERATOR, \
        pos, \
        BOOST_PP_SEQ_TO_LIST(expr) \
    )

#define VDC_DETAIL_NUMERATOR(unspec, pos, token) \
    BOOST_PP_IF( \
        VDC_DETAIL_GET_FLAG(token), \
        VDC_DETAIL_TOKEN_NUMERATOR(token, pos), \
        VDC_DETAIL_TOKEN_FN_NUMERATOR(token, pos) \
    )

#define VDC_DETAIL_PAIR(a, b) a, b

#define VDC_DETAIL_TOKEN_NUMERATOR(token, pos) \
    BOOST_PP_IF( \
        VDC_DETAIL_IS_NUMBERED(token), \
        BOOST_PP_CAT( \
            VDC_DETAIL_GET_STR(token), \
            VDC_DETAIL_GET_POS(token, pos) \
        ), \
        VDC_DETAIL_GET_STR(token) \
    )

#define VDC_DETAIL_TOKEN_FN_NUMERATOR(token, pos) \
    BOOST_PP_IF( \
        VDC_DETAIL_IS_NUMBERED(token), \
        VDC_DETAIL_GET_FN(token)( VDC_DETAIL_GET_POS(token, pos) ), \
        VDC_DETAIL_GET_FN(token)() \
    )

#endif /* VARIADIC_DETAIL_NUMERATE_HPP */
