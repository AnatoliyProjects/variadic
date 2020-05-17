# Copyright 2020 Anatoliy Petrov
# (apetrov.projects@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0.
#
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt


# output file name
OUT=VariadicTest

# includes
VARIADIC= ../include/

# compiler
CC=clang++

# compiler flags
CFLAGS= -Wall \
        -stdlib=libc++ \
        -std=c++11 \
        -o $(OUT) \
        -I $(VARIADIC) \
        -I $(BOOST_INCLUDE_DIR)

# messages
ifndef BOOST_INCLUDE_DIR
$(error BOOST_INCLUDE_DIR is not set)
endif

$(info BOOST_INCLUDE_DIR=$(BOOST_INCLUDE_DIR))

all: all.cpp
	$(CC) $(CFLAGS) all.cpp