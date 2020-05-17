// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_DETAIL_RELATIVE_POS_HPP
#define VARIADIC_DETAIL_RELATIVE_POS_HPP

/* variadic */
#include <variadic/detail/select.hpp>
#include <variadic/detail/add_parens.hpp>

/* boost */
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/first_n.hpp>

#define VDC_DETAIL_RELATIVE_POS(target, abs_pos, params) \
    BOOST_PP_DEC( \
        BOOST_PP_SEQ_SIZE( \
            VDC_DETAIL_SELECT( \
                VDC_DETAIL_ADD_PARENS_COND(target), \
                BOOST_PP_SEQ_FIRST_N(BOOST_PP_INC(abs_pos), params) \
            ) \
        ) \
    )

#endif /* VARIADIC_DETAIL_RELATIVE_POS_HPP */
