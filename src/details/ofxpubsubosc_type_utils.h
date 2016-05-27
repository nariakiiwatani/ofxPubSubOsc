//
//  ofxpubsubosc_type_utils.h
//
//  Created by ISHII 2bit on 2015/06/07.
//
//

#pragma once

#include "ofxpubsubosc_settings.h"

namespace ofxpubsubosc {
    template <typename T, typename U = void>
    using is_not_ofxoscmessage = std::enable_if<!std::is_same<std::remove_reference<T>, ofxOscMessage>::value, U>;
    
    namespace {
        template <typename T>
        struct remove_const_reference {
            using type = T;
        };
        
        template <typename T>
        struct remove_const_reference<T &> : remove_const_reference<T> {};
        
        template <typename T>
        struct remove_const_reference<const T> : remove_const_reference<T> {};
        
        template <typename T>
        struct remove_const_reference<const T&> : remove_const_reference<T> {};
        
        template <typename T>
        using add_reference_if_non_arithmetic = std::conditional<std::is_arithmetic<T>::value, T, T&>;
    };
    
    template <typename T>
    using get_type = typename T::type;
    
    template <typename T>
    struct is_callable {
        template <typename U, decltype(&U::operator()) = &U::operator()>
        struct checker {};
        template <typename U> static std::true_type  test(checker<U> *);
        template <typename>   static std::false_type test(...);
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };
    
    template <typename R, typename ... Arguments>
    struct is_callable<R(*)(Arguments ...)> {
        static constexpr bool value = true;
    };
    
    template <typename R, typename ... Arguments>
    struct is_callable<R(*&)(Arguments ...)> {
        static constexpr bool value = true;
    };
    
    template <typename R, typename ... Arguments>
    struct is_callable<R(&)(Arguments ...)> {
        static constexpr bool value = true;
    };
    
    template <typename R, typename ... Arguments>
    struct is_callable<R(Arguments ...)> {
        static constexpr bool value = true;
    };
    
    template <typename R, typename ... Arguments>
    struct is_callable<std::function<R(Arguments ...)>> {
        static constexpr bool value = true;
    };
    
    template <typename T>
    struct function_info : public function_info<decltype(&T::operator())>  {};
    
    template <typename U, typename ret, typename ... arguments>
    struct function_info<ret(U::*)(arguments ...) const> {
        static constexpr bool is_function = true;
        static constexpr std::size_t arity = sizeof...(arguments);
        using result_type = ret;
        using arguments_types_tuple = std::tuple<arguments ...>;
        template <std::size_t index>
        using argument_type = get_type<std::tuple_element<index, arguments_types_tuple>>;
        using function_type = std::function<result_type(arguments ...)>;
    };
    
    template <typename U, typename ret, typename ... arguments>
    struct function_info<ret(U::*)(arguments ...)> {
        static constexpr bool is_function = true;
        static constexpr std::size_t arity = sizeof...(arguments);
        using result_type = ret;
        using arguments_types_tuple = std::tuple<arguments ...>;
        template <std::size_t index>
        using argument_type = get_type<std::tuple_element<index, arguments_types_tuple>>;
        using function_type = std::function<result_type(arguments ...)>;
    };
    
    template <typename ret, typename ... arguments>
    struct function_info<ret(*)(arguments ...)> {
        static constexpr bool is_function = true;
        static constexpr std::size_t arity = sizeof...(arguments);
        using result_type = ret;
        using arguments_types_tuple = std::tuple<arguments ...>;
        template <std::size_t index>
        using argument_type = get_type<std::tuple_element<index, arguments_types_tuple>>;
        using function_type = std::function<result_type(arguments ...)>;
    };
    
    template <typename ret, typename ... arguments>
    struct function_info<ret(arguments ...)> {
        static constexpr bool is_function = true;
        static constexpr std::size_t arity = sizeof...(arguments);
        using result_type = ret;
        using arguments_types_tuple = std::tuple<arguments ...>;
        template <std::size_t index>
        using argument_type = get_type<std::tuple_element<index, arguments_types_tuple>>;
        using function_type = std::function<result_type(arguments ...)>;
    };
    
    template <typename T>
    function_info<T> get_function_info(T &t); // for decltype
};

bool operator==(const ofMatrix3x3 &x, const ofMatrix3x3 &y) {
    return (x.a == y.a) && (x.b == y.b) && (x.c == y.c)
        && (x.d == y.d) && (x.e == y.e) && (x.f == y.f)
        && (x.g == y.g) && (x.h == y.h) && (x.i == y.i);
}

bool operator!=(const ofMatrix3x3 &x, const ofMatrix3x3 &y) {
    return !operator==(x, y);
}

bool operator==(const ofMatrix4x4 &x, const ofMatrix4x4 &y) {
    return (x._mat[0][0] == y._mat[0][0]) && (x._mat[0][1] == y._mat[0][1]) && (x._mat[0][2] == y._mat[0][2]) && (x._mat[0][3] == y._mat[0][3])
        && (x._mat[1][0] == y._mat[1][0]) && (x._mat[1][1] == y._mat[1][1]) && (x._mat[1][2] == y._mat[1][2]) && (x._mat[1][3] == y._mat[1][3])
        && (x._mat[2][0] == y._mat[2][0]) && (x._mat[2][1] == y._mat[2][1]) && (x._mat[2][2] == y._mat[2][2]) && (x._mat[2][3] == y._mat[2][3])
        && (x._mat[3][0] == y._mat[3][0]) && (x._mat[3][1] == y._mat[3][1]) && (x._mat[3][2] == y._mat[3][2]) && (x._mat[3][3] == y._mat[3][3]);
}

bool operator!=(const ofMatrix4x4 &x, const ofMatrix4x4 &y) {
    return !operator==(x, y);
}

bool operator==(const ofBuffer &x, const ofBuffer &y) {
    return (x.size() == y.size()) && (memcmp(x.getData(), y.getData(), x.size()) == 0);
}

bool operator!=(const ofBuffer &x, const ofBuffer &y) {
    return !operator==(x, y);
}

#if (OF_VERSION_MAJOR == 0) && ((OF_VERSION_MINOR < 9) || ((OF_VERSION_MINOR == 9) && (OF_VERSION_PATCH == 0)))

std::ostream &operator<<(std::ostream &os, const ofQuaternion &q) {
    os << q.x() << ", " << q.y() << ", " << q.z() << ", " << q.w();
    return os;
}

std::istream &operator>>(std::istream &is, ofQuaternion &q) {
    is >> q.x();
    is.ignore(2);
    is >> q.y();
    is.ignore(2);
    is >> q.z();
    is.ignore(2);
    is >> q.w();
    return is;
}

#endif
