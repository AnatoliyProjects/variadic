// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_COMMA_IF
#define VARIADIC_COMMA_IF

/* variadic */
#include <variadic/size_of.hpp>

/* boost */
#include <boost/preprocessor/punctuation/comma_if.hpp>

#define VDC_COMMA_IF(targets, params) \
    BOOST_PP_COMMA_IF(VDC_SIZE_OF(targets, params))

#endif /* VARIADIC_COMMA_IF */
