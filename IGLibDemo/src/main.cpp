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
		window.ping();
		ig::Image img = window.to_image();
		img.flip_v();
		img.save_tga("F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\image.tga");
	}

	std::cout << (int)key << '\n';
}

void draw2d_callback(Context2D c)
{
	const ig::Vector2f m = c.get_window().get_mouse_position();
	/*constexpr size_t iters = 100000;
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
	}*/


	c.demo();
	//c.quad(Vector2f(32.0f, 32.0f), Vector2f(32.0f, 32.0f + (m.y * 0.2f)), m, Vector2f(32.0f + (m.y * 0.2f), 32.0f), { 255, 44, 99, 255 });
	c.line(c.get_window().get_size() / 2, m, {255, 0, 0, 255});
	
}

int main()
{


	//std::cout << "hello there\n";
	//auto k = ig::Image("F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\image.png");
	//std::cout << "hello there\n";
	//std::cout << k.get_buffer_size() << '\n';
	//std::cout << "hello there\n";
	//std::cout << (int)k.get_channels() << '\n';
	//std::cout << "hello there\n";
	//std::cout << k.get_size() << '\n';
	//std::cout << "wadawdasdawdas" << '\n';

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
			std::this_thread::sleep_for(std::chrono::microseconds(long long(1000.0 / 60.0)));

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
		std::cerr << "EXCEPTION:" << '\n' << e.what() << std::endl;
		throw e;
	}
}
