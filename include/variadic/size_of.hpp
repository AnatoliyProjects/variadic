// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_SIZE_OF_HPP
#define VARIADIC_SIZE_OF_HPP

/* variadic */
#include <variadic/detail/select.hpp>
#include <variadic/detail/add_parens.hpp>
#include <variadic/detail/not_empty.hpp>

/* boost */
#include <boost/preprocessor/seq/size.hpp>

#define VDC_SIZE_OF(targets, params) \
    BOOST_PP_SEQ_SIZE( \
        VDC_DETAIL_SELECT( \
            VDC_DETAIL_ADD_PARENS_COND(targets), \
            VDC_DETAIL_NOT_EMPTY(params)  \
        ) \
    )

#endif /* VARIADIC_SIZE_OF_HPP */
