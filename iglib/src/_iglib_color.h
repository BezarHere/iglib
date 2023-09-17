#pragma once
#include "_iglib_base.h"

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
	using Colorf = BaseColorTemplate<float, 1>;

}
