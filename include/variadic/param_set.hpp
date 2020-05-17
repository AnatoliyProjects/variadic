// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_PARAM_SET_HPP
#define VARIADIC_PARAM_SET_HPP

/* boost */
#include <boost/preprocessor/config/limits.hpp> //BOOST_PP_LIMIT_MAG
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#define VDC_ANY_PARAM  BOOST_PP_DEC(BOOST_PP_LIMIT_MAG)

#define VDC_MAKE_PARAM_SET(params...) BOOST_PP_VARIADIC_TO_SEQ(params)

// implementation details

#define VDC_DETAIL_MOCK_PARAM BOOST_PP_LIMIT_MAG

#endif /* VARIADIC_PARAM_SET_HPP */
