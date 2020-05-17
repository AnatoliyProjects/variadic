// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_STATE_HPP
#define VARIADIC_DETAIL_STATE_HPP

/* variadic */
#include <variadic/detail/list.hpp> //VDC_DETAIL_1...

/* boost */
#include <boost/preprocessor/comparison/equal.hpp>

#define VDC_DETAIL_PACK_STATE(init_set, product, arity, max_arity, handler) \
    (init_set)(product)(arity)(max_arity)(handler)

#define VDC_DETAIL_GET_INIT_SET(state)   VDC_DETAIL_1(state)
#define VDC_DETAIL_GET_PRODUCT(state)    VDC_DETAIL_2(state)
#define VDC_DETAIL_GET_CURR_ARITY(state) VDC_DETAIL_3(state)
#define VDC_DETAIL_GET_MAX_ARITY(state)  VDC_DETAIL_4(state)
#define VDC_DETAIL_GET_HANDLER(state)    VDC_DETAIL_5(state)

#define VDC_DETAIL_IS_INIT(state) \
    BOOST_PP_EQUAL(VDC_DETAIL_GET_CURR_ARITY(state), 1)

#endif /* VARIADIC_DETAIL_STATE_HPP */
