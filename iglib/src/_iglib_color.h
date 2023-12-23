#pragma once
#include "_iglib_base.h"

namespace ig
{
	template <typename _T, int _DEFAULT>
	struct BaseColorTemplate
	{
		using value_type = _T;
		using this_type = BaseColorTemplate<_T, _DEFAULT>;
		static constexpr value_type full_value = value_type(_DEFAULT);

		constexpr BaseColorTemplate()
			: r{}, g{}, b{}, a{ value_type(full_value) }
		{}

		constexpr BaseColorTemplate(value_type rr, value_type gg, value_type bb, value_type aa = value_type(full_value))
			: r{ rr }, g{ gg }, b{ bb }, a{ aa }
		{}

		constexpr this_type inverted(const bool use_alpha = false) const
		{
			return { full_value - r, full_value - g, full_value - b, use_alpha ? a : (full_value - a) };
		}

		constexpr this_type brighten(const value_type factor, const bool use_alpha = false) const
		{
			return { full_value * factor, full_value * factor, full_value * factor, use_alpha ? full_value : (full_value * factor) };
		}

		value_type r, g, b, a;
	};


	using Colorb = BaseColorTemplate<byte, 255>;
	using Color16 = BaseColorTemplate<uint16_t, 65535>;
	using Colorf = BaseColorTemplate<float_t, 1>;

	//using Colorcomplex = BaseColorTemplate<std::complex<std::complex<std::complex<std::complex<std::complex<std::complex<double>>>>>>, -1>;

	//constexpr size_t c = sizeof(Colorcomplex);

}
