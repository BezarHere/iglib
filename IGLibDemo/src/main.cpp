#include <iostream>
#include <string>
#include <thread>
#include <intrin.h>
#include <istream>
#include <fstream>

#include <iglib.h>
#include <winsock.h>
#undef min
#undef max

using namespace ig;

const std::string current_dir = "F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\";


float zb = 0;
ig::Transform2D tr{};
ig::Texture before_tex;
ig::Texture after_tex;

struct LARGE
{
	LARGE() {

	}
	__m256i a[ 8 ]{};
};

void a( LARGE a, LARGE b, LARGE c, LARGE d ) {
	LARGE *aptr = &a;
	LARGE *bptr = &b;
	LARGE *cptr = &c;
	LARGE *dptr = &d;

	size_t asz = size_t( aptr );
	size_t bsz = size_t( bptr );
	size_t csz = size_t( cptr );
	size_t dsz = size_t( dptr );

	size_t adif = bsz - asz;
	size_t ddif = dsz - asz;

	std::cout << aptr << bptr << cptr << dptr << '\n';
}

void callback( ig::Window &window, ig::WindowCallbackReason reason ) {
	if (reason == ig::WindowCallbackReason::Unfocused)
		window.ping();

	//std::cout << window.get_title() << ": " << (int)reason << '\n';
}

std::string readall( const std::string &fs ) {
	FILE *f;
	auto err = fopen_s( &f, fs.c_str(), "r" );

	if (f == NULL)
		return "invalid file";

	const size_t begin = ftell( f );
	fseek( f, 0, FILE_END );
	const size_t size = ftell( f ) - begin;
	fseek( f, begin, FILE_BEGIN );
	char *buf = new char[ size ];
	fread( buf, 1, size, f );
	std::string st{ buf, size };
	delete[] buf;
	fclose( f );
	return st;
}



static float cube_distance = 0.f;
static float factor = 1.f;
static int rot_switch = 0;
void scroll( ig::Window &w, int16_t x, int16_t y ) {
	factor += y * 0.25f;
	std::cout << "factor: " << factor << '\n';
	std::cout << "scrolled by: " << x << ", " << y << '\n';
}

static ig::Transform3D ply{};
Vector2f mouse_pos_when_space = { 0.f, 0.f };
void input_callback( ig::Window &window, ig::InputEvent event, ig::InputEventType type ) {
	if (type == InputEventType::Key)
	{
		const auto key = event.key;

		if (key.keycode == KeyCode::Key_W)
		{

			zb -= 1.f;
			tr.rotate( 0.1 );
			window.ping();
			/*ig::Image img = window.to_image();
			img.flip_v();
			img.save_tga("F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\image.tga");*/
		}

		if (key.keycode == KeyCode::Key_S)
		{
			zb += 1.f;
			tr.rotate( -0.1f );
		}


		/*if (key == ig::Key_Space)
		{
			mouse_pos_when_space = window.get_mouse_position();
			tr.set_scale(tr.get_scale() + Vector2f{ 0.1f, 0.1f });
		}


		if (key == ig::Key_LeftControl)
		{
			mouse_pos_when_space = window.get_mouse_position();
			tr.set_scale(tr.get_scale() - Vector2f{ 0.1f, 0.1f });
		}*/

		const auto v = ply.get_basis().get_rotation();
		if (key.action != InputAction::Released)
		{
			switch (key.keycode)
			{
			case KeyCode::Key_W:
				ply.set_position( ply.get_position() - ply.get_back_dir() );
				break;
			case KeyCode::Key_S:
				ply.set_position( ply.get_position() + ply.get_back_dir() );
				break;
			case KeyCode::Key_D:
				ply.set_position( ply.get_position() - ply.get_left_dir() );
				break;
			case KeyCode::Key_A:
				ply.set_position( ply.get_position() + ply.get_left_dir() );
				break;
			case KeyCode::Key_Q:
				ply.set_position( ply.get_position() - ply.get_down_dir() );
				break;
			case KeyCode::Key_E:
				ply.set_position( ply.get_position() + ply.get_down_dir() );
				break;
			case KeyCode::Key_Z:
				ply.get_basis().set_angle( Vector3f( 0.f, Pi * factor / 2.f, 0.f ) + v );
				rot_switch = 1;
				break;
			case KeyCode::Key_X:
				ply.get_basis().set_angle( -Vector3f( 0.f, Pi * factor / 2.f, 0.f ) + v );
				rot_switch = 2;
				break;
			case KeyCode::Key_C:
				ply.get_basis().set_angle( Vector3f( Pi * factor / 2.f, 0.f, 0.f ) + v );
				rot_switch = 3;
				break;
			case KeyCode::Key_V:
				ply.get_basis().set_angle( -Vector3f( Pi * factor / 2.f, 0.f, 0.f ) + v );
				rot_switch = 4;
				break;
			case KeyCode::Key_B:
				ply.get_basis().set_angle( Vector3f( 0.f, 0.f, Pi * factor / 2.f ) + v );
				rot_switch = 5;
				break;
			case KeyCode::Key_N:
				ply.get_basis().set_angle( -Vector3f( 0.f, 0.f, Pi * factor / 2.f ) + v );
				rot_switch = 6;
				break;
			default:
				break;
			}
		}
	}
	else if (type == InputEventType::MouseScrollWheel)
	{
		scroll( window, event.mouse_scroll.x, event.mouse_scroll.y );
	}

}

