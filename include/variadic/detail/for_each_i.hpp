// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_FOR_EACH_I_HPP
#define VARIADIC_DETAIL_FOR_EACH_I_HPP

/* variadic */
#include <variadic/detail/select.hpp>
#include <variadic/detail/enum.hpp>

/* boost */
#include <boost/preprocessor/seq/for_each_i.hpp>

#define VDC_DETAIL_FOR_EACH_I(targets, op, data, params) \
    VDC_DETAIL_ENUM_IF_NOT_EMPTY( \
        BOOST_PP_SEQ_FOR_EACH_I( \
            op, \
            data, \
            VDC_DETAIL_SELECT(targets, params) \
        ) \
    )

#define VDC_DETAIL_FOR_EACH_I_R(r, targets, op, data, params) \
    VDC_DETAIL_ENUM_IF_NOT_EMPTY( \
        BOOST_PP_SEQ_FOR_EACH_I_R( \
            r, \
            op, \
            data, \
            VDC_DETAIL_SELECT(targets, params) \
        ) \
    )

#endif /* VARIADIC_DETAIL_FOR_EACH_I_HPP */
