#include "containers/actions_test.h"
#include "containers/actions.h"

namespace stdsharp::test::containers::actions
{
    using namespace std;
    using namespace boost::ut;
    using namespace bdd;

    inline constexpr auto dummy_predicate = [](const auto&) { return true; };

    template<typename T>
    concept vec_req = requires(vector<T> v, decltype(v.cbegin()) iter, T value)
    {
        stdsharp::containers::actions::emplace(v, iter, move(value));
        stdsharp::containers::actions::emplace_back(v, move(value));
        stdsharp::containers::actions::emplace_front(v, move(value));

        stdsharp::containers::actions::erase(v, value);
        stdsharp::containers::actions::erase(v, iter);
        stdsharp::containers::actions::erase(v, iter, iter);
        stdsharp::containers::actions::erase_if(v, dummy_predicate);

        stdsharp::containers::actions::pop_front(v);
        stdsharp::containers::actions::pop_back(v);

        stdsharp::containers::actions::resize(v, 0);
    };

    template<typename T>
    concept set_req = requires(set<T> v, decltype(v.cbegin()) iter, T value)
    {
        stdsharp::containers::actions::emplace(v, move(value));

        stdsharp::containers::actions::erase(v, value);
        stdsharp::containers::actions::erase(v, iter);
        stdsharp::containers::actions::erase(v, iter, iter);
        stdsharp::containers::actions::erase_if(v, dummy_predicate);
    };

    template<typename T>
    concept unordered_map_req =
        requires(unordered_map<T, int> v, decltype(v.cbegin()) iter, T value)
    { // clang-format off
        requires requires { emplace(v, 0); } == false; // clang-format on

        stdsharp::containers::actions::emplace(v, move(value), 0);

        stdsharp::containers::actions::erase(v, value);
        stdsharp::containers::actions::erase(v, iter);
        stdsharp::containers::actions::erase(v, iter, iter);
        stdsharp::containers::actions::erase_if(v, dummy_predicate);
    };

    boost::ut::suite& actions_test()
    {
        static boost::ut::suite suite = []
        {
            feature("container actions") = []<typename T>(const type_identity<T>)
            {
                static_expect<vec_req<T>>();
                static_expect<set_req<T>>();
                static_expect<unordered_map_req<T>>();
            } | tuple{type_identity<int>{}, type_identity<unique_ptr<int>>{}};
        };

        return suite;
    }
}
