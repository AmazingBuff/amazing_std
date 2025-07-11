#pragma once

#include <type_traits>
#include <utility>
#include "astd/base/macro.h"

AMAZING_NAMESPACE_BEGIN

namespace Trait
{
	template<typename T>
	concept is_function = std::is_function_v<T>;

	template<typename T>
	concept not_function = !is_function<T>;

	template<typename T>
	concept is_function_pointer = not_function<T> && (is_function<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>);

	template<typename T>
	concept not_function_pointer = !is_function_pointer<T>;


	template<typename T>
	struct is_template : std::false_type {};

	template<template <typename...> typename Tp, typename... Args>
	struct is_template<Tp<Args...>> : std::true_type {};

	template<typename T>
	static constexpr bool is_template_v = is_template<T>::value;

	template<typename T, typename U>
	struct is_same_template : std::false_type {};

	template<template <typename...> typename Tp, typename... ArgsT, typename... ArgsU>
	struct is_same_template<Tp<ArgsT...>, Tp<ArgsU...>> : std::true_type {};

	template<typename T, typename U>
	static constexpr bool is_same_template_v = is_same_template<T, U>::value;


	template<size_t Idx, template <typename...> typename List, typename... Args>
	struct type_element;

	template<size_t Idx, template <typename...> typename List, typename First, typename... Args>
	struct type_element<Idx, List, First, Args...>
	{
		using type = typename type_element<Idx - 1, List, Args...>::type;
		using list = typename type_element<Idx - 1, List, Args...>::list;
	};

	template<template <typename...> typename List, typename First, typename... Args>
	struct type_element<0, List, First, Args...>
	{
		using type = First;
		using list = List<First, Args...>;
	};

	template<size_t Idx, template <typename...> typename List, typename... Args>
	using type_element_t = typename type_element<Idx, List, Args...>::type;


	struct null_type {};

	template<typename... Args>
	struct type_list;

	template<typename First, typename... Args>
	struct type_list<First, Args...> : private type_list<Args...>
	{
		using type = First;
		using remain = type_list<Args...>;

		type_list() = default;

		explicit type_list(First&& first, Args&&... args)
			requires(!std::is_class_v<First>)
			: type_list<Args...>(std::forward<Args>(args)...), m_data(std::forward<First>(first)) {}

		explicit type_list(const First& first, Args&&... args)
			requires(std::is_class_v<First>)
			: type_list<Args...>(std::forward<Args>(args)...), m_data(first) {}

		type_list(const type_list& other)
			: type_list<Args...>(other.get_rest()), m_data(other.m_data) {}

		type_list(type_list&& other) noexcept
			: type_list<Args...>(std::move(other.get_rest())), m_data(std::move(other.m_data)) {}

		constexpr type_list& operator=(const type_list& other)
		{
			m_data = other.m_data;
			get_rest() = other.get_rest();
			return *this;
		}

		constexpr const type_list& operator=(const type_list& other) const
		{
			m_data = other.m_data;
			get_rest() = other.get_rest();
			return *this;
		}

		template<size_t Idx, typename... UArgs>
		friend constexpr const type_element_t<Idx, type_list, UArgs...>& get_value(const type_list<UArgs...>& list);

		template<size_t Idx, typename... UArgs>
		friend constexpr const type_element_t<Idx, type_list, UArgs...>&& get_value(const type_list<UArgs...>&& list);

		template<size_t Idx, typename... UArgs>
		friend constexpr type_element_t<Idx, type_list, UArgs...>& get_value(type_list<UArgs...>& list);

		template<size_t Idx, typename... UArgs>
		friend constexpr type_element_t<Idx, type_list, UArgs...>&& get_value(type_list<UArgs...>&& list);


		constexpr type_list<Args...>& get_rest()
		{
			return *this;
		}

		constexpr const type_list<Args...>& get_rest() const
		{
			return *this;
		}

		type m_data = type();
	};

	template<>
	struct type_list<>
	{
		null_type m_data;

		type_list& operator=(const type_list&)
		{
			return *this;
		}
	};

