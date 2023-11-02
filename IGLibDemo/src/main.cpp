#include <iglib.h>
#include <iostream>
#include <string>
#include <thread>
#include <intrin.h>
#include <istream>
#include <fstream>

#include <winsock.h>

using namespace ig;

const std::string current_dir = "F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\";

ig::Transform2D tr{};

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

std::string readall(const std::string &fs)
{
	FILE *f;
	auto err = fopen_s(&f, fs.c_str(), "r");
	
	if (f == NULL)
		return "invalid file";

	const size_t begin = ftell(f);
	fseek(f, 0, FILE_END);
	const size_t size = ftell(f) - begin;
	fseek(f, begin, FILE_BEGIN);
	char *buf = new char[ size ];
	fread(buf, 1, size, f);
	std::string st{ buf, size };
	delete[] buf;
	fclose(f);
	return st;
}

Vector2f mouse_pos_when_space = { 0.f, 0.f };
void key_callback(ig::Window &window, ig::Key key, ig::KeyAction action, ig::KeyModFlags mods)
{
	if (key == ig::Key_W)
	{
		tr.rotate(0.1);
		window.ping();
		/*ig::Image img = window.to_image();
		img.flip_v();
		img.save_tga("F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\image.tga");*/
	}

	if (key == ig::Key_S)
	{
		tr.rotate(-0.1);
	}


	if (key == ig::Key_Space)
	{
		mouse_pos_when_space = window.get_mouse_position();
		tr.set_scale(tr.get_scale() + Vector2f{ 0.1f, 0.1f });
	}


	if (key == ig::Key_LeftControl)
	{
		mouse_pos_when_space = window.get_mouse_position();
		tr.set_scale(tr.get_scale() - Vector2f{ 0.1f, 0.1f });
	}

	std::cout << (int)key << '\n';
}

void draw2d_callback(Canvas c)
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

	//ig::Shader ss{};
	tr.set_position(m);
	c.transform2d() = tr;
	c.demo();
	//c.quad(Vector2f(32.0f, 32.0f), Vector2f(32.0f, 32.0f + (m.y * 0.2f)), m, Vector2f(32.0f + (m.y * 0.2f), 32.0f), { 255, 44, 99, 255 });

	//c.rect(mouse_pos_when_space, c.get_window().get_mouse_position(), { 255, 255, 200, 255 });

	//c.bind_shader(ss);
	//c.line(c.get_window().get_size() / 2, m, {255, 0, 0, 255});
	
}


int main()
{
	ig::wavefront::Obj o{ "v 12 44 41\nv 54 65 11\n\nvp 12" };

	std::cout << readall(current_dir + "\\main.cpp") << '\n';

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
	
	{
		ig::Window i = ig::Window({128, 128}, "Window !!!");


		ig::Window p = ig::Window({ 128, 128 }, "Other one");
		
		// NOTICE: Hiding window for later fixes
		p.hide();

		p.set_callback(callback);

		i.set_callback(callback);
		i.set_key_callback(key_callback);
		i.set_draw_callback(draw2d_callback);
		p.set_draw_callback(draw2d_callback);

		std::cout << ig::get_opengl_version() << '\n';

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
}
