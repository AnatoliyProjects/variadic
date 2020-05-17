// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_LIST_HPP
#define VARIADIC_DETAIL_LIST_HPP

/* boost */
#include <boost/preprocessor/seq/elem.hpp>

#define VDC_DETAIL_MAKE_PAIR(first, second) (first)(second)

#define VDC_DETAIL_1(seq) BOOST_PP_SEQ_ELEM(0, seq)
#define VDC_DETAIL_2(seq) BOOST_PP_SEQ_ELEM(1, seq)
#define VDC_DETAIL_3(seq) BOOST_PP_SEQ_ELEM(2, seq)
#define VDC_DETAIL_4(seq) BOOST_PP_SEQ_ELEM(3, seq)
#define VDC_DETAIL_5(seq) BOOST_PP_SEQ_ELEM(4, seq)
#define VDC_DETAIL_6(seq) BOOST_PP_SEQ_ELEM(5, seq)
#define VDC_DETAIL_7(seq) BOOST_PP_SEQ_ELEM(6, seq)
#define VDC_DETAIL_8(seq) BOOST_PP_SEQ_ELEM(7, seq)
#define VDC_DETAIL_9(seq) BOOST_PP_SEQ_ELEM(8, seq)

#endif /* VARIADIC_DETAIL_LIST_HPP */