	template<size_t Idx, typename... Args>
	constexpr const type_element_t<Idx, type_list, Args...>& get_value(const type_list<Args...>& list)
	{
		using type = typename type_element<Idx, type_list, Args...>::list;
		return static_cast<const type&>(list).m_data;
	}

	template<size_t Idx, typename... Args>
	constexpr const type_element_t<Idx, type_list, Args...>&& get_value(const type_list<Args...>&& list)
	{
		using type = typename type_element<Idx, type_list, Args...>::list;
		return static_cast<const type&&>(list).m_data;
	}

	template<size_t Idx, typename... Args>
	constexpr type_element_t<Idx, type_list, Args...>& get_value(type_list<Args...>& list)
	{
		using type = typename type_element<Idx, type_list, Args...>::list;
		return static_cast<type&>(list).m_data;
	}

	template<size_t Idx, typename... Args>
	constexpr type_element_t<Idx, type_list, Args...>&& get_value(type_list<Args...>&& list)
	{
		using type = typename type_element<Idx, type_list, Args...>::list;
		return static_cast<type&&>(list).m_data;
	}


	template<typename>
	struct function_traits;

	template<typename Ret, typename... Args>
	struct function_traits<Ret(Args...)>
	{
		using return_type = Ret;
		using argument_type = type_list<Args...>;

		static constexpr bool is_member_function = false;
	};

	template<typename Ret, typename... Args>
	struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

	template<typename Class, typename Ret, typename... Args>
	struct function_traits<Ret(Class::*)(Args...)>
	{
		using class_type = Class;
		using return_type = Ret;
		using argument_type = type_list<Args...>;

		static constexpr bool is_member_function = true;
		static constexpr bool is_const_member_function = false;
	};

	template<typename Class, typename Ret, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const> : function_traits<Ret(Class::*)(Args...)>
	{
		static constexpr bool is_const_member_function = true;
	};


	template<typename T>
	struct variable_traits
	{
		using type = T;
		using parameter_type = T;

		static constexpr bool is_const_variable = false;
		static constexpr bool is_volatile_variable = false;
		static constexpr bool is_pointer_variable = false;
		static constexpr bool is_left_reference_variable = false;
		static constexpr bool is_right_reference_variable = false;
		static constexpr bool is_member_variable = false;
	};

	template<not_function T>
	struct variable_traits<const T> : variable_traits<T>
	{
		using parameter_type = const T;
		static constexpr bool is_const_variable = true;
	};

	template<typename T>
	struct variable_traits<volatile T> : variable_traits<T>
	{
		using parameter_type = volatile T;
		static constexpr bool is_volatile_variable = true;
	};

	template<not_function T>
	struct variable_traits<T*> : variable_traits<T>
	{
		using parameter_type = T*;
		static constexpr bool is_pointer_variable = true;
	};

	template<typename T>
	struct variable_traits<T&&> : variable_traits<T>
	{
		using parameter_type = T&&;
		static constexpr bool is_right_reference_variable = true;
	};

	template<typename T>
	struct variable_traits<T&> : variable_traits<T>
	{
		using parameter_type = T&;
		static constexpr bool is_left_reference_variable = true;
	};

	template<typename Ret, typename Class, typename... Args>
	struct variable_traits<Ret(Class::*)(Args...)> : variable_traits<Ret(*)(Args...)>
	{
		using class_type = Class;
		using parameter_type = Ret(Class::*)(Args...);
		using return_type = Ret;
		using argument_type = type_list<Args...>;
		static constexpr bool is_member_variable = true;
	};

	template<typename Ret, typename Class, typename... Args>
	struct variable_traits<Ret(Class::*)(Args...) const> : variable_traits<Ret(*)(Args...)>
	{
		using class_type = Class;
		using parameter_type = Ret(Class::*)(Args...);
		using return_type = Ret;
		using argument_type = type_list<Args...>;
		static constexpr bool is_member_variable = true;
	};

	template<not_function T, typename Class>
	struct variable_traits<T Class::*> : variable_traits<T>
	{
		using class_type = Class;
		using parameter_type = T Class::*;

		static constexpr bool is_member_variable = true;
	};

	template<typename T>
	struct remove_qualifier
	{
		using type = typename variable_traits<T>::type;
	};

	template<typename T>
	using remove_qualifier_t = typename remove_qualifier<T>::type;


