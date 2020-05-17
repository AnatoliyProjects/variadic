// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


// Metafunction invoker.

// Supports invocation of arbitrary metafuction with a heterogeneous
// argument list (mixing of bool, unsigned and types).

/* variadic */
#include <variadic.hpp>

/* std */
#include <type_traits>

#define BOOL     0 //non-type param (bool B)
#define UNSIGNED 1 //non-type param (unsigned U)
#define TYPE     2 //type param (typename T)

// 'param_set' argument for VDC_PARAM_LIST, see below
#define PARAM_SET \
    VDC_MAKE_PARAM_SET(BOOL, UNSIGNED, TYPE) // ->{BOOL, UNSIGNED, TYPE}

// 'max_arity' argument for VDC_PARAM_LIST, see below
#define MAX_ARITY 3

// 'handler' argument for VDC_PARAM_LIST, see below
#define HANDLER(r, params) \
    template< \
        template< UNNAMED_PARAMS(r, params) > class MFn, \
        NAMED_PARAMS(r, params) \
    > \
    auto invoke() -> MFn< ARGS(r, params) >;

// Expands to {bool, unsigned, typename} permutations
#define UNNAMED_PARAMS(r, params) \
    VDC_EXPAND_R( \
        r, \
        params, \
        VDC_ANY_PARAM, \
        VDC_TOKEN_IF(BOOL, bool, VDC_PLAIN) \
        VDC_TOKEN_IF(UNSIGNED, unsigned, VDC_PLAIN) \
        VDC_TOKEN_IF(TYPE, typename, VDC_PLAIN)      \
    )

// Expands to {bool B, unsigned U, typename T} permutations
#define NAMED_PARAMS(r, params) \
    VDC_EXPAND_R( \
        r, \
        params, \
        VDC_ANY_PARAM, \
        VDC_TOKEN_IF(BOOL, bool B, VDC_REL_POS) \
        VDC_TOKEN_IF(UNSIGNED, unsigned U, VDC_REL_POS) \
        VDC_TOKEN_IF(TYPE, typename T, VDC_REL_POS)      \
    )

// Expands to {B, U, T} permutations
#define ARGS(r, params) \
    VDC_EXPAND_R( \
        r, \
        params, \
        VDC_ANY_PARAM, \
        VDC_TOKEN_IF(BOOL, B, VDC_REL_POS)    \
        VDC_TOKEN_IF(UNSIGNED, U, VDC_REL_POS)  \
        VDC_TOKEN_IF(TYPE, T, VDC_REL_POS) \
    )

// Expands to 'invoke' overloads.
VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER)

int main() {
    //std::conditional (heterogeneous param list: bool and types)
    static_assert(
        std::is_same<
            typename
                decltype(
                    invoke< std::conditional, true, std::true_type, void >()
                )
            ::type,
            std::true_type
        >::value,
        "types are not same"
    );
    
    //std::extent (heterogeneous param list: type and unsigned)
    static_assert(
        decltype( invoke< std::extent, int[3][4], 0 >() )::value == 3,
        "wrong number of elements along the 1st dimension of the array"
    );
};
