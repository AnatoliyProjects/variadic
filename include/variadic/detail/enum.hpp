// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_ENUM_HPP
#define VARIADIC_DETAIL_ENUM_HPP

/* boost */
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>
#include <boost/preprocessor/control/if.hpp>

#define VDC_DETAIL_ENUM_IF_NOT_EMPTY(params) \
    BOOST_PP_REMOVE_PARENS( \
        BOOST_PP_IF( \
            VDC_DETAIL_IS_NOT_EMPTY(params), \
            (BOOST_PP_SEQ_ENUM(params)), \
            () \
        ) \
    )

#define VDC_DETAIL_IS_NOT_EMPTY(params) \
    BOOST_PP_SEQ_SIZE(params)

#endif /* VARIADIC_DETAIL_ENUM_HPP */