	template<typename T>
	struct template_traits;

	template<template <typename...> typename Tp, typename... Args>
	struct template_traits<Tp<Args...>>
	{
		using type = type_list<Args...>;

		template<typename... T>
		using apply = Tp<T...>;
	};

	template<typename T, typename... Args>
	using apply_template = typename template_traits<T>::template apply<Args...>;


	template<typename F, typename Arg, typename... Args>
	typename function_traits<std::decay_t<F>>::return_type invoke(F&& f, Arg&& arg, Args&&... args)
	{
		if constexpr (function_traits<std::decay_t<F>>::is_member_function)
		{
			if constexpr (std::is_pointer_v<Arg>)
				return ((*static_cast<Arg&&>(arg)).*f)(std::forward<Args>(args)...);
			else
				return (static_cast<Arg&&>(arg).*f)(std::forward<Args>(args)...);
		}
		else
			return static_cast<F&&>(f)(std::forward<Arg>(arg), std::forward<Args>(args)...);
	}

	namespace Internal
	{
		template<typename F, typename... Args>
		struct is_invocable
		{
			static constexpr bool value = std::is_invocable_v<F, Args...> || std::is_nothrow_invocable_v<F, Args...>;
		};

		template<typename F, typename... Args>
		struct is_invocable<F, type_list<Args...>>
		{
			static constexpr bool value = std::is_invocable_v<F, Args...> || std::is_nothrow_invocable_v<F, Args...>;
		};

		template<typename Ret, typename F, typename... Args>
		struct is_invocable_r
		{
			static constexpr bool value = std::is_invocable_r_v<Ret, F, Args...> || std::is_nothrow_invocable_r_v<Ret, F, Args...>;
		};

		template<typename Ret, typename F, typename... Args>
		struct is_invocable_r<Ret, F, type_list<Args...>>
		{
			static constexpr bool value = std::is_invocable_r_v<Ret, F, Args...> || std::is_nothrow_invocable_r_v<Ret, F, Args...>;
		};

		template<typename... T>
		struct concat_types
		{
			using type = type_list<T...>;
		};

		template<typename... T, typename... U>
		struct concat_types<type_list<T...>, type_list<U...>>
		{
			using type = type_list<T..., U...>;
		};

		template<typename... T, typename... U>
		struct concat_types<type_list<T...>, U...>
		{
			using type = type_list<T..., U...>;
		};

		template<typename... Args>
		struct head_type;

		template<typename Head, typename... Args>
		struct head_type<Head, Args...>
		{
			using type = Head;
			using remain = type_list<Args...>;
		};

		template<typename Head, typename... Args>
		struct head_type<type_list<Head, Args...>>
		{
			using type = Head;
			using remain = type_list<Args...>;
		};

		template<>
		struct head_type<>
		{
			using type = type_list<>;
			using remain = type_list<>;
		};

		template<>
		struct head_type<type_list<>>
		{
			using type = type_list<>;
			using remain = type_list<>;
		};

		template<size_t N, typename... Args>
		struct remove_head;

		template<size_t N, typename Head, typename... Args>
		struct remove_head<N, Head, Args...>
		{
			using type = typename remove_head<N - 1, type_list<Args...>>::type;
		};

		template<size_t N, typename Head, typename... Args>
		struct remove_head<N, type_list<Head, Args...>>
		{
			using type = typename remove_head<N - 1, type_list<Args...>>::type;
		};

		template<typename Head, typename... Args>
		struct remove_head<0, Head, Args...>
		{
			using type = type_list<Head, Args...>;
		};

		template<typename Head, typename... Args>
		struct remove_head<0, type_list<Head, Args...>>
		{
			using type = type_list<Head, Args...>;
		};

		template<size_t N>
		struct remove_head<N>
		{
			using type = type_list<>;
		};

		template<size_t N>
		struct remove_head<N, type_list<>>
		{
			using type = type_list<>;
		};

		template<size_t N, typename... Args>
		struct get_type
		{
			using type = typename head_type<typename remove_head<N, Args...>::type>::type;
		};

		template<size_t N, typename... Args>
		struct get_type<N, type_list<Args...>>
		{
			using type = typename head_type<typename remove_head<N, Args...>::type>::type;
		};

