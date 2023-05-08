#pragma once

#include <ranges>
#include <algorithm>
#include <stdexcept>

#include "../cassert/cassert.h"
#include "../functional/operations.h"

namespace stdsharp
{
    inline constexpr auto set_if = []<typename T, typename U, ::std::predicate<U, T> Comp>
        requires ::std::assignable_from<T&, U> // clang-format off
        (T& left, U&& right, Comp comp = {})
        noexcept(nothrow_predicate<Comp, U, T> && nothrow_assignable_from<T&, U>)
        -> T& // clang-format on
    {
        if(::std::invoke(cpp_move(comp), right, left)) left = cpp_forward(right);
        return left;
    };

    using set_if_fn = decltype(set_if);

    inline constexpr auto set_if_greater = []<typename T, typename U>
        requires ::std::invocable<set_if_fn, T&, U, ::std::ranges::greater> // clang-format off
        (T & left, U && right)
        noexcept(nothrow_invocable<set_if_fn, T&, U, ::std::ranges::greater>) -> T& // clang-format on
    {
        return set_if(left, cpp_forward(right), greater_v);
    };

    using set_if_greater_fn = decltype(set_if_greater);

    inline constexpr auto set_if_less = []<typename T, typename U>
        requires ::std::invocable<set_if_fn, T&, U, ::std::ranges::less> // clang-format off
        (T& left, U&& right)
        noexcept(nothrow_invocable<set_if_fn, T&, U, ::std::ranges::less>) -> T& // clang-format on
    {
        return set_if(left, cpp_forward(right), less_v);
    };

    using set_if_less_fn = decltype(set_if_less);

    inline constexpr struct is_between_fn
    {
        template<
            typename T,
            typename Proj = ::std::identity,
            ::std::indirect_strict_weak_order<::std::projected<const T*, Proj>> Compare =
                ::std::ranges::less // clang-format off
        > // clang-format on
        [[nodiscard]] constexpr auto operator()( // NOLINTBEGIN(*-easily-swappable-parameters)
            const T& t,
            const T& min,
            const T& max,
            Compare cmp = {},
            Proj proj = {}
        ) const // NOLINTEND(*-easily-swappable-parameters)
            noexcept( //
                !is_debug ||
                nothrow_predicate<
                    Compare,
                    ::std::projected<const T*, Proj>,
                    ::std::projected<const T*, Proj> // clang-format off
                > // clang-format on
            )
        {
            const auto& proj_max = ::std::invoke(proj, max);
            const auto& proj_min = ::std::invoke(proj, min);
            const auto& proj_t = ::std::invoke(proj, t);

            precondition<::std::invalid_argument>(
                [&] { return !::std::invoke(cmp, proj_max, proj_min); },
                "max value should not less than min value"
            );

            return !::std::invoke(cmp, proj_t, proj_min) && !::std::invoke(cmp, proj_max, proj_t);
        }
    } is_between{};

    template<::std::ranges::input_range TRng, ::std::ranges::input_range URng>
        requires ::std::three_way_comparable<
            ::std::ranges::range_const_reference_t<TRng>,
            ::std::ranges::range_const_reference_t<URng>>
    constexpr auto strict_compare(const TRng& left, const URng& right) noexcept
    {
        using ordering = ::std::partial_ordering;

        auto pre = ordering::equivalent;

        for(auto r_it = ::std::ranges::begin(right); const auto& v : left)
        {
            if(pre == ordering::unordered) return ordering::unordered;

            const ordering next = ::std::compare_three_way{}(v, *r_it);

            if(pre == next || is_eq(next)) continue;
            if(is_eq(pre))
            {
                pre = next;
                continue;
            }

            pre = is_gt(pre) ? is_gt(next) ? ordering::greater : ordering::unordered :
                is_lt(next)  ? ordering::less :
                               ordering::unordered;
        }

        return pre;
    }
}

#undef INVALID_ARGUMENT