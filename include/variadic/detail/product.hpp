// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_PRODUCT_HPP
#define VARIADIC_DETAIL_PRODUCT_HPP

/* variadic */
#include <variadic/detail/state.hpp>//getters

/* boost */
#include <boost/preprocessor/seq/for_each_product.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>

#define VDC_DETAIL_CARTESIAN_PRODUCT_R(r, state) \
    BOOST_PP_SEQ_FOR_EACH_PRODUCT_R( \
        r, \
        BOOST_PP_IF( \
            VDC_DETAIL_IS_INIT(state), \
            VDC_DETAIL_INIT_OP, \
            VDC_DETAIL_REPEAT_OP \
        ), \
        (VDC_DETAIL_GET_PRODUCT(state))(VDC_DETAIL_GET_INIT_SET(state)) \
    )

#define VDC_DETAIL_INIT_OP(unspec, product)   (product)
#define VDC_DETAIL_REPEAT_OP(unspec, product) (BOOST_PP_REMOVE_PARENS(product))

#endif /* VARIADIC_DETAIL_PRODUCT_HPP */
