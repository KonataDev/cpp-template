#pragma once

#include <iterator>
#include <memory>

#include "../functional/invocables.h"
#include "../type_traits/special_member.h"
#include "../utility/constructor.h"

namespace stdsharp
{
    using max_alignment = std::alignment_of<std::max_align_t>;

    inline constexpr auto max_alignment_v = max_alignment::value;

    namespace details
    {
        struct alloc_req_dummy_t
        {
            int v;
        };

        template<typename T>
        concept allocator_pointer =
            nullable_pointer<T> && nothrow_default_initializable<T> && nothrow_movable<T> &&
            nothrow_swappable<T> && nothrow_copyable<T> && nothrow_weakly_equality_comparable<T> &&
            nothrow_weakly_equality_comparable_with<T, std::nullptr_t>;
    }

    template<typename Alloc>
    concept allocator_req = requires //
    {
        typename Alloc::value_type;
        nothrow_copyable<Alloc>;

        requires requires(
            Alloc alloc,
            std::allocator_traits<Alloc> t_traits,
            decltype(t_traits)::pointer p,
            decltype(t_traits)::const_pointer const_p,
            decltype(t_traits)::void_pointer void_p,
            decltype(t_traits)::const_void_pointer const_void_p,
            decltype(t_traits)::value_type v,
            decltype(t_traits)::size_type size,
            decltype(t_traits):: //
            template rebind_traits<details::alloc_req_dummy_t> u_traits,
            decltype(t_traits)::is_always_equal always_equal,
            decltype(t_traits)::propagate_on_container_copy_assignment copy_assign,
            decltype(t_traits)::propagate_on_container_move_assignment move_assign,
            decltype(t_traits)::propagate_on_container_swap swap // clang-format off
        ) // clang-format on
        {
            requires !const_volatile<decltype(v)>;

            requires details::allocator_pointer<decltype(p)> &&
                details::allocator_pointer<decltype(const_p)> &&
                details::allocator_pointer<decltype(void_p)> &&
                details::allocator_pointer<decltype(const_void_p)>;

            requires std::random_access_iterator<decltype(p)> &&
                std::contiguous_iterator<decltype(p)>;

            requires nothrow_convertible_to<decltype(p), decltype(const_p)> &&
                std::random_access_iterator<decltype(const_p)> &&
                std::contiguous_iterator<decltype(const_p)>;

            requires nothrow_convertible_to<decltype(p), decltype(void_p)> &&
                nothrow_explicitly_convertible<decltype(void_p), decltype(p)> &&
                std::same_as<decltype(void_p), typename decltype(u_traits)::void_pointer>;

            requires nothrow_convertible_to<decltype(p), decltype(const_void_p)> &&
                nothrow_convertible_to<decltype(const_p), decltype(const_void_p)> &&
                nothrow_explicitly_convertible<decltype(const_void_p), decltype(const_p)> &&
                nothrow_convertible_to<
                         decltype(void_p),
                         decltype(const_void_p)> &&
                std::same_as< // clang-format off
                    decltype(const_void_p),
                    typename decltype(u_traits)::const_void_pointer
                >; // clang-format on

            requires std::unsigned_integral<decltype(size)>;

            requires std::signed_integral<typename decltype(t_traits)::difference_type>;

            requires std::
                same_as<typename decltype(u_traits)::template rebind_alloc<decltype(v)>, Alloc>;
            // clang-format off

            { *p } -> std::same_as<std::add_lvalue_reference_t<decltype(v)>>;
            { *const_p } -> std::same_as<add_const_lvalue_ref_t<decltype(v)>>;
            { *const_p } -> std::same_as<add_const_lvalue_ref_t<decltype(v)>>; // clang-format on

            requires requires(
                decltype(u_traits)::pointer other_p,
                decltype(u_traits)::const_pointer other_const_p,
                decltype(u_traits)::allocator_type u_alloc,
                decltype(t_traits)::allocator_type another_alloc // clang-format off
            )
            {
                nothrow_constructible_from<Alloc, const decltype(u_alloc)&>;
                nothrow_constructible_from<Alloc, decltype(u_alloc)>;

                { other_p->v } -> std::same_as<decltype(((*other_p).v))>;
                { other_const_p->v } -> std::same_as<decltype(((*other_const_p).v))>;

                t_traits.construct(alloc, other_p);
                t_traits.destroy(alloc, other_p);

                requires noexcept(alloc == another_alloc) && noexcept(alloc != another_alloc);
            }; // clang-format on

            std::pointer_traits<decltype(p)>::pointer_to(v); // clang-format off

            { t_traits.allocate(alloc, size) } -> std::same_as<decltype(p)>;
            { t_traits.allocate(alloc, size, const_void_p) } -> std::same_as<decltype(p)>;
            noexcept(alloc.deallocate(p, size));
            { t_traits.max_size(alloc) } -> std::same_as<decltype(size)>;
            // clang-format on

            requires std::derived_from<decltype(always_equal), std::true_type> ||
                std::derived_from<decltype(always_equal), std::false_type>;

            // clang-format off
            { t_traits.select_on_container_copy_construction(alloc) } -> std::same_as<Alloc>;
            // clang-format on

            requires std::derived_from<decltype(copy_assign), std::true_type> &&
                    nothrow_copy_assignable<Alloc> ||
                    std::derived_from<decltype(copy_assign), std::false_type>;

            requires std::derived_from<decltype(move_assign), std::true_type> &&
                    nothrow_move_assignable<Alloc> ||
                    std::derived_from<decltype(move_assign), std::false_type>;

            requires std::derived_from<decltype(swap), std::true_type> &&
                    nothrow_swappable<Alloc> || std::derived_from<decltype(swap), std::false_type>;
        };
    };

