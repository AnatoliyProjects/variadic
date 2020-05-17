// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_NOT_EMPTY_HPP
#define VARIADIC_DETAIL_NOT_EMPTY_HPP

/* variadic */
#include <variadic/param_set.hpp>

/* boost */
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/punctuation/is_begin_parens.hpp>

#define VDC_DETAIL_NOT_EMPTY(arg) \
    BOOST_PP_IF( \
        BOOST_PP_IS_BEGIN_PARENS(arg), \
        arg, \
        VDC_MAKE_PARAM_SET(VDC_DETAIL_MOCK_PARAM) \
    )

#endif /* VARIADIC_DETAIL_NOT_EMPTY_HPP */
