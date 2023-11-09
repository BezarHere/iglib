#include "pch.h"
#include "_iglib_wavefront_obj.h"

struct string_range
{
	FORCEINLINE constexpr string_range()
		: begin{ std::string::npos }, end{ std::string::npos }
	{
	}

	FORCEINLINE constexpr string_range(size_t b, size_t e)
		: begin{ b }, end{ e }
	{
	}

	size_t begin, end;
};

FORCEINLINE span<string_range> get_lines(const std::string &src, bool keep_empty)
{
	std::vector<string_range> strs{};
	strs.reserve(128);
	size_t i = 0;
	const size_t sz = src.size();
	while (i != std::string::npos && i < sz)
	{
		const size_t j = src.find('\n', i);

		if (j != i || (j == i && keep_empty))
			strs.emplace_back(i, j == std::string::npos ? sz : j);

		if (j == std::string::npos)
			break;
		i = j + 1;
	}
	return span<string_range>{ strs.data(), strs.size() };
}

namespace ig
{
	namespace wavefront
	{
		Obj::Obj(const std::string &src)
		{
			const auto lines = get_lines(src, false);

			/*for (const auto [begin, end] : lines)
			{
				


			}*/

		}
	}
}