static ig::Font *font;
static ig::Text2D *text;
void draw_fromto_comp( Canvas &c, Renderer &rend ) {

	//c.set_texture( 0 );
	//c.transform2d().rotate( c.get_window().get_shader_time() );

	c.draw( text->get_buffer() );
	const float lowest_axis = float( std::min( c.get_renderer()->get_window().width(), c.get_renderer()->get_window().height() ) ) / 2.f;
	rend.bind_texture( NULL );
	c.rect( { 0.f, 0.f }, { lowest_axis, lowest_axis }, { 1.0, 1.0, 1.0 } );
	//c.set_texture(font->get_atlas());
	c.rect( { lowest_axis + 16.f, 0.0f }, { lowest_axis * 2 + 16.f, lowest_axis }, { 1.0, 1.0, 1.0 } );
}

void draw2d_callback( ig::Renderer &rend ) {
	Canvas &c = rend.get_canvas();



	static Vector2f last_m{};
	static bool first_call = true;
	const ig::Vector2f m = c.get_renderer()->get_window().get_mouse_position();
	const bool m_inside_window = c.get_renderer()->get_window().width() > m.x && c.get_renderer()->get_window().height() > m.y && m.x > 0.f && m.y > 0.f;
	if (first_call)
	{
		last_m = m;
		first_call = false;
	}


	const float t = c.get_renderer()->get_window().get_shader_time();
	//switch (rot_switch)
	//{
	//case 1:
	//	ply.get_basis().set_angle(Vector3f(Pi * factor * t / 2.f, 0.f, 0.f));
	//	break;
	//case 2:
	//	ply.get_basis().set_angle(-Vector3f(Pi * factor * t / 2.f, 0.f, 0.f));
	//	break;
	//case 3:
	//	ply.get_basis().set_angle(Vector3f(0.f, Pi * factor * t / 2.f, 0.f));
	//	break;
	//case 4:
	//	ply.get_basis().set_angle(-Vector3f(0.f, Pi * factor * t / 2.f, 0.f));
	//	break;
	//case 5:
	//	ply.get_basis().set_angle(Vector3f(0.f, 0.f, Pi * factor * t / 2.f));
	//	break;
	//case 6:
	//	ply.get_basis().set_angle(-Vector3f(0.f, 0.f, Pi * factor * t / 2.f));
	//	break;
	//default:
	//	break;
	//}

	c.rect( { 0.f, 0.f }, rend.get_window().size(), ColorfTable::Skyblue );
	if (m_inside_window)
	{
		static Vector3f rot = {};
		const auto headrot = Vector3f( (m.y - last_m.y) / 60.f, (m.x - last_m.x) / 60.f, 0.f );
		rot += headrot;
		//std::cout << rot << '\n';
		Basis a{ { rot.x, 0.f, 0.f } };
		Basis b{ { 0.f, rot.y, 0.f } };
		ply.get_basis() = a * b;
		//std::cout << ply.get_basis().xdir << ' ' << ply.get_basis().ydir << ' ' << ply.get_basis().zdir << '\n';
	}

	//const Vector2f inverted_m{ m.x - (c.get_window().width() / 2.0f), (c.get_window().height() / 2.0f) - m.y };
	const Vector2f inverted_m{ 0.f, 0.f };
	//c.demo();


	rend.bind_texture( before_tex.get_handle() );
	rend.bind_default_shader( ShaderUsage::Usage3D );
	rend.set_draw_type( DrawType::Drawing3D );
	c.camera().transform = ply;
	c.update_camera();
	rend.try_update_shader_state();
	c.cube( { inverted_m.x / 100.0f + 2.f, inverted_m.y / 100.0f, cube_distance }, { 1.f, 1.f, 1.f }, { 1.0f, 0.8f, 0.6f, 1.f } );
	c.cube( { inverted_m.x / 100.0f, inverted_m.y / 100.0f, cube_distance }, { 1.f, 1.f, 1.f }, { 0.6f, 0.4f, 0.2f, 1.f } );
	c.cube( { inverted_m.x / 100.0f - 2.f, inverted_m.y / 100.0f, cube_distance }, { 1.f, 1.f, 1.f }, { 0.3f, 0.2f, 0.1f, 1.f } );
	rend.bind_default_shader( ShaderUsage::Usage2D );
	//rend.bind_texture( 0 );
	rend.disable_feature( Feature::DepthTest );
	//draw_fromto_comp(c, rend);

	c.rect( { 32.f, 32.f }, { 128.f, 128.f }, ColorfTable::White );

	c.circle( 64.f, rend.get_window().size() - Vector2f( 64.f, 64.f ), ColorfTable::Bisque, 16 );
	rend.bind_texture( 0 );
	c.circle( 32.f, rend.get_window().size() - Vector2f( 64.f, 64.f ), ColorfTable::Indianred, 64 );

	if (text->is_dirty())
		text->rebuild();
	c.text( *text );
	c.rect( { 0.f, 32.f }, { 128.f, 128.f + 32.f }, ColorfTable::White );

	{
		constexpr uint16_t Indices[ 6 ] = { 0, 1, 2, 2, 3, 0 };
		constexpr Vertex2 Vertices[ 4 ] = {
			{ Vector2f( 64.f, 64.f ), Colorf( 1.f, 0.f, 1.f ), Vector2f() },
			{ Vector2f( 164.f, 64.f ), Colorf( 1.f, 0.f, 0.f ), Vector2f() },
			{ Vector2f( 200.f, 200.f ), Colorf( 0.f, 1.f, 1.f ), Vector2f() },
			{ Vector2f( 64.f, 256.f ), Colorf( 1.f, 1.f, 0.f ), Vector2f() },
		};

		const ig::Index16Buffer index_buffer = { 6, ig::VBufferUsage::StaticRead, Indices };
		ig::Vertex2Buffer vertex_buffer = { ig::PrimitiveType::Triangle, 4, ig::BufferUsage::Static };
		vertex_buffer.update( Vertices );

		//c.draw( vertex_buffer, index_buffer );

	}


	//std::cout << c.transform3d().get_position() << '\n';

	//c.line(Vector3f{ 0.f, 0.f, 0.f }, Vector3f{ 1.f, 1.f, 1.f }, { 0.2f, 1.f, 0.2f, 1.f });
	//c.line(Vector3f{ 0.f, 0.f, 0.f }, Vector3f{ 30.f, 20.f, 10.f }, { 0.2f, 1.f, 0.2f, 1.f });
	//c.line(Vector3f{ 0.f, 0.f, 0.f }, Vector3f{ 400.f, 500.f, 600.f }, { 0.2f, 1.f, 0.2f, 1.f });
	//c.line(Vector3f{ 0.f, 0.f, 0.f }, { m.x, m.y, 20.0f }, { 0.8f, 1.f, 0.4f, 1.f });

	//c.quad(Vector2f(32.0f, 32.0f), Vector2f(32.0f, 32.0f + (m.y * 0.2f)), m, Vector2f(32.0f + (m.y * 0.2f), 32.0f), { 255, 44, 99, 255 });
	//

	//c.rect(c.get_window().size() - mouse_pos_when_space, c.get_window().get_mouse_position(), { 1.0, 1.0, 1.0 });

	//c.bind_shader(ss);
	//c.line(c.get_window().get_size() / 2, m, {255, 0, 0, 255});
	const auto state = ig::diag::OpenglStateFields::create();

	last_m = m;
}

