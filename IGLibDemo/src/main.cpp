#include <iglib.h>
#include <iostream>
#include <string>
#include <thread>
using namespace ig;

void callback(ig::Window &window, ig::WindowCallbackReason reason)
{
	if (reason == ig::WindowCallbackReason::Unfocused)
		window.ping();

	std::cout << window.get_title() << ": " << (int)reason << '\n';
}

int main()
{
	try
	{
		ig::ApplicationConfig appcfg{};
		appcfg.fullscreen = false;
		ig::Application app{ appcfg };
		ig::Window &i = app.get_primary_window();

		//app.create_window(128, 128, "Other one", false).set_callback(callback);

		i.set_callback(callback);

		while (!i)
		{
			//std::cout << i.size() << ' ' << i.position() << '\n';

			std::this_thread::sleep_for(std::chrono::microseconds(long long(1000.0 / 30.0)));

			i.clear();
			i.render();
			i.poll();
		}

	}
	catch (const std::exception &e)
	{
		std::cerr << "EXCEPTION:" << '\n' << e.what() << '\n';
		throw e;
	}
}
