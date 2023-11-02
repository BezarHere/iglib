#include "pch.h"
#include "_iglib_wavefront_obj.h"

FORCEINLINE span<std::string> get_lines(const std::string &src)
{
	std::vector<std::string> strs{};
	strs.reserve(128);
	size_t i = 0;
	const size_t sz = src.size();
	while (i != std::string::npos)
	{
		const size_t j = src.find('\n', i);
		strs.emplace_back(src.data() + i, (j == std::string::npos ? sz : j) - i);
		i = j;
	}
	return span<std::string>{ strs.data(), strs.size() };
}

namespace ig
{
	namespace wavefront
	{
		Obj::Obj(const std::string &src)
		{
		}
	}
}
