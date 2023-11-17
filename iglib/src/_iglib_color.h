#pragma once
#include "_iglib_base.h"

namespace ig
{
	template <typename _T, int _DEFAULT>
	struct BaseColorTemplate
	{
		using value_type = _T;
		using this_type = BaseColorTemplate<_T, _DEFAULT>;
		static constexpr value_type DefaultValue = value_type(_DEFAULT);

		constexpr BaseColorTemplate()
			: r{}, g{}, b{}, a{ value_type(DefaultValue) }
		{}

		constexpr BaseColorTemplate(value_type rr, value_type gg, value_type bb, value_type aa = value_type(DefaultValue))
			: r{ rr }, g{ gg }, b{ bb }, a{ aa }
		{}

		constexpr this_type inverted(const bool use_alpha = false) const
		{
			return { DefaultValue - r, DefaultValue - g, DefaultValue - b, use_alpha ? a : (DefaultValue - a) };
		}

		constexpr this_type brighten(const value_type factor, const bool use_alpha = false) const
		{
			return { DefaultValue * factor, DefaultValue * factor, DefaultValue * factor, use_alpha ? DefaultValue : (DefaultValue * factor) };
		}

		value_type r, g, b, a;
	};

	
	using Colorb = BaseColorTemplate<byte, 255>;
	using Colors = BaseColorTemplate<uint16_t, 65535>;
	using Colorf = BaseColorTemplate<float_t, 1>;
	//using Colorcomplex = BaseColorTemplate<std::complex<std::complex<std::complex<std::complex<std::complex<std::complex<double>>>>>>, -1>;

	//constexpr size_t c = sizeof(Colorcomplex);

}