		template<size_t N, typename... Args>
		struct reverse_type
		{
			using type = typename reverse_type<N - 1, typename concat_types<typename head_type<Args...>::remain, typename head_type<Args...>::type>::type>::type;
		};

		template<typename... Args>
		struct reverse_type<0, type_list<Args...>>
		{
			using type = type_list<Args...>;
		};

		template<size_t N, typename... Args>
		struct reverse_type<N, type_list<Args...>>
		{
			using type = typename reverse_type<N - 1, typename concat_types<typename head_type<Args...>::remain, typename head_type<Args...>::type>::type>::type;
		};

		template<typename... Args>
		struct count_types
		{
			static constexpr size_t value = count_types<typename head_type<Args...>::remain>::value + 1;
		};

		template<typename... Args>
		struct count_types<type_list<Args...>>
		{
			static constexpr size_t value = count_types<typename head_type<Args...>::remain>::value + 1;
		};

		template<>
		struct count_types<type_list<>>
		{
			static constexpr size_t value = 0;
		};

		template<>
		struct count_types<>
		{
			static constexpr size_t value = 0;
		};

		template<typename... Args>
		struct overturn_types
		{
			using type = typename concat_types<typename overturn_types<typename head_type<Args...>::remain>::type, typename head_type<Args...>::type>::type;
		};

		template<typename... Args>
		struct overturn_types<type_list<Args...>>
		{
			using type = typename concat_types<typename overturn_types<typename head_type<Args...>::remain>::type, typename head_type<Args...>::type>::type;
		};

		template<>
		struct overturn_types<>
		{
			using type = type_list<>;
		};

		template<>
		struct overturn_types<type_list<>>
		{
			using type = type_list<>;
		};

		template<typename... Args>
		struct tail_type
		{
			using type = typename head_type<typename overturn_types<Args...>::type>::type;
			using remain = typename overturn_types<typename head_type<typename overturn_types<Args...>::type>::remain>::type;
		};

		template<typename... Args>
		struct tail_type<type_list<Args...>>
		{
			using type = typename head_type<typename overturn_types<Args...>::type>::type;
			using remain = typename overturn_types<typename head_type<typename overturn_types<Args...>::type>::remain>::type;
		};

		template<>
		struct tail_type<>
		{
			using type = type_list<>;
			using remain = type_list<>;
		};

		template<>
		struct tail_type<type_list<>>
		{
			using type = type_list<>;
			using remain = type_list<>;
		};

		template<typename F, typename List, size_t... Idx>
			requires(!function_traits<std::decay_t<F>>::is_member_function)
		constexpr typename function_traits<std::decay_t<F>>::return_type apply(F&& f, List&& list, std::index_sequence<Idx...>)
		{
			return Trait::invoke(std::forward<F>(f), get_value<Idx>(std::forward<List>(list))...);
		}

		template<typename F, typename Class, typename List, size_t... Idx>
			requires(function_traits<std::decay_t<F>>::is_member_function)
		constexpr typename function_traits<std::decay_t<F>>::return_type apply(F&& f, Class&& c, List&& list, std::index_sequence<Idx...>)
		{
			return Trait::invoke(std::forward<F>(f), std::forward<Class>(c), get_value<Idx>(std::forward<List>(list))...);
		}

		template<size_t Idx, typename F, typename List>
		constexpr typename function_traits<std::decay_t<F>>::return_type single_of(F&& f, List&& list)
		{
			if constexpr (function_traits<std::decay_t<F>>::is_member_function)
			{
				if constexpr (is_same_template_v<typename function_traits<std::decay_t<F>>::argument_type,
								std::decay_t<decltype(get_value<Idx + 1>(std::forward<List>(list)))>>)
					return apply(std::forward<F>(f), get_value<0>(std::forward<List>(list)), get_value<Idx + 1>(std::forward<List>(list)));
				else
					return Trait::invoke(std::forward<F>(f), get_value<0>(std::forward<List>(list)), get_value<Idx + 1>(std::forward<List>(list)));
			}
			else
			{
				if constexpr (is_same_template_v<typename function_traits<std::decay_t<F>>::argument_type,
					std::decay_t<decltype(get_value<Idx>(std::forward<List>(list)))>>)
					return apply(std::forward<F>(f), get_value<Idx>(std::forward<List>(list)));
				else
					return Trait::invoke(std::forward<F>(f), get_value<Idx>(std::forward<List>(list)));
			}
		}