template <typename _PROC>
void process_img( Image &img, _PROC proc ) {
	for (int y = 0; y < img.height(); y++)
	{
		for (int x = 0; x < img.width(); x++)
		{
			const int index = ((y * img.width()) + x) * int( img.format() );
			proc( img.get_buffer() + index );
		}
	}
}

int main() {
	//ig::wavefront::Obj o{ "v 12 44 41\nv 54 65 11\n\nvp 12" };

	//std::cout << readall(current_dir + "\\main.cpp") << '\n';

	//std::cout << "hello there\n";
	//auto k = ig::Image("F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\image.png");
	//std::cout << "hello there\n";
	//std::cout << k.get_buffer_size() << '\n';
	//std::cout << "hello there\n";
	//std::cout << (int)k.format() << '\n';
	//std::cout << "hello there\n";
	//std::cout << k.get_size() << '\n';
	//std::cout << "wadawdasdawdas" << '\n';

	//a(LARGE{}, LARGE{}, LARGE{}, LARGE{});


	{
		std::cout << "starting\n";
		ig::Window i = ig::Window( { 512, 512 }, "Window !!!" );
		std::cout << "created the window\n";

		font = new Font( "F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\font.ttf", 14, []( codepoint_t cp ) { return cp < 128; } );
		//delete font;
		//font = new Font( Font::get_default() );

		std::cout << "created the font\n";

		//font->set_char_spacing( -2 );
		const std::string garbage = readall( "F:\\Assets\\visual studio\\IGLib\\iglib\\py\\signed_distance_field_img2.py" ) + readall( "F:\\Assets\\visual studio\\IGLib\\iglib\\py\\signed_distance_field_img2.py" );
		//text = new Text2D( "hello?! dat not good q", *font );
		text = new Text2D( garbage, *font );
		std::cout << "created the Text2D\n";

		text->set_scale( { 1.f, 1.f } );

		{
			ig::Image img{ "F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\checkers.png" };
			before_tex = ig::Texture( img );
			Image img2 = img;
			img2.convert( ColorFormat::LA );
			img2.convert( ColorFormat::RGBA );
			img.blit( img2, { 0, 0, 256, 256 }, { 200, 200 } );
			after_tex = ig::Texture( img );
		}
		std::cout << "made sample textures\n";

		i.set_callback( callback );
		i.set_input_callback( input_callback );
		std::cout << "set window callbacks\n";

		ig::Renderer renderer{ i, draw2d_callback };
		std::cout << "created renderer\n";
		//ig::RenderEnvironment ebv = renderer.get_environment();
		//ebv.enabled_postprocessing = false;
		//renderer.set_environment( ebv );

		renderer.set_environment( ig::RenderEnvironment{ false } );

		std::cout << ig::get_opengl_version() << '\n';
		std::cout << "main loop running\n";
		while (!i.should_close())
		{
			std::this_thread::sleep_for( std::chrono::microseconds( long long( 1000.0 / 20.0 ) ) );

			renderer.clear();
			renderer.draw();
			i.poll();

		}

	}
}
