#include <type_traits>
#include <cstddef>

template <typename... Ts>
struct TypeList {};

namespace detail {

    template <typename T>
    struct AlwaysFalse : std::false_type {};

    template <std::size_t Index, typename List>
    struct GetImpl;

    template <typename Head, typename... Tail>
    struct GetImpl<0, TypeList<Head, Tail...>> {
        using type = Head;
    };

    template <std::size_t Index, typename Head, typename... Tail>
    struct GetImpl<Index, TypeList<Head, Tail...>> {
        using type = typename GetImpl<Index - 1, TypeList<Tail...>>::type;
    };

    template <std::size_t Index>
    struct GetImpl<Index, TypeList<>> {
        static_assert(AlwaysFalse<std::integral_constant<std::size_t, Index>>::value,
            "Index is out of bounds for TypeList");
    };

    template <typename List>
    struct SizeImpl;

    template <typename... Ts>
    struct SizeImpl<TypeList<Ts...>> {
        static constexpr std::size_t value = sizeof...(Ts);
    };

    template <typename Target, typename List>
    struct ContainsImpl;

    template <typename Target>
    struct ContainsImpl<Target, TypeList<>> {
        static constexpr bool value = false;
    };

    template <typename Target, typename Head, typename... Tail>
    struct ContainsImpl<Target, TypeList<Head, Tail...>> {
        static constexpr bool value = std::is_same_v<Target, Head> || ContainsImpl<Target, TypeList<Tail...>>::value;
    };

    template <typename Target, typename List>
    struct IndexOfImpl;

    template <typename Target>
    struct IndexOfImpl<Target, TypeList<>> {
        static_assert(AlwaysFalse<Target>::value, "Type not found in TypeList");
    };

    template <bool Found, typename Target, typename List>
    struct IndexOfHelper;

    template <typename Target, typename Head, typename... Tail>
    struct IndexOfHelper<true, Target, TypeList<Head, Tail...>> {
        static constexpr std::size_t value = 0;
    };

    template <typename Target, typename Head, typename... Tail>
    struct IndexOfHelper<false, Target, TypeList<Head, Tail...>> {
        static constexpr std::size_t value = 1 + IndexOfImpl<Target, TypeList<Tail...>>::value;
    };

    template <typename Target, typename Head, typename... Tail>
    struct IndexOfImpl<Target, TypeList<Head, Tail...>> {
        static constexpr std::size_t value = IndexOfHelper<
            std::is_same_v<Target, Head>, 
            Target,
            TypeList<Head, Tail...>
        >::value;
    };

    template <typename List, typename NewType>
    struct AppendImpl;

    template <typename... Ts, typename NewType>
    struct AppendImpl<TypeList<Ts...>, NewType> {
        using type = TypeList<Ts..., NewType>;
    };

    template <typename List, typename NewType>
    struct PrependImpl;

    template <typename... Ts, typename NewType>
    struct PrependImpl<TypeList<Ts...>, NewType> {
        using type = TypeList<NewType, Ts...>;
    };

}


template <std::size_t Index, typename List>
using Get_t = typename detail::GetImpl<Index, List>::type;

template <typename List>
constexpr std::size_t Size_v = detail::SizeImpl<List>::value;

template <typename Target, typename List>
constexpr bool Contains_v = detail::ContainsImpl<Target, List>::value;

template <typename Target, typename List>
constexpr std::size_t IndexOf_v = detail::IndexOfImpl<Target, List>::value;

template <typename List, typename NewType>
using Append_t = typename detail::AppendImpl<List, NewType>::type;

template <typename List, typename NewType>
using Prepend_t = typename detail::PrependImpl<List, NewType>::type;


int main() {
    using MyList = TypeList<int, float, double>;

    // Получение размера
    static_assert(Size_v<MyList> == 3, "Size should be 3");
    static_assert(Size_v<TypeList<>> == 0, "Empty list size should be 0");

    // Получение элемента по индексу
    static_assert(std::is_same_v<Get_t<0, MyList>, int>, "Index 0 should be int");
    static_assert(std::is_same_v<Get_t<1, MyList>, float>, "Index 1 should be float");
    static_assert(std::is_same_v<Get_t<2, MyList>, double>, "Index 2 should be double");

    // Проверка наличия типа
    static_assert(Contains_v<float, MyList> == true, "List contains float");
    static_assert(Contains_v<char, MyList> == false, "List does not contain char");

    // Получение индекса типа
    static_assert(IndexOf_v<int, MyList> == 0, "Index of int is 0");
    static_assert(IndexOf_v<float, MyList> == 1, "Index of float is 1");

    // Добавление в конец
    using AppendedList = Append_t<MyList, char>;
    static_assert(std::is_same_v<AppendedList, TypeList<int, float, double, char>>, "Append failed");
    static_assert(Size_v<AppendedList> == 4, "Appended list size should be 4");

    // Добавление в начало
    using PrependedList = Prepend_t<MyList, char>;
    static_assert(std::is_same_v<PrependedList, TypeList<char, int, float, double>>, "Prepend failed");

    return 0;
}
