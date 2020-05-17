// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


#ifndef VARIADIC_EXPAND_HPP
#define VARIADIC_EXPAND_HPP

/* variadic */
#include <variadic/token.hpp>
#include <variadic/detail/list.hpp> //VDC_DETAIL_1...
#include <variadic/detail/for_each_i.hpp>
#include <variadic/detail/relative_pos.hpp>
#include <variadic/detail/match.hpp>
#include <variadic/detail/add_parens.hpp>
#include <variadic/detail/numerate.hpp>

/* boost */
#include <boost/preprocessor/seq/filter.hpp>

#define VDC_EXPAND(params, targets, expr) \
    VDC_DETAIL_FOR_EACH_I( \
        VDC_DETAIL_ADD_PARENS_COND(targets), \
        VDC_DETAIL_EXPR_OP, \
        VDC_DETAIL_PACK_EXPAND_ARGS( \
            params, \
            VDC_DETAIL_ADD_PARENS_COND(targets), \
            expr \
        ), \
        params\
    )

#define VDC_EXPAND_R(r, params, targets, expr) \
    VDC_DETAIL_FOR_EACH_I_R( \
        r, \
        VDC_DETAIL_ADD_PARENS_COND(targets), \
        VDC_DETAIL_EXPR_OP, \
        VDC_DETAIL_PACK_EXPAND_ARGS( \
            params, \
            VDC_DETAIL_ADD_PARENS_COND(targets), \
            expr \
        ), \
        params \
    )

// implementation details
#define VDC_DETAIL_PACK_EXPAND_ARGS(params, target, expr) (params)(target)(expr)
#define VDC_DETAIL_GET_PARAMS(pack) VDC_DETAIL_1(pack)
#define VDC_DETAIL_GET_EXPR(pack)   VDC_DETAIL_3(pack)

#define VDC_DETAIL_EXPR_OP(unspec, pack, abs_pos, param) \
    VDC_DETAIL_ADD_PARENS( \
        VDC_DETAIL_NUMERATE( \
            VDC_DETAIL_FILTER(VDC_DETAIL_GET_EXPR(pack), param), \
            VDC_DETAIL_PACK_POS( \
                abs_pos, \
                VDC_DETAIL_RELATIVE_POS( \
                    param, abs_pos, VDC_DETAIL_GET_PARAMS(pack)) \
            ) \
        ) \
    )

#define VDC_DETAIL_FILTER(expr, param) \
    BOOST_PP_SEQ_FILTER(VDC_DETAIL_FILTER_PRED, param, expr)
    
#define VDC_DETAIL_FILTER_PRED(unspec, param, token) \
    VDC_DETAIL_MATCH(param, VDC_DETAIL_GET_TARGETS(token))

#endif /* VARIADIC_EXPAND_HPP */
