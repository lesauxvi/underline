#pragma once
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <functional>

namespace _ {

    namespace Private {

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct rebind {
        };

        template <class NewType>
        struct rebind<QStringList, NewType> {
            typedef QList<NewType> type;
        };

        template <class NewType>
        struct rebind<QVariantList, NewType> {
            typedef QList<NewType> type;
        };

        template <class ValueType, class... Args, template <class...> class Container, class NewType>
        struct rebind<Container<ValueType, Args...>, NewType>
        {
            typedef Container<NewType, typename rebind<Args, NewType>::type...> type;
        };

        template <typename T>
        struct container_value_type {
            typedef typename std::remove_reference<T>::type::value_type type;
        };

        /// Check is the Functor be able to take Args as input. It works with generic lambda.
        template <typename Functor,typename ...Args>
        struct is_args_compatible {
            enum {
                value = std::is_convertible<Functor, std::function<void(Args...)> >::value
            };
        };

        template <typename Type, typename Functor, typename ...Args>
        using enable_if_args_compatible = typename std::enable_if<is_args_compatible<Functor, Args &&...>::value, Type>;

        template <typename Type, typename Functor, typename ...Args>
        using enable_if_args_not_compatible = typename std::enable_if<!is_args_compatible<Functor, Args &&...>::value, Type>;

        /// Convert to a function with zero paramter. Only the return type remained the same.
        template <typename Functor>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()()), Functor>::type
        decl_func0();

        template <typename Functor>
        typename enable_if_args_not_compatible<void, Functor>::type
        decl_func0();

        template <typename Functor, typename Arg1>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>())), Functor, Arg1>::type
        decl_func0();

        template <typename Functor, typename Arg1>
        typename enable_if_args_not_compatible<void, Functor, Arg1>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>(), std::declval<Arg2>())), Functor, Arg1, Arg2>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2>
        typename enable_if_args_not_compatible<void, Functor, Arg1, Arg2>::type
        decl_func0();

        template <typename Functor, typename ...Args>
        struct ret_func {
            using type = decltype(decl_func0<Functor, Args&&...>());
        };

        template <typename Functor>
        inline auto invoke(Functor functor) -> decltype(std::declval<Functor>()()) {
            return functor();
        }

        template <typename Functor, typename A1>
        inline auto invoke(Functor functor, A1) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            decltype(decl_func0<Functor>())>::type {
            return functor();
        }

        template <typename Functor, typename A1>
        inline auto invoke(Functor functor, A1 a1) ->
        typename std::enable_if<is_args_compatible<Functor,A1>::value,
        typename ret_func<Functor,A1>::type>::type {
            return functor(a1);
        }

        /* invoke(Functor,A1,A2) */

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1 a1, A2 a2) ->
            typename std::enable_if<is_args_compatible<Functor,A1,A2>::value,
            typename ret_func<Functor,A1, A2>::type>::type {
            return functor(a1,a2);
        }

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1 a1, A2) ->
            typename std::enable_if<is_args_compatible<Functor,A1>::value,
            typename ret_func<Functor,A1>::type>::type {
            return functor(a1);
        }

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1, A2) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            typename ret_func<Functor>::type>::type{
            return functor();
        }

        /// Declare a function with same output as invoke but taking zero argument.

        template <typename Functor>
        inline decltype(invoke<Functor>(std::declval<Functor>())) decl_invoke0();

        template <typename Functor, typename Arg1>
        inline decltype(invoke<Functor>(std::declval<Functor>(), std::declval<Arg1>())) decl_invoke0();

        template <typename Functor, typename Arg1, typename Arg2>
        inline decltype(invoke<Functor>(std::declval<Functor>(), std::declval<Arg1>(), std::declval<Arg2>())) decl_invoke0();

        template <typename Functor, typename ...Args>
        struct ret_invoke {
            using type = decltype(decl_invoke0<Functor, Args&&...>());
        };

    }

    template <typename T, typename P>
    inline bool some(const T& list, P predicate) {
        bool res = false;

        for (int i = 0 ; i < list.size() ; i++) {
            if (Private::invoke(predicate, list[i], i)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename T, typename F>
    inline auto map(const T& list, F callback) -> typename Private::rebind<T,
        typename Private::ret_invoke<F, typename Private::container_value_type<T>::type, int>::type
    >::type {

        typename Private::rebind<T, typename Private::ret_invoke<F, typename Private::container_value_type<T>::type, int>::type>::type res;

        for (int i = 0 ; i < list.size() ; i++) {
            res << Private::invoke(callback, list[i], i);
        }

        return res;
    }
}
