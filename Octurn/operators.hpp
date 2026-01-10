#pragma once
#include "log/logHandler.hpp"
#include <vector>
#include <any>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <typeinfo>
#include <format>
#include <stdexcept>
#include <variant>
#include "types.hpp"

using Octurn::AnyValue;


// ============================================================================================ //
//                                   Type traits
//       ** std::vector<bool>.operator[] gives a proxy type of one bit, not a bool **
//       ** Used to bypass throw on bool types **
// ============================================================================================ //

template<typename T>
struct is_bool_like {
    static constexpr bool value =
        std::is_same_v<T, bool> ||
        std::is_same_v<T, std::vector<bool>::reference> ||
        std::is_same_v<T, std::vector<bool>::const_reference>;
};

// ============================================================================================ //
//                                   Broadcast to vector
//                     ** Allows to work with scalars as vectors **
// ============================================================================================ //

template <typename T>
const std::vector<T>& to_vector(const std::vector<T>& value) {
    return value;
}

template <typename T>
std::vector<T> to_vector(const T& value) {
    return std::vector<T>{value};
}

// ============================================================================================ //
//                                   Calculate vectors
// Identify input types -> return result if operands are appropriate
// To better manage different types broadcast to vector is chosen -> possible to compare (vector) and (scalar)
// ** Since scalar is broadcasted to vector and functors automatically decide on return type **
// ** If operands aren't comparable -> throw runtime error **
// ============================================================================================ //


template<typename A, typename B, typename Op>
AnyValue vector_op(const A& lhs,const B& rhs,Op op){

    const auto& L = to_vector(lhs);
    const auto& R = to_vector(rhs);

    using ResultType = decltype(op(L[0], R[0]));

    std::vector<ResultType> result;
    size_t size = std::max(L.size(),R.size());
    result.reserve(size);

    for (size_t i = 0; i < size; ++i){
        result.push_back(op(L[i%L.size()], R[i%R.size()]));
    }

    return AnyValue{std::move(result)};
}

// ========================================================== //
//                    Functor operators
// Functor -> class as a function with operator overloading
// ========================================================== //

struct OpPlus {
    template<typename L,typename R>
    double operator()(const L& a,const R& b) const {

        if constexpr (!(std::is_same_v<L, double> && std::is_same_v<R, double>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of summing operator on type {}", typeid(a).name())
            );
        } else {
            return a + b;
        }
    };
};

struct OpMinus {
    template<typename L,typename R>
    double operator()(const L& a, const R& b) const {

        if constexpr (!(std::is_same_v<L, double> && std::is_same_v<R, double>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of subtraction operator on type {}", typeid(a).name())
            );
        } else {
            return a - b;
        }
    };
};

struct OpMultiply {
    template<typename L,typename R>
    double operator()(const L& a, const R& b) const { 
        
        g_logger.report("************MULTIPLY CALLLED**************");

        if constexpr (!(std::is_same_v<L, double> && std::is_same_v<R, double>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of multiplication operator on type {}", typeid(a).name())
            );
        } else {
            std::cout<<"Multiplication result: ";
            std::cout<<a*b<<"\n";
            
            return a * b;
        }
        
    };
};

struct OpDivide {
    template<typename L,typename R>
    double operator()(const L& a, const R& b) const { 
        

        if constexpr (!(std::is_same_v<L, double> && std::is_same_v<R, double>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of division operator on type {}", typeid(a).name())
            );
        } else {
            return a / b;
        }

    };
};

struct OpGreater {
    template<typename L,typename R>
    bool operator()(const L& a, const R& b) const { 

        if constexpr (!(std::is_same_v<L, double> && std::is_same_v<R, double>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of \">\" operator on type {}", typeid(a).name())
            );
        } else {
            return a > b;
        }
    };
};

struct OpLess {
    template<typename L,typename R>
    bool operator()(const L& a, const R& b) const { 

        if constexpr (!(std::is_same_v<L, double> && std::is_same_v<R, double>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of \"<\" operator on type {}", typeid(a).name())
            );
        } else {
            return a < b;
        }
        
    };
};

struct OpEqual {
    template<typename L,typename R>
    bool operator()(const L& a, const R& b) const {
        if constexpr(std::is_same_v<L, double> && std::is_same_v<R, double> || (is_bool_like<L>::value && is_bool_like<R>::value)){
            return a == b;
        } else {
            throw std::runtime_error(
                std::format("Incorrect usage of equality operator on type {}", typeid(a).name())
            );
        }
    }
};


struct OpAnd {
    template<typename L,typename R>
    bool operator()(const L& a, const R& b) const {

        if constexpr (!(std::is_convertible_v<L, bool> && std::is_convertible_v<R, bool>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of \"and\" operator on type {}", typeid(a).name())
            );
        } else {
            return static_cast<bool>(a) && static_cast<bool>(b);
        }
    }
};


struct OpOr {
    template<typename L,typename R>
    bool operator()(const L& a, const R& b) const { 

        using Lhs = std::decay_t<decltype(a)>;
        using Rhs = std::decay_t<decltype(b)>;

        if constexpr (!(std::is_convertible_v<L, bool> && std::is_convertible_v<R, bool>)) {
            throw std::runtime_error(
                std::format("Incorrect usage of \"or\" operator on type {}", typeid(a).name())
            );
        } else {
            return static_cast<bool>(a) || static_cast<bool>(b);
        }
    
    };
};


using OperatorVariant = std::variant<
    OpPlus,
    OpMinus,
    OpMultiply,
    OpDivide,
    OpGreater,
    OpLess,
    OpEqual,
    OpAnd,
    OpOr >;

inline std::unordered_map<std::string, OperatorVariant> OperatorMap {
    {"+", OpPlus{}},
    {"-", OpMinus{}},
    {"*", OpMultiply{}},
    {"/", OpDivide{}},
    {">", OpGreater{}},
    {"<", OpLess{}},
    {"==", OpEqual{}},
    {"and", OpAnd{}},
    {"or", OpOr{}}
};