		template<typename F, typename List, size_t... Idx>
			requires std::is_same_v<typename function_traits<std::decay_t<F>>::return_type, void>
		constexpr void all_of(F&& f, List&& list, std::index_sequence<Idx...>)
		{
			(single_of<Idx>(std::forward<F>(f), std::forward<List>(list)), ...);
		}

		template<typename F, typename List, size_t... Idx>
			requires(!std::is_same_v<typename function_traits<std::decay_t<F>>::return_type, void>)
		constexpr std::array<typename function_traits<std::decay_t<F>>::return_type, count_types<List>::value>
			all_of(F&& f, List&& list, std::index_sequence<Idx...>)
		{
			return std::array<typename function_traits<std::decay_t<F>>::return_type, count_types<List>::value>{ single_of<Idx>(std::forward<F>(f), std::forward<List>(list))...};
		}

	}

	template<typename F, typename... Args>
	static constexpr bool is_invocable_v = Internal::is_invocable<F, Args...>::value;

	template<typename Ret, typename F, typename... Args>
	static constexpr bool is_invocable_r_v = Internal::is_invocable_r<Ret, F, Args...>::value;

	template<typename... Args>
	using head_type_t = typename Internal::head_type<Args...>::type;

	template<size_t N, typename... Args>
	using remove_head_t = typename Internal::remove_head<N, Args...>::type;

	template<size_t N, typename... Args>
	using get_type_t = typename Internal::get_type<N, Args...>::type;

	template<size_t N, typename... Args>
	using reverse_type_t = typename Internal::reverse_type<N, Args...>::type;

	template<typename... Args>
	using overturn_types_t = typename Internal::overturn_types<Args...>::type;

	template<typename... Args>
	using tail_type_t = typename Internal::tail_type<Args...>::type;

	template<typename... Args>
	static constexpr size_t count_types_v = Internal::count_types<Args...>::value;

	template<is_function_pointer F, typename... Args>
		requires(!function_traits<std::decay_t<F>>::is_member_function)
	typename function_traits<std::decay_t<F>>::return_type apply(F&& f, type_list<Args...>&& args)
	{
		return Internal::apply(std::forward<F>(f), std::forward<type_list<Args...>>(args), std::make_index_sequence<count_types_v<Args...>>{});
	}

	template<typename F, typename Arg, typename... Args>
		requires function_traits<std::decay_t<F>>::is_member_function
	typename function_traits<std::decay_t<F>>::return_type apply(F&& f, Arg&& arg, type_list<Args...>&& args)
	{
		return Internal::apply(std::forward<F>(f), std::forward<Arg>(arg), std::forward<type_list<Args...>>(args), std::make_index_sequence<count_types_v<Args...>>{});
	}

	template<size_t Idx, typename F, typename Arg, typename... Args>
	constexpr typename function_traits<std::decay_t<F>>::return_type single_of(F&& f, Arg&& arg, Args&&... args)
	{
		return Internal::single_of<Idx, F, type_list<Arg, Args...>>(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...));
	}

	template<typename F, typename Arg, typename... Args>
		requires (std::is_same_v<typename function_traits<std::decay_t<F>>::return_type, void>)
	constexpr void all_of(F&& f, Arg&& arg, Args&&... args)
	{
		if constexpr (function_traits<std::decay_t<F>>::is_member_function)
			Internal::all_of(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...), std::make_index_sequence<count_types_v<Args...>>{});
		else
			Internal::all_of(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...), std::make_index_sequence<count_types_v<Args...> + 1>{});
	}
}

template<typename Tp>
concept copyable = std::is_trivially_copyable_v<Tp> && std::is_standard_layout_v<Tp>;

template<typename Tp, typename Up>
concept memcopyable = copyable<Tp> && copyable<Up> && std::is_same_v<Tp, Up>;


template<typename T>
    requires std::is_enum_v<T>
