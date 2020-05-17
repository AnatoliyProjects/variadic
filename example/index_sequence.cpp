// Copyright 2020 Anatoliy Petrov
// (apetrov.projects@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt


// Adapter for std::index_sequence.

// Supports instantiation of std::index_sequence with
// a heterogeneous argument list (mixing of std::size_t non-types
// and std::integral_constant types)

/* variadic */
#include <variadic.hpp>

/* std */
#include <type_traits>
#include <utility>

/* 'param_set' argument for VDC_PARAM_LIST */
#define SIZE_T 0 //non-type param (std::size_t V)
#define TYPE   1 //type param (typename T)
#define PARAM_SET VDC_MAKE_PARAM_SET(SIZE_T, TYPE) // -> {SIZE_T, TYPE}

/* 'max_arity' argument for VDC_PARAM_LIST */
#define MAX_ARITY 4

/* 'handler' argument for VDC_PARAM_LIST */
#define HANDLER(r, params) \
    template< IN(params) > \
    auto as_idx() -> std::index_sequence< OUT(params) >;

// Expands to {std::size_t V, typename T} permutations
#define IN(params) \
    VDC_EXPAND( \
        params, \
        VDC_ANY_PARAM, \
        VDC_TOKEN_IF(SIZE_T, std::size_t V, VDC_REL_POS) \
        VDC_TOKEN_IF(TYPE, typename T, VDC_REL_POS)      \
    )

// Expands to {V, T::value} permutations
#define OUT(params) \
    VDC_EXPAND( \
        params, \
        VDC_ANY_PARAM, \
        VDC_TOKEN_IF(SIZE_T, V, VDC_REL_POS) \
        VDC_TOKEN_FN_IF(TYPE, FN, VDC_REL_POS)      \
    )
#define FN(n) VDC_CAT(T, n) ::value

// Overloaded function for empty template parameter list.
// This code can be placed in the HANDLER macro in a case of use VDC_EMPTY.
template <typename...Ts>
auto as_idx() -> std::index_sequence<> {
    static_assert(sizeof...(Ts) == 0, "undefined");
};

// Expands to 'as_idx' overloads
VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER)

int main() {
    //empty
    static_assert(
        std::is_same<std::index_sequence<>, decltype(as_idx<>())>::value == true,
        "types are not same");
    
    //heterogeneous args in 'as_idx'
    static_assert(
        std::is_same<
            std::index_sequence<0, 1>,
            decltype( as_idx< std::integral_constant<std::size_t, 0>, 1 >() )
        >::value == true,
        "types are not same"
    );
    static_assert(
        std::is_same<
            std::index_sequence<0, 1, 2, 3>,
            decltype( as_idx< std::integral_constant<std::size_t, 0>, 1,
            std::integral_constant<std::size_t, 2>, 3 >() )
        >::value == true,
        "types are not same"
    );
};
