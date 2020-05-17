# Copyright 2020 Anatoliy Petrov
# (apetrov.projects@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0.
#
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt


cmake_minimum_required(VERSION 3.10)

project(VariadicTest VERSION 1.0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(VariadicTest all.cpp)

target_include_directories(VariadicTest PUBLIC 
			   "../include"
			   "${OPT_BOOST_INCLUDE_DIR}")