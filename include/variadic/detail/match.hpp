// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_MATCH_HPP
#define VARIADIC_DETAIL_MATCH_HPP

/* variadic */
#include <variadic/param_set.hpp> //VDC_(MOCK|ANY)_PARAM
#include <variadic/detail/select.hpp>

/* boost */
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>
#include <boost/preprocessor/list/size.hpp>
#include <boost/preprocessor/list/filter.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/comparison/greater.hpp>

#define VDC_DETAIL_MATCH(param, targets) \
    BOOST_PP_IF( \
        BOOST_PP_GREATER(BOOST_PP_TUPLE_SIZE(targets), 1), \
        VDC_DETAIL_ANY_OF(targets, param), \
        VDC_DETAIL_EQUAL_TO( \
            param, \
            BOOST_PP_TUPLE_ELEM(0, targets) \
        ) \
    )

#define VDC_DETAIL_EQUAL_TO(param, target) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL(param, VDC_DETAIL_MOCK_PARAM), \
        0, \
        BOOST_PP_IF( \
            BOOST_PP_EQUAL(target, VDC_ANY_PARAM), \
            1, \
            BOOST_PP_EQUAL(param, target) \
        ) \
    )

// Conversion to the list is necessary to bypass the restriction on the depth
// of nesting BOOST_PP_FOR loop (especially in the case where VDC_EXPAND calls
// directly from VDC_HANDLER)
#define VDC_DETAIL_ANY_OF(tpl, val) \
    BOOST_PP_NOT_EQUAL( \
        BOOST_PP_LIST_SIZE( \
            BOOST_PP_LIST_FILTER( \
                VDC_DETAIL_EQUAL_TO_OP, \
                val, \
                BOOST_PP_TUPLE_TO_LIST(tpl) \
            ) \
        ), \
        0 \
    )

#define VDC_DETAIL_EQUAL_TO_OP(unspec, data, elem) \
    VDC_DETAIL_EQUAL_TO(elem, data)

#endif /* VARIADIC_DETAIL_MATCH_HPP */
