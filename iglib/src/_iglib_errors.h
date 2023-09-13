#pragma once
#include "_iglib_base.h"

namespace ig
{
	enum class ErrorCode
	{
		Ok,
		Failed
	};

	struct Error
	{
		ErrorCode code{ ErrorCode::Ok };
		std::string msg{};
	};
}
