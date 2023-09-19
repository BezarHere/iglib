#pragma once
#include "_iglib_base.h"
#include <complex>

namespace ig
{
	template <typename _T, int _ALPHA_DEFAULT>
	struct BaseColorTemplate
	{
		using value_type = _T;
		using this_type = BaseColorTemplate<_T, _ALPHA_DEFAULT>;

		BaseColorTemplate()
			: r{}, g{}, b{}, a{ value_type(_ALPHA_DEFAULT) }
		{}

		BaseColorTemplate(value_type rr, value_type gg, value_type bb, value_type aa = value_type(_ALPHA_DEFAULT))
			: r{ rr }, g{ gg }, b{ bb }, a{ aa }
		{}

		value_type r, g, b, a;
	};

	
	using Colorb = BaseColorTemplate<byte, 255>;
	using Colors = BaseColorTemplate<uint16_t, 65535>;
	using Colorf = BaseColorTemplate<float_t, 1>;
	//using Colorcomplex = BaseColorTemplate<std::complex<std::complex<std::complex<std::complex<std::complex<std::complex<double>>>>>>, -1>;

	//constexpr size_t c = sizeof(Colorcomplex);

}
