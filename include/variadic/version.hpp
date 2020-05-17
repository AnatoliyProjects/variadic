// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_VERSION_HPP
#define VARIADIC_VERSION_HPP

/* boost */
#include <boost/preprocessor/stringize.hpp>

#define VDC_MAJOR 1
#define VDC_MINOR 0
#define VDC_PATCH 0

//VDC_MAJOR * 10000 + VDC_MINOR * 100 + VDC_PATCH
#define VDC_VERSION 10000

#define VDC_LIB_VERSION \
    BOOST_PP_STRINGIZE(VDC_MAJOR) "." \
    BOOST_PP_STRINGIZE(VDC_MINOR) "." \
    BOOST_PP_STRINGIZE(VDC_PATCH)

#endif /* VARIADIC_VERSION_HPP */
