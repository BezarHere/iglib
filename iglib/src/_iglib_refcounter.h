#pragma once
#include "_iglib_base.h"

namespace ig
{
	class RefCounter
	{
		inline void _Inc();
		inline void _Dec();
	public:
		inline RefCounter()
			: m_c{ new size_t(1) }
		{
		}

		inline RefCounter(const RefCounter &copy)
			: m_c{ copy.m_c }
		{
			(void)_Inc();
		}

		inline RefCounter(RefCounter &&move) noexcept
			: m_c{ move.m_c }
		{
			move.m_c = nullptr;
		}

		inline RefCounter &operator=(const RefCounter &copy)
		{
			_Dec();
		}

		inline RefCounter &operator=(RefCounter &&move) noexcept
		{
			move.m_c = nullptr;
		}

	private:
		inline void _Inc()
		{
			if (m_c)
				(*m_c)++;
		}

		inline void _Dec()
		{
			if (m_c && *m_c)
			{
				(*m_c)--;
			}
		}


	private:
		volatile size_t *m_c;
	};
}