    namespace details
    {
        template<typename T>
        struct make_obj_using_allocator_fn
        {
            template<typename Alloc, typename... Args>
                requires std::constructible_from<T, Args..., const Alloc&>
            constexpr T operator()(const Alloc& alloc, Args&&... args) const
                noexcept(nothrow_constructible_from<T, Args..., const Alloc&>)
            {
                return T{cpp_forward(args)..., alloc};
            }

            template<typename Alloc, typename... Args>
                requires std::constructible_from<T, std::allocator_arg_t, const Alloc&, Args...>
            constexpr T operator()(const Alloc& alloc, Args&&... args) const
                noexcept(nothrow_constructible_from<T, std::allocator_arg_t, const Alloc&, Args...>)
            {
                return T{std::allocator_arg, alloc, cpp_forward(args)...};
            }
        };
    }

    template<typename T>
    using make_obj_uses_allocator_fn =
        sequenced_invocables<details::make_obj_using_allocator_fn<T>, constructor<T>>;

    template<typename T>
    inline constexpr make_obj_uses_allocator_fn<T> make_obj_uses_allocator{};

    template<allocator_req Alloc>
    struct allocator_traits : private std::allocator_traits<Alloc>
    {
    private: // NOLINTBEGIN(*-owning-memory)
        struct default_constructor
        {
            template<typename T, typename... Args>
            constexpr decltype(auto) operator()(const Alloc&, T* const ptr, Args&&... args) const
                noexcept(noexcept(std::ranges::construct_at(ptr, std::declval<Args>()...)))
                requires requires { std::ranges::construct_at(ptr, std::declval<Args>()...); }
            {
                return std::ranges::construct_at(ptr, cpp_forward(args)...);
            }

            template<typename T, typename... Args>
            constexpr decltype(auto) operator()(
                const Alloc&,
                void* const ptr,
                const std::in_place_type_t<T>,
                Args&&... args
            ) const noexcept(noexcept(std::ranges::construct_at(ptr, std::declval<Args>()...)))
                requires requires { std::ranges::construct_at(ptr, std::declval<Args>()...); }
            {
                return ::new(ptr) T{cpp_forward(args)...};
            }
        };

        struct using_alloc_ctor
        {
            template<typename T, typename... Args>
                requires std::constructible_from<T, Args..., const Alloc&>
            constexpr void operator()(const Alloc& alloc, T* const ptr, Args&&... args) //
                const noexcept(stdsharp::nothrow_constructible_from<T, Args..., const Alloc&>)
            {
                std::ranges::construct_at(ptr, cpp_forward(args)..., alloc);
            }

            template<typename T, typename... Args, typename Tag = std::allocator_arg_t>
                requires std::constructible_from<T, Tag, const Alloc&, Args...>
            constexpr void operator()(const Alloc& alloc, T* const ptr, Args&&... args) //
                const noexcept(stdsharp::nothrow_constructible_from<T, Tag, const Alloc&, Args...>)
            {
                std::ranges::construct_at(ptr, std::allocator_arg, alloc, cpp_forward(args)...);
            }

            template<typename T, typename... Args>
                requires std::constructible_from<T, Args..., const Alloc&>
            constexpr void operator()(
                const Alloc& alloc,
                void* const ptr,
                const std::in_place_type_t<T>,
                Args&&... args
            ) const noexcept(stdsharp::nothrow_constructible_from<T, Args..., const Alloc&>)
            {
                ::new(ptr) T{cpp_forward(args)..., alloc};
            }

            template<typename T, typename... Args, typename Tag = std::allocator_arg_t>
                requires std::constructible_from<T, Tag, const Alloc&, Args...>
            constexpr void operator()(
                const Alloc& alloc,
                void* const ptr,
                const std::in_place_type_t<T>,
                Args&&... args
            ) const noexcept(stdsharp::nothrow_constructible_from<T, Tag, const Alloc&, Args...>)
            {
                ::new(ptr) T{std::allocator_arg, alloc, cpp_forward(args)...};
            }
        };

        struct custom_constructor
        {
            template<typename T, typename... Args>
            constexpr void operator()(Alloc& a, T* const ptr, Args&&... args) const
                noexcept(noexcept(a.construct(ptr, std::declval<Args>()...)))
                requires requires { a.construct(ptr, std::declval<Args>()...); }
            {
                a.construct(ptr, cpp_forward(args)...);
            }
        }; // NOLINTEND(*-owning-memory)

