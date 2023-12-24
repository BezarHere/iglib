#pragma once
#include "_iglib_base.h"

namespace ig
{
	template <typename _T, int _DEFAULT>
	struct BaseColorTemplate
	{
		using value_type = _T;
		using this_type = BaseColorTemplate<_T, _DEFAULT>;
		static constexpr value_type full_value = value_type(_DEFAULT);

		inline constexpr BaseColorTemplate()
			: r{}, g{}, b{}, a{ value_type(full_value) }
		{}

		inline constexpr BaseColorTemplate(value_type rr, value_type gg, value_type bb, value_type aa = value_type(full_value))
			: r{ rr }, g{ gg }, b{ bb }, a{ aa }
		{}

		inline constexpr this_type inverted(const bool use_alpha = false) const
		{
			return { full_value - r, full_value - g, full_value - b, use_alpha ? a : (full_value - a) };
		}

		inline constexpr this_type brighten(const value_type factor, const bool use_alpha = false) const
		{
			return { r * factor, g * factor, b * factor, use_alpha ? a : (a * factor) };
		}

		inline constexpr this_type darken( const value_type factor, const bool use_alpha = false ) const {
			return brighten( full_value / factor, use_alpha );
		}

		// set the opacity (alpha) of the color
		inline constexpr this_type opacity(const value_type alpha) const
		{
			return { r, g, b, alpha };
		}

		// set the transparency (inverse of alpha) of the color
		inline constexpr _NODISCARD this_type transparency(const value_type alpha) const
		{
			return { r, g, b, full_value - alpha };
		}

		inline constexpr _NODISCARD this_type operator*( const this_type &other ) const {
			return { r * other.r, g * other.g, b * other.b, a * other.a };
		}

		inline constexpr _NODISCARD this_type operator*( value_type factor ) const {
			return { r * factor, g * factor, b * factor, a * factor };
		}

		inline constexpr _NODISCARD this_type operator/( const this_type &other ) const {
			return { r / other.r, g / other.g, b / other.b, a / other.a };
		}

		inline constexpr _NODISCARD this_type operator/( value_type factor ) const {
			return { r / factor, g / factor, b / factor, a / factor };
		}

		inline constexpr _NODISCARD this_type operator+( const this_type &other ) const {
			return { r + other.r, g + other.g, b + other.b, a + other.a };
		}

		inline constexpr _NODISCARD this_type operator-( const this_type &other ) const {
			return { r - other.r, g - other.g, b - other.b, a - other.a };
		}

		inline constexpr this_type &operator*=( const this_type &other ) {
			r *= other.r;
			g *= other.g;
			b *= other.b;
			a *= other.a;
			return *this;
		}

		inline constexpr this_type &operator/=( const this_type &other ) {
			r /= other.r;
			g /= other.g;
			b /= other.b;
			a /= other.a;
			return *this;
		}

		inline constexpr this_type &operator+=( const this_type &other ) {
			r += other.r;
			g += other.g;
			b += other.b;
			a += other.a;
			return *this;
		}

		inline constexpr this_type &operator-=( const this_type &other ) {
			r -= other.r;
			g -= other.g;
			b -= other.b;
			a -= other.a;
			return *this;
		}

		inline constexpr _NODISCARD this_type operator~() const {
			return this->inverted();
		}

		inline constexpr _NODISCARD this_type operator&(const this_type &other) const {
			static constexpr value_type Two = value_type( 2 );
			return { (r + other.r) / Two, (g + other.g) / Two, (b + other.b) / Two, (a + other.a) / Two };
		}

		value_type r, g, b, a;
	};


	using Colorb = BaseColorTemplate<byte, 255>;
	using Color16 = BaseColorTemplate<uint16_t, 65535>;
	using Colorf = BaseColorTemplate<float_t, 1>;
	using Colord = BaseColorTemplate<double_t, 1>;

	template <typename _COLOR>
	struct BaseColorTableTemplate
	{
		using color_type = _COLOR;
		using color_value_type = typename color_type::value_type;

		static constexpr color_type rgb( double r, double g, double b ) {
			return { (color_value_type)(color_type::full_value * r),
							 (color_value_type)(color_type::full_value * g),
							 (color_value_type)(color_type::full_value * b),
								color_type::full_value };
		}

		static constexpr color_type rgba( double r, double g, double b, double a ) {
			return { (color_value_type)(color_type::full_value * r),
							 (color_value_type)(color_type::full_value * g),
							 (color_value_type)(color_type::full_value * b),
							 (color_value_type)(color_type::full_value * a) };
		}
		
