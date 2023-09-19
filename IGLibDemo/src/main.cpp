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
	const ig::Vector2f m = c.get_window().get_mouse_position();
	constexpr size_t iters = 100000;
	for (size_t i{}; i < iters; i++)
	{
		const float fi(i + 1);
		c.quad(
			Vector2f(),
			Vector2f(0.0f, iters - i),
			Vector2f(iters - i, iters - i),
			Vector2f(iters - i, 0.0f),
			{ 45, ig::byte(i  & 0xff), 188, 255 }
		);
	}

	/*c.demo();
	c.rect(Vector2f(32.0f, 32.0f), c.get_window().get_mouse_position(), {255, 44, 99, 255});
	c.line(Vector2f(), c.get_window().get_mouse_position(), { 255, 0, 0, 255 });*/
}

int main()
{
	float b[]{ 1.0f, 4.0f, 9.0f, 12.0f };
	float c[]{ -4.1f, 2.2f, 53.2f, 0.77f };

	__m128 mm128{ _mm_div_ps(_mm_load1_ps((float *)&b), _mm_load1_ps((float *)&c)) };
	
	

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
		p.set_draw2d_callback(draw2d_callback);

		while (!i.should_close())
		{
			//std::cout << i.size() << ' ' << i.position() << '\n';

			std::this_thread::sleep_for(std::chrono::microseconds(long long(1000.0 / 30.0)));

			//std::cout << "mouse pos: " << i.get_mouse_position() << '\n';

			i.clear();
			i.render();
			i.poll();

			//p.clear();
			//p.render();
			//p.poll();
		}

	}
	catch (const std::exception &e)
	{
		std::cerr << "EXCEPTION:" << '\n' << e.what() << '\n';
		throw e;
	}
}
