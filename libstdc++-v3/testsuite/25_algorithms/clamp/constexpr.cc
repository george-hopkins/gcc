// { dg-options "-std=gnu++17" }
// { dg-do compile }

// Copyright (C) 2016 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <algorithm>
#include <functional>

#ifndef __cpp_lib_clamp
# error "Feature-test macro for clamp missing"
#elif __cpp_lib_clamp != 201603
# error "Feature-test macro for clamp has wrong value"
#endif

static_assert(std::clamp(2, 0, 1) == 1, "");
static_assert(std::clamp(2, 0, 1, std::greater<int>()) == 0, "");