class Enum
{
    using type = std::underlying_type_t<T>;
public:
    Enum();
    explicit Enum(const type& value);
    explicit Enum(const T& value);
    Enum(const Enum& other);
    Enum(Enum&& other) noexcept;

    // compare
    NODISCARD constexpr bool operator==(const Enum& value) const;
    NODISCARD constexpr bool operator!=(const Enum& value) const;
    NODISCARD constexpr bool operator>(const Enum& value) const;
    NODISCARD constexpr bool operator>=(const Enum& value) const;
    NODISCARD constexpr bool operator<(const Enum& value) const;
    NODISCARD constexpr bool operator<=(const Enum& value) const;

    Enum& operator=(const T& value);
    Enum& operator=(const Enum& value);

    NODISCARD constexpr bool operator==(const T& value) const;
    NODISCARD constexpr bool operator!=(const T& value) const;
    NODISCARD constexpr bool operator>(const T& value) const;
    NODISCARD constexpr bool operator>=(const T& value) const;
    NODISCARD constexpr bool operator<(const T& value) const;
    NODISCARD constexpr bool operator<=(const T& value) const;

    // transform
    NODISCARD constexpr explicit operator bool() const;
    NODISCARD constexpr explicit operator T() const;
    NODISCARD constexpr explicit operator type() const;
protected:
    type m_value;
};

template<typename T>
    requires std::is_enum_v<T>
class BitFlag : public Enum<T>
{
    using type = std::underlying_type_t<T>;
public:
    BitFlag() = default;
    BitFlag(const type& value);
    BitFlag(const T& value);
    BitFlag(const BitFlag& other);
    BitFlag(BitFlag&& other) noexcept;

    // bit op
    constexpr BitFlag& operator|=(const BitFlag& value);
    constexpr BitFlag& operator&=(const BitFlag& value);
    constexpr BitFlag& operator^=(const BitFlag& value);
    NODISCARD constexpr BitFlag operator|(const BitFlag& value) const;
    NODISCARD constexpr BitFlag operator&(const BitFlag& value) const;
    NODISCARD constexpr BitFlag operator^(const BitFlag& value) const;
    NODISCARD constexpr BitFlag operator|(const T& value) const;
    NODISCARD constexpr BitFlag operator&(const T& value) const;
    NODISCARD constexpr BitFlag operator^(const T& value) const;
    NODISCARD constexpr BitFlag operator~() const;

    BitFlag& operator=(const BitFlag& value);
    BitFlag& operator=(const T& value);
};

template<typename Tp>
NODISCARD constexpr std::underlying_type_t<Tp> to_underlying(BitFlag<Tp> val)
{
    return static_cast<std::underlying_type_t<Tp>>(val);
}

template<typename Tp>
NODISCARD constexpr std::underlying_type_t<Tp> to_underlying(Tp val)
{
    return static_cast<std::underlying_type_t<Tp>>(val);
}

#include "trait.inl"


#define FLAG_ENUM(type) \
inline type operator|(const type& l, const type& r) {return static_cast<type>(to_underlying(l) | to_underlying(r));}	\
inline type operator&(const type& l, const type& r) {return static_cast<type>(to_underlying(l) & to_underlying(r));}	\
inline type operator^(const type& l, const type& r) {return static_cast<type>(to_underlying(l) ^ to_underlying(r));}	\
inline type& operator|=(type& l, const type& r) {l = static_cast<type>(to_underlying(l) | to_underlying(r)); return l;}	\
inline type& operator&=(type& l, const type& r) {l = static_cast<type>(to_underlying(l) & to_underlying(r)); return l;}	\
inline type& operator^=(type& l, const type& r) {l = static_cast<type>(to_underlying(l) ^ to_underlying(r)); return l;}	\
inline bool operator!(const type& v) {return to_underlying(v) == 0;}		\
inline bool operator||(const type& l, const type& r) { return to_underlying(l) != 0 || to_underlying(r) != 0; }	\
inline bool operator&&(const type& l, const type& r) { return to_underlying(l) != 0 && to_underlying(r) != 0; }

#define FLAG_IDENTITY(v, flag) (v & flag) == flag

AMAZING_NAMESPACE_END