		static constexpr color_type Aliceblue = rgb( 0.9411764705882353, 0.9725490196078431, 1.0 );
		static constexpr color_type Antiquewhite = rgb( 0.9803921568627451, 0.9215686274509803, 0.8431372549019608 );
		static constexpr color_type Aqua = rgb( 0.0, 1.0, 1.0 );
		static constexpr color_type Aquamarine = rgb( 0.4980392156862745, 1.0, 0.8313725490196079 );
		static constexpr color_type Azure = rgb( 0.9411764705882353, 1.0, 1.0 );
		static constexpr color_type Beige = rgb( 0.9607843137254902, 0.9607843137254902, 0.8627450980392157 );
		static constexpr color_type Bisque = rgb( 1.0, 0.8941176470588236, 0.7686274509803922 );
		static constexpr color_type Black = rgb( 0.0, 0.0, 0.0 );
		static constexpr color_type Blanchedalmond = rgb( 1.0, 0.9215686274509803, 0.803921568627451 );
		static constexpr color_type Blue = rgb( 0.0, 0.0, 1.0 );
		static constexpr color_type Blueviolet = rgb( 0.5411764705882353, 0.16862745098039217, 0.8862745098039215 );
		static constexpr color_type Brown = rgb( 0.6470588235294118, 0.16470588235294117, 0.16470588235294117 );
		static constexpr color_type Burlywood = rgb( 0.8705882352941177, 0.7215686274509804, 0.5294117647058824 );
		static constexpr color_type Cadetblue = rgb( 0.37254901960784315, 0.6196078431372549, 0.6274509803921569 );
		static constexpr color_type Chartreuse = rgb( 0.4980392156862745, 1.0, 0.0 );
		static constexpr color_type Chocolate = rgb( 0.8235294117647058, 0.4117647058823529, 0.11764705882352941 );
		static constexpr color_type Coral = rgb( 1.0, 0.4980392156862745, 0.3137254901960784 );
		static constexpr color_type Cornflower = rgb( 0.39215686274509803, 0.5843137254901961, 0.9294117647058824 );
		static constexpr color_type Cornsilk = rgb( 1.0, 0.9725490196078431, 0.8627450980392157 );
		static constexpr color_type Crimson = rgb( 0.8627450980392157, 0.0784313725490196, 0.23529411764705882 );
		static constexpr color_type Cyan = rgb( 0.0, 1.0, 1.0 );
		static constexpr color_type Darkblue = rgb( 0.0, 0.0, 0.5450980392156862 );
		static constexpr color_type Darkcyan = rgb( 0.0, 0.5450980392156862, 0.5450980392156862 );
		static constexpr color_type Darkgoldenrod = rgb( 0.7215686274509804, 0.5254901960784314, 0.043137254901960784 );
		static constexpr color_type Darkgray = rgb( 0.6627450980392157, 0.6627450980392157, 0.6627450980392157 );
		static constexpr color_type Darkgreen = rgb( 0.0, 0.39215686274509803, 0.0 );
		static constexpr color_type Darkkhaki = rgb( 0.7411764705882353, 0.7176470588235294, 0.4196078431372549 );
		static constexpr color_type Darkmagenta = rgb( 0.5450980392156862, 0.0, 0.5450980392156862 );
		static constexpr color_type Darkolivegreen = rgb( 0.3333333333333333, 0.4196078431372549, 0.1843137254901961 );
		static constexpr color_type Darkorange = rgb( 1.0, 0.5490196078431373, 0.0 );
		static constexpr color_type Darkorchid = rgb( 0.6, 0.19607843137254902, 0.8 );
		static constexpr color_type Darkred = rgb( 0.5450980392156862, 0.0, 0.0 );
		static constexpr color_type Darksalmon = rgb( 0.9137254901960784, 0.5882352941176471, 0.47843137254901963 );
		static constexpr color_type Darkseagreen = rgb( 0.5607843137254902, 0.7372549019607844, 0.5607843137254902 );
		static constexpr color_type Darkslateblue = rgb( 0.2823529411764706, 0.23921568627450981, 0.5450980392156862 );
		static constexpr color_type Darkslategray = rgb( 0.1843137254901961, 0.30980392156862746, 0.30980392156862746 );
		static constexpr color_type Darkturquoise = rgb( 0.0, 0.807843137254902, 0.8196078431372549 );
		static constexpr color_type Darkviolet = rgb( 0.5803921568627451, 0.0, 0.8274509803921568 );
		static constexpr color_type Deeppink = rgb( 1.0, 0.0784313725490196, 0.5764705882352941 );
		static constexpr color_type Deepskyblue = rgb( 0.0, 0.7490196078431373, 1.0 );
		static constexpr color_type Dimgray = rgb( 0.4117647058823529, 0.4117647058823529, 0.4117647058823529 );
		static constexpr color_type Dodgerblue = rgb( 0.11764705882352941, 0.5647058823529412, 1.0 );
		static constexpr color_type Firebrick = rgb( 0.6980392156862745, 0.13333333333333333, 0.13333333333333333 );
		static constexpr color_type Floralwhite = rgb( 1.0, 0.9803921568627451, 0.9411764705882353 );
		static constexpr color_type Forestgreen = rgb( 0.13333333333333333, 0.5450980392156862, 0.13333333333333333 );
		static constexpr color_type Fuchsia = rgb( 1.0, 0.0, 1.0 );
		static constexpr color_type Gainsboro = rgb( 0.8627450980392157, 0.8627450980392157, 0.8627450980392157 );
		static constexpr color_type Ghostwhite = rgb( 0.9725490196078431, 0.9725490196078431, 1.0 );
		static constexpr color_type Gold = rgb( 1.0, 0.8431372549019608, 0.0 );
		static constexpr color_type Goldenrod = rgb( 0.8549019607843137, 0.6470588235294118, 0.12549019607843137 );
		static constexpr color_type Gray = rgb( 0.7450980392156863, 0.7450980392156863, 0.7450980392156863 );
		static constexpr color_type Green = rgb( 0.0, 1.0, 0.0 );
		static constexpr color_type Greenyellow = rgb( 0.6784313725490196, 1.0, 0.1843137254901961 );
		static constexpr color_type Honeydew = rgb( 0.9411764705882353, 1.0, 0.9411764705882353 );
		static constexpr color_type Hotpink = rgb( 1.0, 0.4117647058823529, 0.7058823529411765 );
		static constexpr color_type Indianred = rgb( 0.803921568627451, 0.3607843137254902, 0.3607843137254902 );
		static constexpr color_type Indigo = rgb( 0.29411764705882354, 0.0, 0.5098039215686274 );
		static constexpr color_type Ivory = rgb( 1.0, 1.0, 0.9411764705882353 );
		static constexpr color_type Khaki = rgb( 0.9411764705882353, 0.9019607843137255, 0.5490196078431373 );
		static constexpr color_type Lavender = rgb( 0.9019607843137255, 0.9019607843137255, 0.9803921568627451 );
		static constexpr color_type Lavenderblush = rgb( 1.0, 0.9411764705882353, 0.9607843137254902 );
		static constexpr color_type Lawngreen = rgb( 0.48627450980392156, 0.9882352941176471, 0.0 );
		static constexpr color_type Lemonchiffon = rgb( 1.0, 0.9803921568627451, 0.803921568627451 );
		static constexpr color_type Lightblue = rgb( 0.6784313725490196, 0.8470588235294118, 0.9019607843137255 );
		static constexpr color_type Lightcoral = rgb( 0.9411764705882353, 0.5019607843137255, 0.5019607843137255 );
		static constexpr color_type Lightcyan = rgb( 0.8784313725490196, 1.0, 1.0 );
		static constexpr color_type Lightgoldenrod = rgb( 0.9803921568627451, 0.9803921568627451, 0.8235294117647058 );
		static constexpr color_type Lightgray = rgb( 0.8274509803921568, 0.8274509803921568, 0.8274509803921568 );
		static constexpr color_type Lightgreen = rgb( 0.5647058823529412, 0.9333333333333333, 0.5647058823529412 );
		static constexpr color_type Lightpink = rgb( 1.0, 0.7137254901960784, 0.7568627450980392 );
		static constexpr color_type Lightsalmon = rgb( 1.0, 0.6274509803921569, 0.47843137254901963 );
		static constexpr color_type Lightseagreen = rgb( 0.12549019607843137, 0.6980392156862745, 0.6666666666666666 );
		static constexpr color_type Lightskyblue = rgb( 0.5294117647058824, 0.807843137254902, 0.9803921568627451 );
		static constexpr color_type Lightslategray = rgb( 0.4666666666666667, 0.5333333333333333, 0.6 );
		static constexpr color_type Lightsteelblue = rgb( 0.6901960784313725, 0.7686274509803922, 0.8705882352941177 );
		static constexpr color_type Lightyellow = rgb( 1.0, 1.0, 0.8784313725490196 );
		static constexpr color_type Lime = rgb( 0.0, 1.0, 0.0 );
		static constexpr color_type Limegreen = rgb( 0.19607843137254902, 0.803921568627451, 0.19607843137254902 );
		static constexpr color_type Linen = rgb( 0.9803921568627451, 0.9411764705882353, 0.9019607843137255 );
		static constexpr color_type Magenta = rgb( 1.0, 0.0, 1.0 );
		static constexpr color_type Maroon = rgb( 0.6901960784313725, 0.18823529411764706, 0.3764705882352941 );
		static constexpr color_type Mediumaquamarine = rgb( 0.4, 0.803921568627451, 0.6666666666666666 );
		static constexpr color_type Mediumblue = rgb( 0.0, 0.0, 0.803921568627451 );
		static constexpr color_type Mediumorchid = rgb( 0.7294117647058823, 0.3333333333333333, 0.8274509803921568 );
		static constexpr color_type Mediumpurple = rgb( 0.5764705882352941, 0.4392156862745098, 0.8588235294117647 );
		static constexpr color_type Mediumseagreen = rgb( 0.23529411764705882, 0.7019607843137254, 0.44313725490196076 );
		static constexpr color_type Mediumslateblue = rgb( 0.4823529411764706, 0.40784313725490196, 0.9333333333333333 );
		static constexpr color_type Mediumspringgreen = rgb( 0.0, 0.9803921568627451, 0.6039215686274509 );
		static constexpr color_type Mediumturquoise = rgb( 0.2823529411764706, 0.8196078431372549, 0.8 );
		static constexpr color_type Mediumvioletred = rgb( 0.7803921568627451, 0.08235294117647059, 0.5215686274509804 );
		static constexpr color_type Midnightblue = rgb( 0.09803921568627451, 0.09803921568627451, 0.4392156862745098 );
		static constexpr color_type Mintcream = rgb( 0.9607843137254902, 1.0, 0.9803921568627451 );
		static constexpr color_type Mistyrose = rgb( 1.0, 0.8941176470588236, 0.8823529411764706 );
		static constexpr color_type Moccasin = rgb( 1.0, 0.8941176470588236, 0.7098039215686275 );
		static constexpr color_type Navajowhite = rgb( 1.0, 0.8705882352941177, 0.6784313725490196 );
		static constexpr color_type Navyblue = rgb( 0.0, 0.0, 0.5019607843137255 );
		static constexpr color_type Oldlace = rgb( 0.9921568627450981, 0.9607843137254902, 0.9019607843137255 );
		static constexpr color_type Olive = rgb( 0.5019607843137255, 0.5019607843137255, 0.0 );
		static constexpr color_type Olivedrab = rgb( 0.4196078431372549, 0.5568627450980392, 0.13725490196078433 );
		static constexpr color_type Orange = rgb( 1.0, 0.6470588235294118, 0.0 );
		static constexpr color_type Orangered = rgb( 1.0, 0.27058823529411763, 0.0 );
		static constexpr color_type Orchid = rgb( 0.8549019607843137, 0.4392156862745098, 0.8392156862745098 );
		static constexpr color_type Palegoldenrod = rgb( 0.9333333333333333, 0.9098039215686274, 0.6666666666666666 );
		static constexpr color_type Palegreen = rgb( 0.596078431372549, 0.984313725490196, 0.596078431372549 );
		static constexpr color_type Paleturquoise = rgb( 0.6862745098039216, 0.9333333333333333, 0.9333333333333333 );
		static constexpr color_type Palevioletred = rgb( 0.8588235294117647, 0.4392156862745098, 0.5764705882352941 );
		static constexpr color_type Papayawhip = rgb( 1.0, 0.9372549019607843, 0.8352941176470589 );
		static constexpr color_type Peachpuff = rgb( 1.0, 0.8549019607843137, 0.7254901960784313 );
		static constexpr color_type Peru = rgb( 0.803921568627451, 0.5215686274509804, 0.24705882352941178 );
		static constexpr color_type Pink = rgb( 1.0, 0.7529411764705882, 0.796078431372549 );
		static constexpr color_type Plum = rgb( 0.8666666666666667, 0.6274509803921569, 0.8666666666666667 );
		static constexpr color_type Powderblue = rgb( 0.6901960784313725, 0.8784313725490196, 0.9019607843137255 );
		static constexpr color_type Purple = rgb( 0.6274509803921569, 0.12549019607843137, 0.9411764705882353 );
		static constexpr color_type Rebeccapurple = rgb( 0.4, 0.2, 0.6 );
		static constexpr color_type Red = rgb( 1.0, 0.0, 0.0 );
		static constexpr color_type Rosybrown = rgb( 0.7372549019607844, 0.5607843137254902, 0.5607843137254902 );
		static constexpr color_type Royalblue = rgb( 0.2549019607843137, 0.4117647058823529, 0.8823529411764706 );
		static constexpr color_type Saddlebrown = rgb( 0.5450980392156862, 0.27058823529411763, 0.07450980392156863 );
		static constexpr color_type Salmon = rgb( 0.9803921568627451, 0.5019607843137255, 0.4470588235294118 );
		static constexpr color_type Sandybrown = rgb( 0.9568627450980393, 0.6431372549019608, 0.3764705882352941 );
		static constexpr color_type Seagreen = rgb( 0.1803921568627451, 0.5450980392156862, 0.3411764705882353 );
		static constexpr color_type Seashell = rgb( 1.0, 0.9607843137254902, 0.9333333333333333 );
		static constexpr color_type Sienna = rgb( 0.6274509803921569, 0.3215686274509804, 0.17647058823529413 );
		static constexpr color_type Silver = rgb( 0.7529411764705882, 0.7529411764705882, 0.7529411764705882 );
		static constexpr color_type Skyblue = rgb( 0.5294117647058824, 0.807843137254902, 0.9215686274509803 );
		static constexpr color_type Slateblue = rgb( 0.41568627450980394, 0.35294117647058826, 0.803921568627451 );
		static constexpr color_type Slategray = rgb( 0.4392156862745098, 0.5019607843137255, 0.5647058823529412 );
		static constexpr color_type Snow = rgb( 1.0, 0.9803921568627451, 0.9803921568627451 );
		static constexpr color_type Springgreen = rgb( 0.0, 1.0, 0.4980392156862745 );
		static constexpr color_type Steelblue = rgb( 0.27450980392156865, 0.5098039215686274, 0.7058823529411765 );
		static constexpr color_type Tan = rgb( 0.8235294117647058, 0.7058823529411765, 0.5490196078431373 );
		static constexpr color_type Teal = rgb( 0.0, 0.5019607843137255, 0.5019607843137255 );
		static constexpr color_type Thistle = rgb( 0.8470588235294118, 0.7490196078431373, 0.8470588235294118 );
		static constexpr color_type Tomato = rgb( 1.0, 0.38823529411764707, 0.2784313725490196 );
		static constexpr color_type Transparent = rgba( 1.0, 1.0, 1.0, 0.0 ); // transparent white
		static constexpr color_type Turquoise = rgb( 0.25098039215686274, 0.8784313725490196, 0.8156862745098039 );
		static constexpr color_type Violet = rgb( 0.9333333333333333, 0.5098039215686274, 0.9333333333333333 );
		static constexpr color_type Void = rgba( 0.0, 0.0, 0.0, 0.0 ); // transparent black
		static constexpr color_type Webgray = rgb( 0.5019607843137255, 0.5019607843137255, 0.5019607843137255 );
		static constexpr color_type Webgreen = rgb( 0.0, 0.5019607843137255, 0.0 );
		static constexpr color_type Webmaroon = rgb( 0.5019607843137255, 0.0, 0.0 );
		static constexpr color_type Webpurple = rgb( 0.5019607843137255, 0.0, 0.5019607843137255 );
		static constexpr color_type Wheat = rgb( 0.9607843137254902, 0.8705882352941177, 0.7019607843137254 );
		static constexpr color_type White = rgb( 1.0, 1.0, 1.0 );
		static constexpr color_type Whitesmoke = rgb( 0.9607843137254902, 0.9607843137254902, 0.9607843137254902 );
		static constexpr color_type Yellow = rgb( 1.0, 1.0, 0.0 );
		static constexpr color_type Yellowgreen = rgb( 0.6039215686274509, 0.803921568627451, 0.19607843137254902 );


	private:
		BaseColorTableTemplate() = delete;
		~BaseColorTableTemplate() = delete;
	};

	using ColorbTable = BaseColorTableTemplate<Colorb>;
	using Color16Table = BaseColorTableTemplate<Color16>;
	using ColorfTable = BaseColorTableTemplate<Colorf>;
	using ColordTable = BaseColorTableTemplate<Colord>;

	//using Colorcomplex = BaseColorTemplate<std::complex<std::complex<std::complex<std::complex<std::complex<std::complex<double>>>>>>, -1>;

	//constexpr size_t c = sizeof(Colorcomplex);

}
