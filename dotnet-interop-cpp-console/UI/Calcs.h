#pragma once
#include <vector>
#include <SFML/System.hpp>

namespace UI
	{
	enum class Alignment { beg, mid, end };
	enum class Axis { X, Y };

	struct VAlign
		{
		inline static constexpr Alignment top   {Alignment::beg};
		inline static constexpr Alignment high  {Alignment::beg};
		inline static constexpr Alignment summit{Alignment::beg};

		inline static constexpr Alignment mid   {Alignment::mid};
		inline static constexpr Alignment middle{Alignment::mid};

		inline static constexpr Alignment bot   {Alignment::end};
		inline static constexpr Alignment low   {Alignment::end};
		inline static constexpr Alignment below {Alignment::end};
		};
	struct HAlign
		{
		inline static constexpr Alignment left  {Alignment::beg};
		inline static constexpr Alignment center{Alignment::beg};
		inline static constexpr Alignment right {Alignment::beg};
		};

	namespace calcs
		{
		enum class t { val, infinite, adapt };
		inline t operator+(const t& a, const t& b) 
			{
			if (a == t::infinite || b == t::infinite) { return t::infinite; }
			if (a == t::val      || b == t::val     ) { return t::val;      }
			else { return t::adapt; }
			}

		struct max_f
			{
			private:
				max_f(float value, t type = t::val) : value{value}, type{type} {}
			public:
				static max_f zero()                   { return max_f(0); }
				static max_f infinite()               { return max_f(0, t::infinite); }
				static max_f adaptive()               { return max_f(0, t::adapt);    }
				static max_f infval(float value)      { return max_f(value, t::infinite); }
				static max_f adaval(float value)      { return max_f(value, t::adapt);    }
				static max_f val   (float value)      { return max_f(value); }
				static max_f infval(max_f other)      { return max_f(other.value, t::infinite); }
				static max_f val   (max_f other)      { return max_f(other.value, other.type); }
				static max_f merge (max_f a, max_f b) { return max_f(std::max(a.value, b.value), a.type + b.type); }
				static max_f sum   (max_f a, max_f b) { return max_f(a.value + b.value, a.type + b.type); }

				float value;
				t type;
				bool infinity() { return type == t::infinite; }
				bool adapt()    { return type == t::adapt; }
				bool operator==(const max_f& other) { return value == other.value && type == other.type; }
			};
		struct min_f
			{
			private:
				min_f(float value) : value(value) {}
			public:
				static min_f zero()                  { return min_f(0); }
				static min_f val  (float value)      { return min_f(value); }
				static min_f val  (min_f other)      { return min_f(other.value); }
				static min_f merge(min_f a, min_f b) { return min_f(std::max(a.value, b.value)); }
				static min_f sum  (min_f a, min_f b) { return min_f(a.value + b.value); }

				float value;
			};

		struct Limits
			{
			float min;
			float max;
			t     type;
			bool infinity() { return type == t::infinite; }
			bool adapt() { return type == t::adapt; }

			Limits(float min, float max, t type) : min{min}, max{std::max(min, max)}, type{type} {}
			Limits(min_f min, max_f max) : min{min.value}, max{std::max(min.value, max.value)}, type{max.type} {}
			};
		struct PosSize
			{
			float pos;
			float size;
			PosSize(float pos, float size) :pos(pos), size(size) {}
			};

		sf::Vector2f pos(PosSize hor, PosSize ver) { return {hor.pos, ver.pos}; }
		sf::Vector2f size(PosSize hor, PosSize ver) { return {hor.size, ver.size}; }

		PosSize possize(Limits limit, float available, Alignment align)
			{
			float tmp_ava = available - limit.min;
			float tmp_max = limit.max - limit.min;
			float proportion = tmp_max == 0 ? 1 : std::min(1.f, tmp_ava / tmp_max);

			float first_pass = ((limit.max - limit.min) * proportion) + limit.min;


			float used = first_pass;
			float left = available - used;

			if (left <= 0)                         { return {0, first_pass}; }
			if (limit.infinity() || limit.adapt()) { return {0, first_pass + left}; }
			switch (align)
				{
					case Alignment::beg: return {0, first_pass};
					case Alignment::mid: return {available / 2, first_pass};
					case Alignment::end: return {available - first_pass, first_pass};
				}
			}

		std::vector<PosSize> possizes(std::vector<Limits> arr, float available)
			{
			for (int i = 0; i < arr.size(); i++) { if (arr[i].max == 0) { arr[i].max = arr[i].min; } }

			float min_sum = 0, max_sum = 0;
			for (auto element : arr) { min_sum += element.min; max_sum += element.max; }

			float tmp_ava = available - min_sum;
			float tmp_max = max_sum - min_sum;
			float proportion = tmp_max == 0 ? 1 : std::min(1.f, tmp_ava / tmp_max);

			float for_each_infinite = 0;

			std::vector<float> first_pass; first_pass.reserve(arr.size());

			for (const auto& e : arr) { first_pass.push_back(((e.max - e.min) * proportion) + e.min); }

			float used = 0;
			for (float f : first_pass) { used += f; }

			float left = available - used;

			int infinite_count = 0;
			for (auto e : arr) { if (e.infinity() || e.adapt()) { infinite_count++; } }

			for_each_infinite = left / infinite_count;

			float current = 0;

			std::vector<PosSize> ret; ret.reserve(arr.size());
			for (int i = 0; i < arr.size(); i++)
				{
				float s = ((arr[i].max - arr[i].min) * proportion) + arr[i].min + (arr[i].infinity() || arr[i].adapt() ? for_each_infinite : 0);
				ret.emplace_back(current, s);
				current += s;
				}
			return ret;
			}
		}

	}