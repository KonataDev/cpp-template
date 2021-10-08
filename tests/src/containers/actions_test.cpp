#include "containers/actions_test.h"
#include "containers/actions.h"
#include "type_traits/object.h"

namespace stdsharp::test::containers::actions
{
    using namespace std;
    using namespace boost::ut;
    using namespace bdd;

    namespace
    {
        template<typename T>
        using dummy_predicate_t = bool(const T&);

        template<typename T>
        concept vec_req = requires(
            vector<T> v,
            decltype(v.cbegin()) iter,
            T value,
            dummy_predicate_t<T> dummy_predicate //
        )
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
        concept set_req = requires(
            set<T> v,
            decltype(v.cbegin()) iter,
            T value,
            dummy_predicate_t<T> dummy_predicate //
        )
        {
            stdsharp::containers::actions::emplace(v, move(value));

            stdsharp::containers::actions::erase(v, value);
            stdsharp::containers::actions::erase(v, iter);
            stdsharp::containers::actions::erase(v, iter, iter);
            stdsharp::containers::actions::erase_if(v, dummy_predicate);
        };

        template<typename T>
        concept unordered_map_req = requires(
            unordered_map<T, int> v,
            decltype(v.cbegin()) iter,
            T value,
            dummy_predicate_t<pair<const T, int>> dummy_predicate //
        )
        {
            stdsharp::containers::actions::emplace(v, move(value), 0);

            stdsharp::containers::actions::erase(v, value);
            stdsharp::containers::actions::erase(v, iter);
            stdsharp::containers::actions::erase(v, iter, iter);
            stdsharp::containers::actions::erase_if(v, dummy_predicate);
        };

        void vector_actions_test()
        {
            feature("vector actions") = []<typename T>(const type_identity<T>)
            {
                println(fmt::format("current type {}", reflection::type_name<T>()));
                static_expect<vec_req<T>>();
            } | tuple{type_identity<int>{}, type_identity<unique_ptr<float>>{}};
        }

        void set_actions_test()
        {
            feature("set actions") = []<typename T>(const type_identity<T>)
            {
                println(fmt::format("current type {}", reflection::type_name<T>()));
                static_expect<set_req<T>>();
            } | tuple{type_identity<int>{}};
        }

        void unordered_map_actions_test()
        {
            feature("unordered map actions") = []<typename T>(const type_identity<T>)
            {
                println(fmt::format("current type {}", reflection::type_name<T>()));
                static_expect<unordered_map_req<T>>();
            } | tuple{type_identity<int>{}, type_identity<unique_ptr<long>>{}};
        }
    }

    boost::ut::suite& actions_test()
    {
        static boost::ut::suite suite = []
        {
            vector_actions_test();
            set_actions_test();
            unordered_map_actions_test();
        };

        return suite;
    }
}
