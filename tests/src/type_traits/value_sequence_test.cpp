#include <fmt/ranges.h>

#include "test.h"
#include "stdsharp/type_traits/value_sequence.h"

using namespace std;
using namespace fmt;
using namespace stdsharp;

using namespace type_traits;

using test_seq = value_sequence<0, 1, size_t{7}, 1, to_array("my literal")>;

TEMPLATE_TEST_CASE( // NOLINT
    "Scenario: value sequence",
    "[type traits]",
    test_seq,
    value_sequence<>
)
{
    STATIC_REQUIRE(default_initializable<TestType>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence get",
    "[type traits]",
    ((auto Index, auto Expect), Index, Expect),
    (0, 0),
    (1, 1),
    (2, 7),
    (3, 1)
)
{
    STATIC_REQUIRE(get<Index>(test_seq{}) == Expect);
}

TEMPLATE_TEST_CASE( // NOLINT
    "Scenario: value sequence invoke",
    "[type traits]",
    identity,
    decltype([](stdsharp::concepts::same_as_any<int, size_t, array<char, 11>> auto) {})
)
{
    STATIC_REQUIRE(invocable<test_seq::invoke_fn<>, TestType>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence find",
    "[type traits]",
    ((auto V, auto Expect), V, Expect),
    (0, 0),
    (1, 1),
    ('!', test_seq::size())
)
{
    STATIC_REQUIRE(test_seq::find(V) == Expect);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence find if",
    "[type traits]",
    ((auto Fn, auto Expect), Fn, Expect),
    ([](const auto&) { return true; }, 0),
    ([](const auto&) { return false; }, test_seq::size()),
    (
        []<typename T>(const T v)
        {
            if constexpr(same_as<T, size_t>) return v == 7;
            else return false;
        },
        2
    )
)
{
    STATIC_REQUIRE(test_seq::find_if(Fn) == Expect);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence count",
    "[type traits]",
    ((auto V, auto Expect), V, Expect),
    (0, 1),
    (1, 2),
    ('?', 0)
)
{
    STATIC_REQUIRE(test_seq::count(V) == Expect);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence count if",
    "[type traits]",
    ((auto Fn, auto Expect), Fn, Expect),
    ([](const auto&) { return true; }, test_seq::size()),
    ([](const auto&) { return false; }, 0),
    ([]<typename T>(const T) { return integral<T>; }, 4)
)
{
    STATIC_REQUIRE(test_seq::count_if(Fn) == Expect);
}

SCENARIO("apply_t", "[type traits]") // NOLINT
{
    STATIC_REQUIRE(default_initializable<test_seq::apply_t<value_sequence>>);
}

namespace // Escape Catch2 special characters like '[' and ']'
{
    constexpr auto transform_functor_1 = [](const int v) mutable { return v + 1; };
    constexpr auto transform_functor_2 = [](const int v) { return v + 42; };
    constexpr auto transform_functor_3 = [](const size_t v) { return v + 42; };
    constexpr auto transform_functor_4 = [](const int v) { return v + 6; };
    constexpr auto transform_functor_5 = []<auto Size>(const array<char, Size>& str)
    {
        return str[0];
    };
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence transform",
    "[type traits]",
    ((auto... Functor), Functor...),
    identity{},
    ( //
        transform_functor_1,
        transform_functor_2,
        transform_functor_3,
        transform_functor_4,
        transform_functor_5
    )
)
{
    STATIC_REQUIRE(default_initializable<test_seq::template transform_t<Functor...>>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence indexed",
    "[type traits]",
    ((typename Expect, auto... V), Expect, V...),
    (regular_value_sequence<1, size_t{7}>, 1, 2),
    (regular_value_sequence<size_t{7}, to_array("my literal")>, 2, 4)
)
{
    STATIC_REQUIRE(same_as<test_seq::indexed_t<V...>, Expect>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence append",
    "[type traits]",
    ( //
        (typename Expect, typename FrontExpect, auto... V),
        Expect,
        FrontExpect,
        V...
    ),
    ( //
        (regular_value_sequence<0, 1, size_t{7}, 1, to_array("my literal"), 1, 2>),
        regular_value_sequence<1, 2, 0, 1, size_t{7}, 1, to_array("my literal")>,
        1,
        2
    ),
    ( //
        (regular_value_sequence<0, 1, size_t{7}, 1, to_array("my literal"), 2, 4>),
        regular_value_sequence<2, 4, 0, 1, size_t{7}, 1, to_array("my literal")>,
        2,
        4
    )
)
{
    STATIC_REQUIRE(same_as<test_seq::append_t<V...>, Expect>);
    STATIC_REQUIRE(same_as<test_seq::append_front_t<V...>, FrontExpect>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence insert",
    "[type traits]",
    ((auto Index, typename Expect, auto... Element), Index, Expect, Element...),
    ( //
        3,
        regular_value_sequence<0, 1, size_t{7}, 1, 2, 1, to_array("my literal")>,
        1,
        2
    ),
    ( //
        5,
        regular_value_sequence<0, 1, size_t{7}, 1, to_array("my literal"), 2, 4>,
        2,
        4
    )
)
{
    STATIC_REQUIRE(same_as<test_seq::insert_t<Index, Element...>, Expect>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: value sequence remove at",
    "[type traits]",
    ((typename Expect, auto... I), Expect, I...),
    ( //
        regular_value_sequence<0, 1, to_array("my literal")>,
        1,
        2
    ),
    ( //
        regular_value_sequence<0, 1, 1>,
        2,
        4
    )
)
{
    STATIC_REQUIRE(same_as<test_seq::remove_at_t<I...>, Expect>);
}

TEMPLATE_TEST_CASE_SIG( // NOLINT
    "Scenario: unique value sequence",
    "[type traits]",
    ((auto V, typename Seq, typename Expect), V, Seq, Expect),
    (0, regular_value_sequence<>, regular_value_sequence<>),
    (0, regular_value_sequence<0, 10, 1, 5, 10, 1>, regular_value_sequence<0, 10, 1, 5>) //
)
{
    STATIC_REQUIRE( //
        same_as<
            typename as_value_sequence_t<Seq>:: //
            template apply_t<type_traits::unique_value_sequence_t>,
            Expect
        >
    );
}