        using m_base = std::allocator_traits<Alloc>;

    public:
        using constructor =
            sequenced_invocables<custom_constructor, using_alloc_ctor, default_constructor>;

        static constexpr constructor construct{};

    private:
        struct custom_destructor
        {
            template<typename U>
            constexpr void operator()(Alloc& a, U* const ptr) const noexcept
                requires(noexcept(a.destroy(ptr)))
            {
                a.destroy(ptr);
            }
        };

        struct default_destructor
        {
            template<typename U>
            constexpr void operator()(const Alloc&, U* const ptr) const noexcept
            {
                std::ranges::destroy_at(ptr);
            }
        };

    public:
        using destructor = sequenced_invocables<custom_destructor, default_destructor>;

        static constexpr destructor destroy{};

        template<typename T, typename... Args>
        static constexpr auto constructible_from = std::invocable<constructor, Alloc&, T*, Args...>;

        template<typename T>
        static constexpr auto cp_constructible = constructible_from<T, const T&>;

        template<typename T>
        static constexpr auto mov_constructible = constructible_from<T, T>;

        template<typename T, typename... Args>
        static constexpr auto nothrow_constructible_from =
            nothrow_invocable<constructor, Alloc&, T*, Args...>;

        template<typename T>
        static constexpr auto nothrow_cp_constructible = nothrow_constructible_from<T, const T&>;

        template<typename T>
        static constexpr auto nothrow_mov_constructible = nothrow_constructible_from<T, T>;

        template<typename T>
        static constexpr auto destructible = std::invocable<destructor, Alloc&, T*>;

        template<typename T>
        static constexpr auto nothrow_destructible = nothrow_invocable<destructor, Alloc&, T*>;

        using typename m_base::allocator_type;
        using typename m_base::const_pointer;
        using typename m_base::const_void_pointer;
        using typename m_base::difference_type;
        using typename m_base::is_always_equal;
        using typename m_base::pointer;
        using typename m_base::propagate_on_container_copy_assignment;
        using typename m_base::propagate_on_container_move_assignment;
        using typename m_base::propagate_on_container_swap;
        using typename m_base::size_type;
        using typename m_base::value_type;
        using typename m_base::void_pointer;

#if __cpp_lib_allocate_at_least >= 202302L
        using m_base::allocate_at_least;
#endif

        static constexpr auto propagate_on_copy_v = propagate_on_container_copy_assignment::value;

        static constexpr auto propagate_on_move_v = propagate_on_container_move_assignment::value;

        static constexpr auto propagate_on_swap_v = propagate_on_container_swap::value;

        static constexpr auto always_equal_v = is_always_equal::value;

        template<typename U>
        using rebind_alloc = m_base::template rebind_alloc<U>;

        template<typename U>
        using rebind_traits = m_base::template rebind_traits<U>;

        using m_base::max_size;
        using m_base::select_on_container_copy_construction;

        static constexpr pointer allocate(
            allocator_type& alloc,
            const size_type count,
            const const_void_pointer hint = nullptr
        )
        {
            return hint == nullptr ? m_base::allocate(alloc, count) :
                                     m_base::allocate(alloc, count, hint);
        }

        static constexpr void
            deallocate(allocator_type& alloc, pointer ptr, const size_type count) noexcept
        {
            m_base::deallocate(alloc, ptr, count);
        }

        static constexpr pointer try_allocate(
            allocator_type& alloc,
            const size_type count,
            const const_void_pointer hint = nullptr
        ) noexcept
        {
            if(max_size(alloc) < count) return nullptr;

            try
            {
                return allocate(alloc, count, hint);
            }
            catch(...)
            {
                return nullptr;
            }
        }

        static constexpr auto try_allocate(
            allocator_type& alloc,
            const size_type count,
            [[maybe_unused]] const const_void_pointer hint = nullptr
        ) noexcept
            requires requires // clang-format off
        {
            { alloc.try_allocate(count) } -> std::same_as<pointer>; // clang-format on
            requires noexcept(alloc.try_allocate(count));
        }
        {
            if constexpr( // clang-format off
                requires
                {
                    { alloc.try_allocate(count, hint) } ->
                        std::same_as<pointer>; // clang-format on
                    requires noexcept(alloc.try_allocate(count, hint));
                } //
            )
                if(hint != nullptr) return alloc.try_allocate(count, hint);

            return alloc.try_allocate(count);
        }
    };

    template<allocator_req Alloc>
    using allocator_pointer = allocator_traits<Alloc>::pointer;

    template<allocator_req Alloc>
    using allocator_size_type = allocator_traits<Alloc>::size_type;

    template<allocator_req Alloc>
    using allocator_cvp = allocator_traits<Alloc>::const_void_pointer;

    template<typename>
    struct allocator_of;

    template<typename T>
        requires requires { typename T::allocator_type; }
    struct allocator_of<T> : std::type_identity<typename T::allocator_type>
    {
    };

    template<typename T>
    using allocator_of_t = ::meta::_t<allocator_of<T>>;
}