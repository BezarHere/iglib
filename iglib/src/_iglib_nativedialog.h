#pragma once
#include "_iglib_base.h"
#include <winapifamily.h>
#include <Windows.h>
#include <wincon.h>

namespace ig
{
	namespace native_dialog
	{
		class NativeDialog
		{
#ifdef _WIN32
		public:
			inline NativeDialog(const HWND hwnd)
				: m_hwnd{ hwnd }
			{}

		private:
			HWND m_hwnd;
#endif // _WIN32
		};

		class OpenFileDialog : public NativeDialog
		{
		public:


		private:

		};
	}
}
