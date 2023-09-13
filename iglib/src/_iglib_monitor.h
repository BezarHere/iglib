#pragma once
#include "_iglib_base.h"
#include "_iglib_rect.h"

namespace ig
{

	typedef void *MonitorData_t;

	class Monitor
	{
	public:
		Monitor(MonitorData_t data);

		static MonitorData_t primary_monitor_id();
		static std::vector<MonitorData_t> all_monitors_ids();

		// refreshes the stats (e.g. resolution/refresh_rate)
		void refresh();

	private:
		MonitorData_t m_data;
		Vector2i m_resolution{};
		int m_refresh_rate{};
		std::array<int, 3> m_bit_depth{};
		std::string m_name{};
	};

}
