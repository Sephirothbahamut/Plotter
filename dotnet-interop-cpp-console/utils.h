#pragma once
#include <math.h>
#include <vector>
#include <string>
#include <type_traits>

template <typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b)
	{
	a.reserve(a.size() + b.size());
	a.insert(a.end(), b.begin(), b.end());
	return a;
	}

namespace utils
	{

	template<typename T>
	int get_digit(T n, long digit)
		{
		if constexpr (std::is_integral_v<T>)
			{
			while (digit--) { n /= T{10}; } return (n % T{10});
			}
		else if constexpr (std::is_floating_point_v<T>)
			{
			if (digit >= 0)
				{
				while (digit--) { n /= T{10}; } return (static_cast<int>(n) % 10);
				}
			else
				{
				while (digit++) { n *= T{10}; } return (static_cast<int>(n) % 10);
				}
			}
		}

	template<typename T>
	char get_digit_c(T n, long digit) { return char{'0' + get_digit(n, digit)}; }

	inline std::string to_string(float n) { return std::to_string(n); }

	template<typename T>
	std::string to_string(T n, long amount_before)
		{
		if (amount_before == 0) { return ""; }
		std::string string; string.resize(amount_before);

		for (long digit = amount_before - 1, index = 0; digit >= 0; digit--, index++) { string[index] = '0' + get_digit(n, digit); }

		size_t clear_index = string.find_first_not_of('0');
		if (clear_index == std::string::npos) { clear_index = string.size(); }
		if (clear_index != std::string::npos)
			{
			for (size_t i = 0; i < clear_index; i++) { string[i] = ' '; }
			if (clear_index == amount_before) { string[clear_index - 1] = '0'; }
			}

		return string;
		}

	template<class T, typename = std::enable_if_t<std::is_floating_point_v<T>> >
	std::string to_string(T n, long amount_before_comma, long amount_after_comma)//TODO check if can replace int with size_t
		{
		auto abc = amount_before_comma, aac = amount_after_comma;
		if (aac == 0) { return to_string(n, abc); }

		std::string string;
		if (abc == 0) { string = "."; string.resize(1 + aac); }
		else { string = to_string(n, abc); string.resize(abc + 1 + aac); string[abc] = '.'; }

		for (long digit = 0, index = 1; digit < aac; digit++, index++) { string[abc + index] = '0' + get_digit(n, -1 - digit); }

		return string;
		}

	inline std::vector<std::string> split(const std::string& str, char delim)
		{
		std::vector<std::string> strings;
		size_t start;
		size_t end = 0;
		while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
			{
			end = str.find(delim, start);
			strings.push_back(str.substr(start, end - start));
			}
		return strings;
		}

	inline float percent_in_range(float value, float lower, float upper) { return (value - lower) / (upper - lower); }

	template <typename T>
	class Buffer
		{
		private:
			std::vector<T> arr;

		public:
			size_t size() { return arr.size(); }

			Buffer(size_t size = 0) : arr(size) {}

			T get_last() { return arr.back(); }
			void set_last(T value) { arr.back() = value; }

			__declspec(property(get = get_last, put = set_last)) T last;

			T& operator[](size_t i) { return arr[i]; }

			void push(T value)
				{
				if (size() != 0)
					{
					for (size_t i = 0; i < size() - 1; i++) { arr[i] = arr[i + 1]; }
					last = value;
					}
				}

			void resize(size_t new_size)
				{
				if (new_size == size()) { return; }
				std::vector<T> new_arr(new_size);

				if (new_size > size())
					{
					for (size_t i = 0; i < new_size - size(); i++) { new_arr[i] = 0.f; }
					for (size_t i = 0; i < size(); i++) { new_arr[new_size - size() + i] = arr[i]; }
					}
				else //if(new_size < size())
					{
					for (size_t i = size() - new_size, j = 0; i < size(); i++, j++) { new_arr[j] = arr[i]; }
					}
				arr.swap(new_arr);
				}
		};
	}