// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_CORE_HPP
#define VARIADIC_CORE_HPP

/* variadic */
#include <variadic/detail/product.hpp> //VDC_DETAIL_CARTESIAN_PRODUCT
#include <variadic/detail/state.hpp>   //packer and getters

/* boost */
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/comparison/less_equal.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>

#define VDC_PARAM_LIST(param_set, max_arity, handler) \
    BOOST_PP_FOR( \
        VDC_DETAIL_PACK_STATE(param_set, param_set, 1, max_arity, handler), \
        VDC_DETAIL_PARAM_LIST_PRED, \
        VDC_DETAIL_PARAM_LIST_OP, \
        VDC_DETAIL_PARAM_LIST_MACRO \
    )

#define VDC_EMPTY(handler) \
    handler(0, VDC_MAKE_PARAM_SET(VDC_DETAIL_MOCK_PARAM)) \

// implementaion details
#define VDC_DETAIL_PARAM_LIST_PRED(r, state) \
    BOOST_PP_LESS_EQUAL( \
        VDC_DETAIL_GET_CURR_ARITY(state), \
        VDC_DETAIL_GET_MAX_ARITY(state) \
    )

#define VDC_DETAIL_PARAM_LIST_OP(r, state) \
    VDC_DETAIL_PACK_STATE( \
        VDC_DETAIL_GET_INIT_SET(state), \
        VDC_DETAIL_CARTESIAN_PRODUCT_R(r, state), \
        BOOST_PP_INC(VDC_DETAIL_GET_CURR_ARITY(state)), \
        VDC_DETAIL_GET_MAX_ARITY(state), \
        VDC_DETAIL_GET_HANDLER(state) \
    )

#define VDC_DETAIL_PARAM_LIST_MACRO(r, state) \
    BOOST_PP_SEQ_FOR_EACH_R( \
        r, \
        BOOST_PP_IF( \
            VDC_DETAIL_IS_INIT(state), \
            VDC_DETAIL_INIT_HANDLER, \
            VDC_DETAIL_HANDLER \
        ), \
        state, \
        VDC_DETAIL_GET_PRODUCT(state) \
    )

#define VDC_DETAIL_HANDLER(r, state, param_list) \
    VDC_DETAIL_GET_HANDLER(state)(r, param_list)

#define VDC_DETAIL_INIT_HANDLER(r, state, param_list) \
    VDC_DETAIL_HANDLER(r, state, (param_list))

#endif /* VARIADIC_CORE_HPP */
