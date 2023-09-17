#include <iglib.h>
#include <iostream>
#include <string>
#include <thread>
#include <intrin.h>

#include <winsock.h>

using namespace ig;

struct LARGE
{
	LARGE()
	{

	}
	__m256i a[ 8 ]{};
};

void a(LARGE a, LARGE b, LARGE c, LARGE d)
{
	LARGE *aptr = &a;
	LARGE *bptr = &b;
	LARGE *cptr = &c;
	LARGE *dptr = &d;

	size_t asz = size_t(aptr);
	size_t bsz = size_t(bptr);
	size_t csz = size_t(cptr);
	size_t dsz = size_t(dptr);

	size_t adif = bsz - asz;
	size_t ddif = dsz - asz;

	std::cout << aptr << bptr << cptr << dptr << '\n';
}

void callback(ig::Window &window, ig::WindowCallbackReason reason)
{
	if (reason == ig::WindowCallbackReason::Unfocused)
		window.ping();

	//std::cout << window.get_title() << ": " << (int)reason << '\n';
}

void key_callback(ig::Window &window, ig::Key key, ig::KeyAction action, ig::KeyModFlags mods)
{
	if (key == ig::Key_W)
	{
	}

	std::cout << (int)key << '\n';
}

void draw2d_callback(Context2D &c)
{
	c.demo();
}

int main()
{
	//a(LARGE{}, LARGE{}, LARGE{}, LARGE{});
	try
	{
		ig::ApplicationConfig appcfg{};
		appcfg.fullscreen = false;
		ig::Application app{ appcfg };
		ig::Window &i = app.get_primary_window();


		ig::Window &p = app.create_window(128, 128, "Other one", false);
		
		// NOTICE: Hiding window for later fixes
		p.hide();

		p.set_callback(callback);

		i.set_callback(callback);
		i.set_key_callback(key_callback);
		i.set_draw2d_callback(draw2d_callback);

		while (!i)
		{
			//std::cout << i.size() << ' ' << i.position() << '\n';

			std::this_thread::sleep_for(std::chrono::microseconds(long long(1000.0 / 30.0)));

			//std::cout << "mouse pos: " << i.get_mouse_position() << '\n';

			i.clear();
			
			i.get_2d_context().demo();
			i.get_2d_context().draw_rect(Vector2f(), Vector2f(i.get_mouse_position()), { 255, 255, 155, 255 });

			i.render();
			i.poll();

			p.clear();
			
			p.get_2d_context().demo();
			
			p.render();
			p.poll();
		}

	}
	catch (const std::exception &e)
	{
		std::cerr << "EXCEPTION:" << '\n' << e.what() << '\n';
		throw e;
	}
}
