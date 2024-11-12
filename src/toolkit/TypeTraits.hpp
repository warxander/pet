#pragma once

#include <memory>
#include <string>
#include <type_traits>

namespace pet
{
	template <typename T>
	struct IsCStyleString
		: public std::bool_constant<(std::is_pointer_v<T> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, char>) ||
									(std::is_array_v<T> && std::is_same_v<std::remove_cv_t<std::remove_extent_t<T>>, char>)>
	{
	};

	template <typename T>
	inline constexpr bool IsCStyleStringValue = IsCStyleString<T>::value;

	template <typename T>
	struct IsCharArray : public std::bool_constant<std::is_array_v<T> && std::is_same_v<std::remove_cv_t<std::remove_extent_t<T>>, char>>
	{
	};

	template <typename T>
	inline constexpr bool IsCharArrayValue = IsCharArray<T>::value;

	template <typename T>
	inline constexpr bool IsStringLike =
		std::is_same_v<std::remove_cv_t<T>, std::string> || std::is_same_v<std::remove_cv_t<T>, std::string_view> ||
		IsCStyleStringValue<T> || IsCharArrayValue<T>;

	template <typename T>
	struct IsSmartPointer : public std::false_type
	{
	};

	template <typename T>
	struct IsSmartPointer<std::shared_ptr<T>> : public std::true_type
	{
	};

	template <typename T>
	struct IsSmartPointer<std::shared_ptr<const T>> : public std::true_type
	{
	};

	template <typename T>
	struct IsSmartPointer<std::unique_ptr<T>> : public std::true_type
	{
	};

	template <typename T>
	struct IsSmartPointer<std::unique_ptr<const T>> : public std::true_type
	{
	};

	template <typename T>
	struct IsSmartPointer<std::weak_ptr<T>> : public std::true_type
	{
	};

	template <typename T>
	struct IsSmartPointer<std::weak_ptr<const T>> : public std::true_type
	{
	};

	template <typename T>
	inline constexpr bool IsSmartPointerValue = IsSmartPointer<T>::value;

	template <typename T, class = void>
	inline constexpr bool HasToStringMethod = false;

	template <typename T>
	inline constexpr bool HasToStringMethod<T, std::void_t<decltype(std::declval<T>().ToString())>> =
		std::is_convertible_v<decltype(std::declval<T>().ToString()), std::string>;

	template <typename T, class = void>
	inline constexpr bool IsIterable = false;

	template <typename T>
	inline constexpr bool IsIterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> = true;
}
