// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_SELECT_HPP
#define VARIADIC_DETAIL_SELECT_HPP

/* variadic */
#include <variadic/detail/match.hpp>

/* boost */
#include <boost/preprocessor/seq/filter.hpp>

#define VDC_DETAIL_SELECT(targets, params) \
    BOOST_PP_SEQ_FILTER( \
        VDC_DETAIL_MATCH_OP, \
        targets, \
        params \
    ) \

#define VDC_DETAIL_MATCH_OP(unspec, targets, param) \
    VDC_DETAIL_MATCH(param, targets)

#endif /* VARIADIC_DETAIL_SELECT_HPP */
