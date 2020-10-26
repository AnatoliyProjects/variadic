> Template parameter representation / parameter representation / representation
* unique integral constant defined as macro *

# Table of contents #
1. [What is Variadic?](#what-is-variadic)
    * [Description](#description)
    * [Rational](#rational)
    * [Basic example](#basic-example)
        * [Step 1. Define the set of possible template parameters](#step-1-define-the-set-of-possible-template-parameters)
        * [Step 2. Define reasonable max length of template parameter list](#step-2-define-reasonable-max-length-of-template-parameter-list)
        * [Step 3. Define macro function for code generation](#step-3-define-macro-function-for-code-generation)
        * [Step 4. Invoke `VDC_PARAM_LIST`](#step-4-invoke-vdc_param_list)
2. [Usage notes](#usage-notes)
    * [Reentrancy](#reentrancy)
    * [Token pasting](#token-pasting)
    * [Commas and parentheses in user-defined macros](#commas-and-parentheses-in-user-defined-macros)
    * [Parameter selection](#parameter-selection)
3. [Reference](#reference)
    * [<variadic/param_set.hpp>](#variadicparam_sethpp)
        * [VDC_ANY_PARAM](#vdc_any_param)
        * [VDC_MAKE_PARAM_SET](#vdc_make_param_set)
    * [<variadic/core.hpp>](#variadiccorehpp)
        * [HANDLER concept](#handler-concept)
        * [VDC_PARAM_LIST](#vdc_param_list)
        * [VDC_EMPTY](#vdc_empty)
    * [<variadic/expand.hpp>](#variadicexpandhpp)
        * [VDC_EXPAND, VDC_EXPAND_R](#vdc_expand-vdc_expand_r)
    * [<variadic/token.hpp>](#variadictokenhpp)
        * [VDC_PLAIN](#vdc_plain)
        * [VDC_ABS_POS](#vdc_abs_pos)
        * [VDC_REL_POS](#vdc_rel_pos)
        * [VDC_TOKEN](#vdc_token)
        * [VDC_TOKEN_FN](#vdc_token_fn)
        * [VDC_TOKEN_IF](#vdc_token_if)
        * [VDC_TOKEN_FN_IF](#vdc_token_fn_if)
    * [<variadic/cat.hpp>](#variadiccathpp)
        * [VDC_CAT](#vdc_cat)
    * [<variadic/size_of.hpp>](#variadicsize_ofhpp)
        * [VDC_SIZE_OF](#vdc_size_of)
    * [<variadic/comma_if.hpp>](#variadiccomma_ifhpp)
        * [VDC_COMMA_IF](#vdc_comma_if)
    * [<variadic/version.hpp>](#variadicversionhpp)
        * [VDC_MAJOR](#vdc_major)
        * [VDC_MINOR](#vdc_minor)
        * [VDC_PATCH](#vdc_patch)
        * [VDC_VERSION](#vdc_version)
        * [VDC_LIB_VERSION](#vdc_lib_version)
4. [Deployment](#deployment)
    * [Dependences](#dependences)
    * [Compilation](#compilation)
    * [Standard requirements](#standard-requirements)
    * [Compiler support](#compiler-support)
5. [License](#license)

-----------
# What is Variadic? #

## Description ##

**Variadic** is a header-only preprocessor library for C++ that provides tools for emulation of "truly" heterogeneous template parameter packs (i.e. parameter packs, which can accept zero or more type, non-type or template arguments in one argument list).

But what for?

## Rational ##

Let's imagine that we are developing a template metaprogramming library, like [boost/mp11](https://www.boost.org/doc/libs/develop/libs/mp11/doc/html/mp11.html), [boost/mpl](https://www.boost.org/doc/libs/1_63_0/libs/mpl/doc/index.html), [brigand](https://github.com/edouarda/brigand) or [metal](https://github.com/brunocodutra/metal).   

Mentioned libraries provide functionality for instantiation of arbitrary metafunction in unified manner (like `std::invoke` or `std::apply`, but at compile-time). This functionality can be implemented as follows: 

 ```cpp
#include <type_traits> // for remove_const

using namespace std;

// High-order metafunction that's doing all the work
template <
    template<typename...> class MFn,
    typename...Args>
struct invoke{
    using type = typename MFn<Args...>::type;
};
    
// Alias that simplifies call syntax
template <
    template<typename...> class MFn,
    typename...Args>
using invoke_t = typename invoke<MFn, Args...>::type;
    
// Check
static_assert(
    is_same<
        invoke_t<std::remove_const, const char>,
        char
    >::value, 
    "Types are not same");
 ```

In our example, `invoke` is a high-order metafunction that accepts `std::remove_const` trait and invokes it with given argument (`const char`). All seems good, but what about limitations of homogeneous parameter packs? Lets try to call `invoke` metafunction as follows:

```cpp
static_assert(
    invoke_t<std::extent, int [3][4], 0>::value == 3,
    "Wrong extent");
```

Now we try to compute the extent of the 1st dimension of `int [3][4]` array. But there's no way to do this with `invoke`.

Why?

First of all, let's take a look at `std::extent` declaration: 

```cpp
//from <type_traits>

template <class T, unsigned I = 0> struct extent;
```

Notice, that `std::extent` template parameter list contains not only type, but also non-type (`unsigned I`). Because of this, instantiation of `invoke` with `std::extent` leads to substitution failure for `MFn` parameter, which parameter pack (`template <typename...> class MFn`) contains only types. As a result we have compilation error: "Template template argument has different template parameters than its corresponding template template parameter".

Also we can't pass `unsigned I` (second parameter of `std::extent`) as `invoke` second argument, because `Args` parameter pack (`typename... Args`) accepts only types.

As alternative, we could customize `invoke` for wide range of possible metafunction's signatures, but such decision is error-prone and leads to code duplications.

Another alternative is to use types as the only possible metafunction arguments (see **Practical C++ Metaprogramming by Edouard Alligand and Joel Falcou**, pages 28-30 for details). But this decision is inconsistent with C++ Standard Library, where some traits and typelists accept non-types as well as types (for example, see `std::extent`, `std::aligned_storage`, `std::aligned_union`, `std::conditional`, `std::enable_if` traits from *<type_traits>* and `std::index_sequence` alias from *<utility>* ).

Newest C++17/20 features (including non-type template parameters with a placeholder type and class types in non-type template parameters from [D0732R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0732r1.pdf)) also useless for out task.

For example, non-type template parameters with a placeholder type from C++17 make it possible to use non-types instead of types in metaprogramming routine as follows:

```cpp
// Overview of non-types with a placeholder (C++17 feature)

/* std */
#include <type_traits>
#include <tuple>

// Cast arbitrary type to non-type
template<typename T>
constexpr std::add_pointer_t<T> as_pointer(){
    return std::add_pointer_t<T>(nullptr);
};

// Cast arbitrary non-type to type
template<auto V>
struct as_type {
    using _v_type = decltype(V);
    using type =
        std::conditional_t<
            //if V is nullptr...
            std::is_pointer_v<_v_type> and not V,
            //it means that V represents type
            std::remove_pointer_t<_v_type>,
            //else means that V is non-type
            std::integral_constant<_v_type, V>
        >;
};

// Alias that simplifies call syntax
template<auto V>
using as_type_t = typename as_type<V>::type;

// Pack non-types to tuple
template<auto...V>
struct pack {
    using type = std::tuple<as_type_t<V>...>;
};

// Alias that simplifies call syntax
template<auto...V>
using pack_t = typename pack<V...>::type;


//Input types
struct foo{};
struct bar{};

int main() {
    static_assert(
        std::is_same<
            pack_t<1, false, 'a', as_pointer<foo>(), as_pointer<bar>() >,
            std::tuple<
                std::integral_constant<int, 1>,
                std::integral_constant<bool, false>,
                std::integral_constant<char, 'a'>,
                foo, bar
            >
            >::value,
            "Types are not same"
        );
};
```

 But possible decision, based on this feature, is just equal to recipe below - use non-types everywhere in your metaprogramming library. This recommendation is also (and much more than previous one) inconsistent with the Standard Library.

The example above and overview of alternatives, points us to limitation of homogeneous parameter packs  - there is no way to make them work both with non-types *and* types (or templates).

Let's see how **Variadic** solves these problem.

## Basic example ##

In this example, we'll try to improve `invoke` metafunction from [Rational](#rational) section and make it work with all standard type traits from *<type_traits>*, including those, which accept non-types.

Usage of **Variadic** in such a scenario consists of the following steps:
1. define the set of possible template parameters for heterogeneous parameter pack;
2. define reasonable max length of template parameter list;
3. define macro function for code generation;
4. invoke `VDC_PARAM_LIST` macro function from *<variadic/core.hpp>*.

Let's cover this steps in depth.

### Step 1. Define the set of possible template parameters ###

Template parameter representation, which **Variadic** works with, is integer macro constant. All user-defined parameters should be packed to set with `VDC_MAKE_PARAM_SET` macro function from *<variadic/param_set.hpp>*.

Because type traits  from *<type_traits>* accept types (mainly), `bool`  (`std::conditional`, `std::enable_if`) and `unsigned/std::size_t`  (`std::extent`, `std::aligned_storage`, `std::aligned_union`), for our example we need heterogenous parameter pack, which can accepts such non-types (`bool` and `unsigned`) as well as types. For simplicity, let's clarify `std::size_t` as alias for `unsigned` (which is obviously wrong). With this assumption our parameter set may looks as follows:

```cpp
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
```

That's all, our parameter set is done.

### Step 2. Define reasonable max length of template parameter list ###

In our usage scenario (invocation of arbitrary type trait from *<type_traits>*) reasonable max length of parameter list is small - 3 template parameters will be enough for demonstration use.

```cpp
// 'max_arity' argument for VDC_PARAM_LIST, see below
#define MAX_ARITY 3
```

### Step 3. Define macro function for code generation ###

This step is the most difficult and complicated. Before cover this theme in depth, let's see how **Variadic** works.

 The main idea is simple - we can emulate heterogeneous parameter packs only with overloading function templates (class template specializations is the wrong choice because we can't specialize type or template parameter with non-type and visa versa). Therefore, we need macro function, which:
1. generates all possible permutations of arbitrary parameters and 
2. produces code of overloaded function template for every permutation.

Permutations of possible template parameters can be produced with `VDC_PARAM_LIST` macro function:

```cpp
// from <variadic/core.hpp>

// possible 'param_set' and 'max_arity' arguments see above
#define VDC_PARAM_LIST(param_set, max_arity, handler) 
```

This macro creates variable-length ( 0 <= `max_arity`) permutations of possible template parameters from `param_set`  and calls user-defined `handler` for every permutation.

For example, if we call `VDC_PARAM_LIST` with `param_set` = *{type, bool}* and `max_arity` = 2  it makes following permutations: *[type], [bool], [type, type], [type, bool], [bool, type], [bool, bool]*. Every permutation will be iteratively passed to user-defined handler (see `handler` parameter of `VDC_PARAM_LIST`), where it expands into code.

User `handler` for `VDC_PARAM_LIST`  can be declared as follows:

```cpp
#define HANDLER(r, params) ... // name is unspecified
```

and should accepts two arguments:

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| r | the next available `BOOST_PP_FOR` repetition. May uses with `VDC_EXPAND_R` to speed up performance. For details see [boost/preprocessor](https://www.boost.org/doc/libs/1_72_0/libs/preprocessor/doc/topics/reentrancy.html) documentation. | 
| params | parameter list for code generation produced by `VDC_PARAM_LIST` or `VDC_EMPTY` |

For our example, `handler` macro function may be implemented as follows: 

```cpp
// 'handler' argument for VDC_PARAM_LIST, see below
#define HANDLER(r, params) \
    template< \
        template< UNNAMED_PARAMS(r, params) > class MFn, \
        NAMED_PARAMS(r, params) \
    > \
    auto invoke() -> MFn< ARGS(r, params) >;
```

Notice that:
1. `invoke` is a function template, not a class template (rational see above);
2. `invoke` is a function declaration, not definition (for metaprogramming routine definition not needed);
3. result of arbitrary metafunction invocation placed to `invoke` return type and can be obtained with `decltype`;
4. `invoke` parameter list and return type formed with user-defined macro functions `UNNAMED_PARAMS`, `NAMED_PARAMS` and `ARGS`.

Now let's take a look at implementation of  `UNNAMED_PARAMS`:

```cpp
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
```
`VDC_EXPAND` and `VDC_EXPAND_R` macro functions from *<variadic/expand.hpp>* are the heart of the code generation process.

This macros: 
1. accept parameter list (current parameter permutation generated by `VDC_PARAM_LIST`),
2. select from this parameter list target parameters (in example above all parameters are target because of `VDC_ANY_PARAM` constant) and 
3. generate code for this target parameters by rules given in `VDC_TOKEN` or `VDC_TOKEN_IF` macros.

`VDC_TOKEN_IF`, in turn, for every parameter, specified in the first argument (`BOOL`, `UNSIGNED` or `TYPE`), past to output given string ("bool", "unsigned" or "typename") without appending to this string any number (because of `VDC_PLAIN` flag).

`NAMED_PARAMS` and `ARGS` macro functions are defined in the similar manner:

```cpp
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
```
`VDC_REL_POS` flag specifies appending to token string ("bool B", "unsigned U", "typename T" in case of `NAMED_PARAM` or "B", "U", "T" in case of `ARG`) relative position number of given parameter (`BOOL`, `UNSIGNED` or `TYPE`) in current parameter list.

For example, if we invoke this macros with parameter list [`UNSIGNED`, `UNSIGNED`] , we will get the following result:

```cpp
UNNAMED_PARAMS -> unsigned, unsigned
NAMED_PARAMS -> unsigned U0, unsigned U1
ARGS -> U0, U1
```

For [`BOOL`, `UNSIGNED`, `TYPE`] parameters we will get:

```cpp
UNNAMED_PARAMS -> bool, unsigned, typename
NAMED_PARAMS -> bool B0, unsigned U0, typename T0
ARGS -> B0, U0, T0
```

Therefore, if we invoke `HANDLER` macro function as above we will get:

```cpp
// in case of [UNSIGNED, UNSIGNED]
template<
    template< unsigned, unsigned > class MFn,
    unsigned U0, unsigned U1
>
auto invoke() -> MFn< U0, U1 >;

// in case of [BOOL, UNSIGNED, TYPE]
template<
    template< bool, unsigned, typename > class MFn,
    bool B0, unsigned U0, typename T0
>
auto invoke() -> MFn< B0, U0, T0 >;
```

(but all in one line because of horizontal repetition in `VDC_EXPAND_R`, for details see [boost/preprocessor](https://www.boost.org/doc/libs/1_72_0/libs/preprocessor/doc/index.html) documentation).

As mentioned above, `handler` macro will be iteratively invokes with all possible permutations of user-defined parameters. So after `VDC_PARAM_LIST` expansion we will get the overloading `invoke` function template, which can be instantiate with arbitrary list of types *and* non-types (i.e. with heterogeneous argument list)!

### Step 4. Invoke `VDC_PARAM_LIST` ###

Easy!

```cpp
// Expands to 'invoke' overloads. 
// PARAM_SET, MAX_ARITY and HANDLER defined above
VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER)
```

After this invocation we will get  `invoke` function template, overloaded for all possible combinations of template parameters (types and non-types). It makes it possible to instantiate  `invoke` with heterogeneous argument list.

### Using of `invoke` metafunction ###

Notice, that `invoke` is a function, not a class. So we can't use it as "ordinary" metafunction (`typename invoke< TYPE_TRAIT, ARGS >::type`). Instead of this, we should use `decltype` specifire: `typename decltype( invoke< TYPE_TRAIT, ARGS >() )::type`.

Now let's test our `invoke` metafunction with `std::extent` and `std::conditional` type traits, which parameter lists contain non-types as well as types:

```cpp
 .. implementation of 'invoke' .. 

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
```

Successful compilation means that `invoke` works as excepted. Full code of example see in *<example/invoke.cpp>*.

-----------
# Usage notes #

## Reentrancy ##

**Variadic** implemented with active use of *boost/preprocessor*. Among other things, *boost/preprocessor* provides macro functions for iteration (see `BOOST_PP_FOR`, `BOOST_PP_REPEAT`, and `BOOST_PP_WHILE`). However, *boost/preprocessor* has limited depth of reentrancy for iteration constructs. Because of this limitation and some implementation decisions in **Variadic**, there is no way to use `BOOST_PP_FOR` iteration within `handler`. For details see [boost/preprocessor](https://www.boost.org/doc/libs/1_72_0/libs/preprocessor/doc/topics/reentrancy.html) documentation.

## Token pasting ##

As mentioned above, you can specify token numeration policy with flags `VDC_PLAIN`, `VDC_ABS_POS`, `VDC_REL_POS`. If you produce output strings with user-defined macro function (see `VDC_TOKEN_FN` and `VDC_TOKEN_FN_IF`), your macro will be invokes with current parameter number (relative or absolute). But in fact, this number is a result of macro expansion, so you can't use it with ## preprocessor command. Instead of this, use `VDC_CAT` macro function from *<variadic/cat.hpp>*. 

## Commas and parentheses in user-defined macros ##

The preprocessor interprets commas as argument separators in macro invocations. So you should enclose every macro argument, which contains commas, in parentheses. 

Furthermore, `str` argument in `VDC_TOKEN` and `VDC_TOKEN_IF` cannot ends with a round bracket. For such cases you may use `VDC_TOKEN_FN` and `VDC_TOKEN_FN_IF`. 

## Parameter selection ##

You can select parameters for code generation by `targets` argument in `VDC_EXPAND`, `VDC_EXPAND_R`, `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` macro functions. But their usage scenarios are not same. 

In case of `VDC_EXPAND`, `VDC_EXPAND_R` use, current parameter, which does not match with any of `targets`, just skipped for further code generation. For example, this macro expands to empty:

```cpp
/* variadic */
#include <variadic/param_set.hpp>
#include <variadic/core.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>

#define TYPE 0 //type param (typename T)
#define PARAM_SET VDC_MAKE_PARAM_SET(TYPE) //-> {TYPE}

#define FAKE_PARAM 100

#define MAX_ARITY 2

#define HANDLER(r, params) \
    VDC_EXPAND(params, FAKE_PARAM, VDC_TOKEN(typename T, VDC_ABS_POS))

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER) 
// -> empty! 
// There is no FAKE_PARAM in 'params'!
```

But `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` not skipping parameters and return empty placeholder if neither of them matches with `targets`:

```cpp
/* variadic */
#include <variadic/param_set.hpp>
#include <variadic/core.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>

#define TYPE 0 //type param (typename T)
#define PARAM_SET VDC_MAKE_PARAM_SET(TYPE) //-> {TYPE}

#define FAKE_PARAM 100

#define MAX_ARITY 2

#define HANDLER(r, params) \
    VDC_EXPAND( \
        params, \
        VDC_ANY_PARAM, \
        VDC_TOKEN_IF(FAKE_PARAM, typename T, VDC_ABS_POS) \
    )

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER) 
// ->  , 
// There is no FAKE_PARAM in 'params', so VDC_TOKEN_IF expands to empty placeholder!
```

Thus you can't use `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` for parameter filtering. For such a case use `VDC_EXPAND`, `VDC_EXPAND_R`. Also you should provide actual token for every parameter, which haven't been skipped by `VDC_EXPAND` and `VDC_EXPAND_R`.

-----------
# Reference #

## *<variadic/param_set.hpp>* ##

### VDC_ANY_PARAM ###

```cpp
#define VDC_ANY_PARAM unspecified
```

#### Description ####

Macro constant, which suppress parameter filtering in  `VDC_EXPAND`, `VDC_EXPAND_R`, `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` (any parameter in current parameter list becomes acceptable for code generation).

#### Header ####

*<variadic/param_set.hpp>*

### VDC_MAKE_PARAM_SET ###

```cpp
#define VDC_MAKE_PARAM_SET(params...)
```

#### Description ####

Packs user-defined parameters to set of parameters for further `VDC_PARAM_LIST`  invocation.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| params | variadic list of user-defined parameters  |

#### Remarks ####

1. Parameters for `params` should be implemented as unsigned integer macro constants. 
2. Invocation of  `VDC_MAKE_PARAM_SET` with empty `params`  is an error.

#### Header ####

*<variadic/param_set.hpp>*

#### Sample code ####

```cpp
#include <variadic/param_set.hpp>

#define TYPE 0 // type parameter (typename T)
#define BOOL 1 // non-type parameter (bool B)

#define PARAM_SET VDC_MAKE_PARAM_SET(TYPE, BOOL) // ->{TYPE, BOOL}
```

## *<variadic/core.hpp>* ##

### HANDLER concept ###

```cpp
#define HANDLER(r, params)
```

#### Description ####

User-defined callback for code generation. Invokes by `VDC_PARAM_LIST` or `VDC_EMPTY` with current parameter list.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| r | the next available `BOOST_PP_FOR` repetition. May uses with `VDC_EXPAND_R` to speed up performance. for details see [boost/preprocessor](https://www.boost.org/doc/libs/1_72_0/libs/preprocessor/doc/topics/reentrancy.html) documentation. | 
| params | parameter list for code generation produced by `VDC_PARAM_LIST` or `VDC_EMPTY` |

#### Remarks ####

For code generation in `HANDLER`, use  `VDC_EXPAND` or `VDC_EXPAND_R` macro functions.

#### Header ####

*user-defined*

#### Sample code ####

see below

### VDC_PARAM_LIST ###

```cpp
#define VDC_PARAM_LIST(param_set, max_arity, handler)
```

#### Description ####

Generates variable-length ( 0 <= `max_arity`) permutations of possible parameters from `param_set`  and calls user-defined `handler` for every permutation.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| param_set | set of user-defined possible parameters produced by `VDC_MAKE_PARAM_SET`|
| max_arity | max length of template parameter list  |
| handler | user-defined handler macro function  |

#### Remarks ####

See `VDC_EMPTY` for empty parameter list emulation.

#### Header ####
<variadic/core.hpp>

#### Sample code ####

```cpp
#include <variadic/param_set.hpp>
#include <variadic/core.hpp>

#define F 0
#define T 1

#define BOOL_SET VDC_MAKE_PARAM_SET(F, T) // -> {F, T}
#define MAX_ARITY 2
#define HANDLER(r, params) [params] // just print params

// expands to: [(0)] [(1)] [(0) (0)] [(0) (1)] [(1) (0)] [(1) (1)]
VDC_PARAM_LIST(BOOL_SET, MAX_ARITY, HANDLER) 
```

### VDC_EMPTY ###

```cpp
#define VDC_EMPTY(handler) 
```

#### Description ####

Invokes user-defined `handler` with emulation of empty parameter list. 

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| handler | user-defined handler macro function  |

#### Remarks ####

Usable if `handler` contains `VDC_EXPAND` or `VDC_EXPAND_R` macro functions, which cannot be invoked with "truly" empty parameter list.

#### Header ####

*<variadic/core.hpp>*

#### Sample code ####

```cpp
#include <variadic/core.hpp>
#include <variadic/expand.hpp>
#include <variadic/token.hpp>

// typelist declaration
template<typename...> struct typelist;

// specialization for empty
#define HANDLER(r, params) \
    template<> \
    struct typelist< VDC_EXPAND( \
        params, VDC_ANY_PARAM, VDC_TOKEN(T, VDC_ABS_POS))>; 
    
VDC_EMPTY(HANDLER) // -> template<> struct typelist< >;
```
## *<variadic/expand.hpp>* ##

### VDC_EXPAND, VDC_EXPAND_R ###

```cpp
#define VDC_EXPAND(params, targets, expr)
#define VDC_EXPAND_R(r, params, targets, expr)
```

#### Description ####

1. accept current parameter list generated by `VDC_PARAM_LIST` or `VDC_EMPTY` (see `params`),
2. select parameters which match with `targets` and 
3. producing code for this target parameters by rules given in `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` macros (see `expr`).

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
|r| the next available `BOOST_PP_FOR` repetition. May uses with `VDC_EXPAND_R` to speed up performance. For details see [boost/preprocessor](https://www.boost.org/doc/libs/1_72_0/libs/preprocessor/doc/topics/reentrancy.html) documentation.|
| params | parameter list for code generation produced by `VDC_PARAM_LIST` or `VDC_EMPTY` |
| targets | user-defined parameter as unary target, `VDC_ANY_PARAM` constant for any, or tuple for multiple targets (for example, `(TYPE, BOOL)` for user-defined `TYPE` and `BOOL` parameters) |
| expr | list of tokens (`VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN` or `VDC_TOKEN_FN_IF`) without any separator between them |

#### Remarks ####

See [Reentrancy](#reentrancy) section in case of using *boost/preprocessor* for code generation.

Also see [Parameter selection](#parameter-selection) section.

#### Header ####

*<variadic/expand.hpp>*

#### Sample code ####

See examples for `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF`.

## *<variadic/token.hpp>* ##

### VDC_PLAIN ###

```cpp
#define VDC_PLAIN unspecified
```

#### Description ####

Specifies producing of unnumbered (plain) parameters by `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` macro functions.

#### Header ####

*<variadic/token.hpp>*

### VDC_ABS_POS ###

```cpp
#define VDC_ABS_POS unspecified
```

#### Description #### 

Specifies producing of numbered parameters by `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` macro functions. As number uses ***absolute*** position of current parameter in parameter list. For example, for parameter list [TYPE, TYPE, UNSIGNED, BOOL] and strings {"T" for TYPE, "U" for UNSIGNED, "B" for BOOL} will be produced: T0, T1, U2, B3.

#### Header #### 

*<variadic/token.hpp>*

### VDC_REL_POS ###

```cpp
#define VDC_REL_POS unspecified
```

#### Description ####

Specifies producing of numbered parameters by `VDC_TOKEN`, `VDC_TOKEN_IF`, `VDC_TOKEN_FN`, `VDC_TOKEN_FN_IF` macro functions. As number uses ***relative*** position of current parameter in parameter list. For example, for parameter list [TYPE, TYPE, UNSIGNED, BOOL] and strings {"T" for TYPE, "U" for UNSIGNED, "B" for BOOL} will be produced: T0, T1, U0, B0.

#### Header ####

*<variadic/token.hpp>*

### VDC_TOKEN ###

```cpp
#define VDC_TOKEN(str, pos)
```

#### Description ####

Expands to `str` argument for every parameter in current parameter list.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| str | user-defined string to pasting  |
| pos | constant which specify numeration policy (see `VDC_PLAIN`, `VDC_ABS_POS`, `VDC_REL_POS` for details) |

#### Remarks ####

In case of using commas or parentheses see [Commas and parentheses in user-defined macros](#commas-and-parentheses-in-user-defined-macros) section.

Also see [Parameter selection](#parameter-selection) section.

#### Header ####

*<variadic/token.hpp>*

#### Sample code ####

```cpp
#include <variadic/param_set.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>
#include <variadic/core.hpp>

#define TYPE 0 //type param

#define PARAM_SET \
    VDC_MAKE_PARAM_SET(TYPE) // ->{TYPE}

#define MAX_ARITY 3

// declaration
template<typename...>
struct foo;

// partial specializations
#define HANDLER(r, params) \
    template< \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN(typename T, VDC_ABS_POS))> \
    struct foo< \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN(T, VDC_ABS_POS)) >;

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER) 
// ->
// template< typename T0 > struct foo< T0 >; 
// template< typename T0, typename T1 > struct foo< T0, T1 >; 
// template< 
//    typename T0, typename T1, typename T2 > struct foo< T0, T1, T2 >;
```

### VDC_TOKEN_FN ###

```cpp
#define VDC_TOKEN_FN(fn, pos) 
```

#### Description ####

Invokes user-defined `fn` with current parameter position (or with empty if  `VDC_PLAIN` flag used) and pasting it expansion to output.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| fn | user-defined unary function which accepts token position (parameter for number requires even if `VDC_PLAIN` flag used)  |
| pos | constant which specify numeration policy (see `VDC_PLAIN`, `VDC_ABS_POS`, `VDC_REL_POS` for details) |

#### Remarks ####

In case of using commas or parentheses within `fn` see [Commas and parentheses in user-defined macros](#commas-and-parentheses-in-user-defined-macros) section.

Also see [Token pasting](#token-pasting) and [Parameter selection](#parameter-selection) sections.

#### Header ####

*<variadic/token.hpp>*

#### Sample code ####

```cpp
#include <variadic/param_set.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>
#include <variadic/core.hpp>
#include <variadic/cat.hpp>

#define TYPE 0 //type param

#define PARAM_SET \
    VDC_MAKE_PARAM_SET(TYPE) // ->{TYPE}

#define MAX_ARITY 3

// foo declaration
template<typename...>
struct foo;

// foo partial specializations
#define HANDLER(r, params) \
    template< \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN_FN(PARAM, VDC_ABS_POS))> \
    struct foo< \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN_FN(ARG, VDC_ABS_POS)) >;

#define PARAM(n) typename VDC_CAT(T, n)
#define ARG(n) VDC_CAT(T, n)

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER)
// ->
// template< typename T0 > struct foo< T0 >;
// template< typename T0, typename T1 > struct foo< T0, T1 >;
// template<
//    typename T0, typename T1, typename T2 > struct foo< T0, T1, T2 >;
```

### VDC_TOKEN_IF ###

```cpp
#define VDC_TOKEN_IF(targets, str, pos)  
```

#### Description ####

Expands to `str` argument for every parameter which matches with `targets` .

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| targets | user-defined parameter as unary target, `VDC_ANY_PARAM` flag for any, or tuple for multiple targets (for example, `(TYPE, BOOL)` for user-defined `TYPE` and `BOOL` template parameters)  |
| str | user-defined string to pasting  |
| pos | constant which specify numeration policy (see `VDC_PLAIN`, `VDC_ABS_POS`, `VDC_REL_POS` for details) |

#### Remarks ####

In case of using commas or parentheses see [Commas and parentheses in user-defined macros](#commas-and-parentheses-in-user-defined-macros) section.

Also see  [Parameter selection](#parameter-selection) section.

#### Header ####

*<variadic/token.hpp>*

#### Sample code ####

```cpp
#include <variadic/param_set.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>
#include <variadic/core.hpp>

#define TYPE 0 //type param
#define BOOL 1 //non-type param

#define PARAM_SET \
    VDC_MAKE_PARAM_SET(TYPE, BOOL) // ->{TYPE, BOOL}

#define MAX_ARITY 2

//overloading function template
#define HANDLER(r, params) \
    template< \
        VDC_EXPAND( \
            params, \
            VDC_ANY_PARAM, \
            VDC_TOKEN_IF(TYPE, typename T, VDC_REL_POS) \
            VDC_TOKEN_IF(BOOL, bool B, VDC_REL_POS) \
        ) \
    > \
    void f();

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER)
// ->
// template< typename T0 > void f();
// template< bool B0 > void f();
// template< typename T0, typename T1 > void f();
// template< typename T0, bool B0 > void f();
// template< bool B0, typename T0 > void f();
// template< bool B0, bool B1 > void f();
```

### VDC_TOKEN_FN_IF ###

```cpp
#define VDC_TOKEN_FN_IF(targets, fn, pos)  
```

#### Description ####

Invokes user-defined `fn` with parameter position (or with empty if  `VDC_PLAIN` flag has used) for every parameter, which matches with `targets`, and pasting `fn` expansion to output.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| targets | user-defined parameter as unary target, `VDC_ANY_PARAM` constant for any, or tuple for multiple targets (for example, `(TYPE, BOOL)` for user-defined `TYPE` and `BOOL` template parameters) |
| fn | user-defined unary function which accepts token position (parameter for number requires even if `VDC_PLAIN` flag used)  |
| pos | constant which specify numeration policy (see `VDC_PLAIN`, `VDC_ABS_POS`, `VDC_REL_POS` for details) |

#### Remarks ####

In case of using commas or parentheses within `fn` see [Commas and parentheses in user-defined macros](#commas-and-parentheses-in-user-defined-macros) section.

Also see [Token pasting](#token-pasting) and [Parameter selection](#parameter-selection) sections.

#### Header ####

*<variadic/token.hpp>*

#### Sample code ####

```cpp
#include <variadic/param_set.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>
#include <variadic/core.hpp>
#include <variadic/cat.hpp>

#define TYPE 0 //type param
#define BOOL 1 //non-type param

#define PARAM_SET \
    VDC_MAKE_PARAM_SET(TYPE, BOOL) // ->{TYPE, BOOL}

#define MAX_ARITY 2

//overloading function template
#define HANDLER(r, params) \
    template< \
        VDC_EXPAND( \
            params, \
            VDC_ANY_PARAM, \
            VDC_TOKEN_FN_IF(TYPE, TYPE_FN, VDC_REL_POS) \
            VDC_TOKEN_FN_IF(BOOL, BOOL_FN, VDC_REL_POS) \
        ) \
    > \
    void f();

#define TYPE_FN(n) VDC_CAT(typename T, n)
#define BOOL_FN(n) VDC_CAT(bool B, n)

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER)
// ->
// template< typename T0 > void f();
// template< bool B0 > void f();
// template< typename T0, typename T1 > void f();
// template< typename T0, bool B0 > void f();
// template< bool B0, typename T0 > void f();
// template< bool B0, bool B1 > void f();
```

## *<variadic/cat.hpp>* ##

### VDC_CAT ###

```cpp
#define VDC_CAT(a, b)  
```

#### Description ####

Concatenates two arguments after they have been expanded. Wrapper for `BOOST_PP_CAT`. 

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| a | the left token for the concatenation |
| b | the right token for the concatenation |

#### Remarks ####

Usable for pasting of parameter number within `fn` user-defined macro in `VDC_TOKEN_FN` and `VDC_TOKEN_FN_IF`. For details see [Token pasting](#token-pasting) section.

#### Header ####

*<variadic/cat.hpp>*

#### Sample code ####

```cpp
#include <variadic/cat.hpp>

#define MACRO(n) n

VDC_CAT(T, MACRO(0)) //-> T0

//result in case of T##MACRO(0) -> TMACRO(0)!
```

## *<variadic/size_of.hpp>* ##

### VDC_SIZE_OF ###

```cpp
#define VDC_SIZE_OF(targets, params)  
```

#### Description ####

Filters input parameter list by  `targets` and returns size of result.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| targets | user-defined parameter as unary target, `VDC_ANY_PARAM` constant for any, or tuple for multiple targets (for example, `(TYPE, BOOL)` for user-defined `TYPE` and `BOOL`  parameters) |
| params | parameter list for code generation produced by `VDC_PARAM_LIST` or `VDC_EMPTY` |

#### Remarks ####

Usable for conditional code generation in `HANDLER` user-defined macro.

#### Header ####

*<variadic/size_of.hpp>*

#### Sample code ####

```cpp
/* variadic */
#include <variadic/param_set.hpp>
#include <variadic/core.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>
#include <variadic/size_of.hpp>

/* boost/preprocessor */
#include <boost/preprocessor/punctuation/comma_if.hpp>

#define TYPE 0 //type param (typename T)
#define PARAM_SET VDC_MAKE_PARAM_SET(TYPE) //-> {TYPE}

#define MAX_ARITY 2

// overloading function template
#define HANDLER(r, params) \
    template< \
        int N \
        BOOST_PP_COMMA_IF( VDC_SIZE_OF(VDC_ANY_PARAM, params) ) \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN(typename T, VDC_ABS_POS)) > \
    void fn();

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER) // ->
// template< int N , typename T0 > void fn();
// template< int N , typename T0, typename T1 > void fn();

VDC_EMPTY(HANDLER) //-> template< int N > void fn();
```

## *<variadic/comma_if.hpp>* ##

### VDC_COMMA_IF ###

```cpp
#define VDC_COMMA_IF(targets, params)  
```
#### Description ####

Filters input parameter list by  `targets` and past comma if result is not empty.

#### Parameters ####

| parameter | description |
|:------------------:|:---------------------------------------------------------------------------|
| targets | user-defined parameter as unary target, `VDC_ANY_PARAM` constant for any, or tuple for multiple targets (for example, `(TYPE, BOOL)` for user-defined `TYPE` and `BOOL`  parameters) |
| params | parameter list for code generation produced by `VDC_PARAM_LIST` or `VDC_EMPTY` |

#### Remarks ####

Usable for conditional code generation in `HANDLER` user-defined macro.

#### Header ####

*<variadic/comma_if.hpp>*

#### Sample code ####

```cpp
/* variadic */
#include <variadic/param_set.hpp>
#include <variadic/core.hpp>
#include <variadic/token.hpp>
#include <variadic/expand.hpp>
#include <variadic/comma_if.hpp>

#define TYPE 0 //type param (typename T)
#define PARAM_SET VDC_MAKE_PARAM_SET(TYPE) //-> {TYPE}

#define MAX_ARITY 2

// overloading function template
#define HANDLER(r, params) \
    template< \
        int N \
        VDC_COMMA_IF(VDC_ANY_PARAM, params) \
        VDC_EXPAND(params, VDC_ANY_PARAM, VDC_TOKEN(typename T, VDC_ABS_POS)) > \
    void fn();

VDC_PARAM_LIST(PARAM_SET, MAX_ARITY, HANDLER) // ->
// template< int N , typename T0 > void fn();
// template< int N , typename T0, typename T1 > void fn();

VDC_EMPTY(HANDLER) //-> template< int N > void fn();
```

## *<variadic/version.hpp>* ##

### VDC_MAJOR ###

```cpp
#define VDC_MAJOR integer constant
```

#### Description ####

Major version of **Variadic**.

#### Header ####

*<variadic/version.hpp>*

### VDC_MINOR ###

```cpp
#define VDC_MINOR integer constant
```

#### Description ####

Minor version of **Variadic**.

#### Header ####

*<variadic/version.hpp>*

### VDC_PATCH ###

```cpp
#define VDC_PATCH integer constant
```

#### Description ####

Patch number for **Variadic**.

#### Header ####

*<variadic/version.hpp>*

### VDC_VERSION ###

```cpp
#define VDC_VERSION integer constant
```

#### Description ####

Current version of **Variadic** in format of integer constant (`VDC_MAJOR * 10000 + VDC_MINOR * 100 + VDC_PATCH`).

#### Header ####

*<variadic/version.hpp>*

### VDC_LIB_VERSION ###

```cpp
#define VDC_LIB_VERSION string
```

#### Description ####

Current version of **Variadic** in string format (`"VDC_MAJOR.VDC_MINOR.VDC_PATCH"`).

#### Header ####

*<variadic/version.hpp>*

----------
# Deployment #

## Dependences ##

**Variadic** has one external dependence - *boost/preprocessor* (tested with boost **1.72.0**). See [official site](https://www.boost.org) for details. Don't forget to specify *boost* include path in search paths of your IDE or build tool.

## Compilation ##

**Variadic** is header-only library, so compilation is not needed. For compilation of unit tests use Makefile or CMakeLists in *<variadic/test>* directory.

## Standard requirements ##

You need C++ compiler with support of variadic macro (C++11 and further). 

*<example/index_sequence.cpp>* requires C++14 compiler because of `std::index_sequence` use. 

## Compiler support ##

**Variadic** tested with **Apple LLVM version 10.0.0 (clang-1000.11.45.5)**

----------
# License #

Copyright 2020 Anatoliy Petrov
(apetrov.projects@gmail.com)

Distributed under the Boost Software License, Version 1.0.

See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt
