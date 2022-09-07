#pragma once

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <fmt/core.h>

using namespace std;
using namespace fmt;

template<typename T>
constexpr ::std::string_view type() noexcept
{
    return __func__; // NOLINT
}