#pragma once

#include <optional>
#ifndef _DUST_CORE_TYPES_HPP_
#define _DUST_CORE_TYPES_HPP_

#include <cstdint>
#include <string>
#include <memory>
#include <utility>

// Standard type aliases

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using f32 = float;
using f64 = double;

// using string = std::string;

// Smart Pointers

namespace dust {

/** @brief Shared ptr */
template <typename T>
using Ref = std::shared_ptr<T>;

/**
 * @brief Create a Ref
 *
 * @tparam RefType Ref template type
 * @tparam Args Arguments types
 * @param args arguments for the creation of the object
 * @return constexpr Ref<RefType> 
 */
template <typename RefType, typename ...Args>
[[nodiscard]]
inline static constexpr Ref<RefType> createRef(Args&& ...args)
{
    return std::make_shared<RefType>(std::forward<Args>(args)...);
}

/** @brief Unique ptr */
template <typename T>
using Scope = std::unique_ptr<T>;

/**
 * @brief Create a Scope
 *
 * @tparam ScopeType Scope template type
 * @tparam Args Arguments types
 * @param args arguments for the creation of the object
 * @return constexpr Scope<ScopeType> 
 */
template <typename ScopeType, typename ...Args>
[[nodiscard]]
inline static constexpr Scope<ScopeType> createScope(Args&& ...args)
{
    return std::make_unique<ScopeType>(std::forward<Args>(args)...);
}

/** @brief Weak ptr */
template <typename T>
using Weak = std::weak_ptr<T>;


/**
 * @brief Result (std::optionnal)
 * @tparam ResultType Containing result type
 */
template <typename ResultType>
using Result = std::optional<ResultType>;


// /**
//  * @brief Create a Result
//  *
//  * @tparam ResultType Result template type
//  * @tparam Args Arguments types
//  * @param args arguments for the creation of the object
//  * @return constexpr Result<ResultType> 
//  */
// template <typename ResultType, typename ...Args>
// [[nodiscard]]
// inline static constexpr Result<ResultType> createResult(Args&& ...args)
// {
//     return std::make_optional<ResultType>(std::forward<Args>(args)...);
// }

}

#endif //_DUST_CORE_TYPES_HPP_