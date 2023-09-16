#include "pch.h"
#include "_iglib_monitor.h"
#include "internal.h"


namespace ig
{

	std::vector<Monitor> get_screens()
	{
		
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		std::vector<Monitor> screens{};
		return screens;
	}


	Monitor::Monitor(MonitorData_t data)
		: m_data{ data }
	{
		refresh();
	}

	MonitorData_t Monitor::primary_monitor_id()
	{
		return (MonitorData_t)glfwGetPrimaryMonitor();
	}

	std::vector<MonitorData_t> Monitor::all_monitors_ids()
	{
		std::vector<MonitorData_t> vc;
		const std::vector<MonitorHandle_t> &pc{ get_monitors() };
		vc.resize(pc.size());
		for (MonitorHandle_t i : pc)
			vc.push_back(i);
		return vc;
	}

	void Monitor::refresh()
	{
		const GLFWvidmode *vd = glfwGetVideoMode((MonitorHandle_t)m_data);
		if (vd == nullptr)
		{
			glfwerror();
			return;
		}
		m_resolution.x = vd->width;
		m_resolution.y = vd->height;
		m_refresh_rate = vd->refreshRate;
		m_bit_depth[ 0 ] = vd->redBits;
		m_bit_depth[ 1 ] = vd->greenBits;
		m_bit_depth[ 2 ] = vd->blueBits;

		const char *cstr = glfwGetMonitorName((MonitorHandle_t)m_data);
		if (cstr != nullptr)
			m_name = cstr;
		else
			m_name = "";
	}

}
