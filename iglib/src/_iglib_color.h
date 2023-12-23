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

		constexpr BaseColorTemplate()
			: r{}, g{}, b{}, a{ value_type(full_value) }
		{}

		constexpr BaseColorTemplate(value_type rr, value_type gg, value_type bb, value_type aa = value_type(full_value))
			: r{ rr }, g{ gg }, b{ bb }, a{ aa }
		{}

		constexpr this_type inverted(const bool use_alpha = false) const
		{
			return { full_value - r, full_value - g, full_value - b, use_alpha ? a : (full_value - a) };
		}

		constexpr this_type brighten(const value_type factor, const bool use_alpha = false) const
		{
			return { full_value * factor, full_value * factor, full_value * factor, use_alpha ? full_value : (full_value * factor) };
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

		constexpr color_type rgbf( double r, double g, double b ) {
			return { (color_type::value_type)(color_type::full_value * r),
							 (color_type::value_type)(color_type::full_value * g),
							 (color_type::value_type)(color_type::full_value * b),
								color_type::full_value };
		}

		constexpr color_type rgbaf( double r, double g, double b, double a ) {
			return { (color_type::value_type)(color_type::full_value * r),
							 (color_type::value_type)(color_type::full_value * g),
							 (color_type::value_type)(color_type::full_value * b),
							 (color_type::value_type)(color_type::full_value * a) };
		}
		
		
		static constexpr color_type Darkcyan = { 0.0, 0.5450980392156862, 0.5450980392156862 };
		static constexpr color_type Mintcream = { 0.9607843137254902, 1.0, 0.9803921568627451 };
		static constexpr color_type Pink = { 1.0, 0.7529411764705882, 0.796078431372549 };
		static constexpr color_type Mediumvioletred = { 0.7803921568627451, 0.08235294117647059, 0.5215686274509804 };
		static constexpr color_type Gold = { 1.0, 0.8431372549019608, 0.0 };
		static constexpr color_type Hotpink = { 1.0, 0.4117647058823529, 0.7058823529411765 };
		static constexpr color_type Springgreen = { 0.0, 1.0, 0.4980392156862745 };
		static constexpr color_type Ghostwhite = { 0.9725490196078431, 0.9725490196078431, 1.0 };
		static constexpr color_type Mediumslateblue = { 0.4823529411764706, 0.40784313725490196, 0.9333333333333333 };
		static constexpr color_type Orange = { 1.0, 0.6470588235294118, 0.0 };
		static constexpr color_type Fuchsia = { 1.0, 0.0, 1.0 };
		static constexpr color_type Teal = { 0.0, 0.5019607843137255, 0.5019607843137255 };
		static constexpr color_type Khaki = { 0.9411764705882353, 0.9019607843137255, 0.5490196078431373 };
		static constexpr color_type Navyblue = { 0.0, 0.0, 0.5019607843137255 };
		static constexpr color_type Mediumseagreen = { 0.23529411764705882, 0.7019607843137254, 0.44313725490196076 };
		static constexpr color_type Goldenrod = { 0.8549019607843137, 0.6470588235294118, 0.12549019607843137 };
		static constexpr color_type Darkolivegreen = { 0.3333333333333333, 0.4196078431372549, 0.1843137254901961 };
		static constexpr color_type Seagreen = { 0.1803921568627451, 0.5450980392156862, 0.3411764705882353 };
		static constexpr color_type Webgreen = { 0.0, 0.5019607843137255, 0.0 };
		static constexpr color_type Orangered = { 1.0, 0.27058823529411763, 0.0 };
		static constexpr color_type White = { 1.0, 1.0, 1.0 };
		static constexpr color_type Lightsalmon = { 1.0, 0.6274509803921569, 0.47843137254901963 };
		static constexpr color_type Blueviolet = { 0.5411764705882353, 0.16862745098039217, 0.8862745098039215 };
		static constexpr color_type Lawngreen = { 0.48627450980392156, 0.9882352941176471, 0.0 };
		static constexpr color_type Snow = { 1.0, 0.9803921568627451, 0.9803921568627451 };
		static constexpr color_type Blue = { 0.0, 0.0, 1.0 };
		static constexpr color_type Lightcoral = { 0.9411764705882353, 0.5019607843137255, 0.5019607843137255 };
		static constexpr color_type Lavender = { 0.9019607843137255, 0.9019607843137255, 0.9803921568627451 };
		static constexpr color_type Linen = { 0.9803921568627451, 0.9411764705882353, 0.9019607843137255 };
		static constexpr color_type Violet = { 0.9333333333333333, 0.5098039215686274, 0.9333333333333333 };
		static constexpr color_type Lightslategray = { 0.4666666666666667, 0.5333333333333333, 0.6 };
		static constexpr color_type Darkslategray = { 0.1843137254901961, 0.30980392156862746, 0.30980392156862746 };
		static constexpr color_type Plum = { 0.8666666666666667, 0.6274509803921569, 0.8666666666666667 };
		static constexpr color_type Darkmagenta = { 0.5450980392156862, 0.0, 0.5450980392156862 };
		static constexpr color_type Darkgoldenrod = { 0.7215686274509804, 0.5254901960784314, 0.043137254901960784 };
		static constexpr color_type Lavenderblush = { 1.0, 0.9411764705882353, 0.9607843137254902 };
		static constexpr color_type Orchid = { 0.8549019607843137, 0.4392156862745098, 0.8392156862745098 };
		static constexpr color_type Indigo = { 0.29411764705882354, 0.0, 0.5098039215686274 };
		static constexpr color_type Lightsteelblue = { 0.6901960784313725, 0.7686274509803922, 0.8705882352941177 };
		static constexpr color_type Lightyellow = { 1.0, 1.0, 0.8784313725490196 };
		static constexpr color_type Burlywood = { 0.8705882352941177, 0.7215686274509804, 0.5294117647058824 };
		static constexpr color_type Rosybrown = { 0.7372549019607844, 0.5607843137254902, 0.5607843137254902 };
		static constexpr color_type Sienna = { 0.6274509803921569, 0.3215686274509804, 0.17647058823529413 };
		static constexpr color_type Darkgreen = { 0.0, 0.39215686274509803, 0.0 };
		static constexpr color_type Silver = { 0.7529411764705882, 0.7529411764705882, 0.7529411764705882 };
		static constexpr color_type Darkgray = { 0.6627450980392157, 0.6627450980392157, 0.6627450980392157 };
		static constexpr color_type Darkkhaki = { 0.7411764705882353, 0.7176470588235294, 0.4196078431372549 };
		static constexpr color_type Crimson = { 0.8627450980392157, 0.0784313725490196, 0.23529411764705882 };
		static constexpr color_type Palegoldenrod = { 0.9333333333333333, 0.9098039215686274, 0.6666666666666666 };
		static constexpr color_type Yellow = { 1.0, 1.0, 0.0 };
		static constexpr color_type Seashell = { 1.0, 0.9607843137254902, 0.9333333333333333 };
		static constexpr color_type Darkorange = { 1.0, 0.5490196078431373, 0.0 };
		static constexpr color_type Webgray = { 0.5019607843137255, 0.5019607843137255, 0.5019607843137255 };
		static constexpr color_type Lightgoldenrod = { 0.9803921568627451, 0.9803921568627451, 0.8235294117647058 };
		static constexpr color_type Antiquewhite = { 0.9803921568627451, 0.9215686274509803, 0.8431372549019608 };
		static constexpr color_type Lime = { 0.0, 1.0, 0.0 };
		static constexpr color_type Darkseagreen = { 0.5607843137254902, 0.7372549019607844, 0.5607843137254902 };
		static constexpr color_type Coral = { 1.0, 0.4980392156862745, 0.3137254901960784 };
		static constexpr color_type Sandybrown = { 0.9568627450980393, 0.6431372549019608, 0.3764705882352941 };
		static constexpr color_type Mediumblue = { 0.0, 0.0, 0.803921568627451 };
		static constexpr color_type Lightskyblue = { 0.5294117647058824, 0.807843137254902, 0.9803921568627451 };
		static constexpr color_type Darkblue = { 0.0, 0.0, 0.5450980392156862 };
		static constexpr color_type Purple = { 0.6274509803921569, 0.12549019607843137, 0.9411764705882353 };
		static constexpr color_type Yellowgreen = { 0.6039215686274509, 0.803921568627451, 0.19607843137254902 };
		static constexpr color_type Gray = { 0.7450980392156863, 0.7450980392156863, 0.7450980392156863 };
		static constexpr color_type Olivedrab = { 0.4196078431372549, 0.5568627450980392, 0.13725490196078433 };
		static constexpr color_type Azure = { 0.9411764705882353, 1.0, 1.0 };
		static constexpr color_type Chocolate = { 0.8235294117647058, 0.4117647058823529, 0.11764705882352941 };
		static constexpr color_type Chartreuse = { 0.4980392156862745, 1.0, 0.0 };
		static constexpr color_type Black = { 0.0, 0.0, 0.0 };
		static constexpr color_type Webmaroon = { 0.5019607843137255, 0.0, 0.0 };
		static constexpr color_type Deepskyblue = { 0.0, 0.7490196078431373, 1.0 };
		static constexpr color_type Limegreen = { 0.19607843137254902, 0.803921568627451, 0.19607843137254902 };
		static constexpr color_type Darksalmon = { 0.9137254901960784, 0.5882352941176471, 0.47843137254901963 };
		static constexpr color_type Bisque = { 1.0, 0.8941176470588236, 0.7686274509803922 };
		static constexpr color_type Magenta = { 1.0, 0.0, 1.0 };
		static constexpr color_type Saddlebrown = { 0.5450980392156862, 0.27058823529411763, 0.07450980392156863 };
		static constexpr color_type Papayawhip = { 1.0, 0.9372549019607843, 0.8352941176470589 };
		static constexpr color_type Dodgerblue = { 0.11764705882352941, 0.5647058823529412, 1.0 };
		static constexpr color_type Royalblue = { 0.2549019607843137, 0.4117647058823529, 0.8823529411764706 };
		static constexpr color_type Steelblue = { 0.27450980392156865, 0.5098039215686274, 0.7058823529411765 };
		static constexpr color_type Gainsboro = { 0.8627450980392157, 0.8627450980392157, 0.8627450980392157 };
		static constexpr color_type Powderblue = { 0.6901960784313725, 0.8784313725490196, 0.9019607843137255 };
		static constexpr color_type Lemonchiffon = { 1.0, 0.9803921568627451, 0.803921568627451 };
		static constexpr color_type Thistle = { 0.8470588235294118, 0.7490196078431373, 0.8470588235294118 };
		static constexpr color_type Slategray = { 0.4392156862745098, 0.5019607843137255, 0.5647058823529412 };
		static constexpr color_type Cornsilk = { 1.0, 0.9725490196078431, 0.8627450980392157 };
		static constexpr color_type Darkviolet = { 0.5803921568627451, 0.0, 0.8274509803921568 };
		static constexpr color_type Ivory = { 1.0, 1.0, 0.9411764705882353 };
		static constexpr color_type Honeydew = { 0.9411764705882353, 1.0, 0.9411764705882353 };
		static constexpr color_type Tan = { 0.8235294117647058, 0.7058823529411765, 0.5490196078431373 };
		static constexpr color_type Aquamarine = { 0.4980392156862745, 1.0, 0.8313725490196079 };
		static constexpr color_type Maroon = { 0.6901960784313725, 0.18823529411764706, 0.3764705882352941 };
		static constexpr color_type Lightgreen = { 0.5647058823529412, 0.9333333333333333, 0.5647058823529412 };
		static constexpr color_type Darkslateblue = { 0.2823529411764706, 0.23921568627450981, 0.5450980392156862 };
		static constexpr color_type Lightblue = { 0.6784313725490196, 0.8470588235294118, 0.9019607843137255 };
		static constexpr color_type Darkorchid = { 0.6, 0.19607843137254902, 0.8 };
		static constexpr color_type Green = { 0.0, 1.0, 0.0 };
		static constexpr color_type Lightpink = { 1.0, 0.7137254901960784, 0.7568627450980392 };
		static constexpr color_type Skyblue = { 0.5294117647058824, 0.807843137254902, 0.9215686274509803 };
		static constexpr color_type Moccasin = { 1.0, 0.8941176470588236, 0.7098039215686275 };
		static constexpr color_type Paleturquoise = { 0.6862745098039216, 0.9333333333333333, 0.9333333333333333 };
		static constexpr color_type Mediumspringgreen = { 0.0, 0.9803921568627451, 0.6039215686274509 };
		static constexpr color_type Whitesmoke = { 0.9607843137254902, 0.9607843137254902, 0.9607843137254902 };
		static constexpr color_type Mediumaquamarine = { 0.4, 0.803921568627451, 0.6666666666666666 };
		static constexpr color_type Forestgreen = { 0.13333333333333333, 0.5450980392156862, 0.13333333333333333 };
		static constexpr color_type Beige = { 0.9607843137254902, 0.9607843137254902, 0.8627450980392157 };
		static constexpr color_type Turquoise = { 0.25098039215686274, 0.8784313725490196, 0.8156862745098039 };
		static constexpr color_type Lightseagreen = { 0.12549019607843137, 0.6980392156862745, 0.6666666666666666 };
		static constexpr color_type Mediumorchid = { 0.7294117647058823, 0.3333333333333333, 0.8274509803921568 };
		static constexpr color_type Darkturquoise = { 0.0, 0.807843137254902, 0.8196078431372549 };
		static constexpr color_type Dimgray = { 0.4117647058823529, 0.4117647058823529, 0.4117647058823529 };
		static constexpr color_type Palegreen = { 0.596078431372549, 0.984313725490196, 0.596078431372549 };
		static constexpr color_type Tomato = { 1.0, 0.38823529411764707, 0.2784313725490196 };
		static constexpr color_type Blanchedalmond = { 1.0, 0.9215686274509803, 0.803921568627451 };
		static constexpr color_type Cornflower = { 0.39215686274509803, 0.5843137254901961, 0.9294117647058824 };
		static constexpr color_type Floralwhite = { 1.0, 0.9803921568627451, 0.9411764705882353 };
		static constexpr color_type Lightcyan = { 0.8784313725490196, 1.0, 1.0 };
		static constexpr color_type Firebrick = { 0.6980392156862745, 0.13333333333333333, 0.13333333333333333 };
		static constexpr color_type Peru = { 0.803921568627451, 0.5215686274509804, 0.24705882352941178 };
		static constexpr color_type Brown = { 0.6470588235294118, 0.16470588235294117, 0.16470588235294117 };
		static constexpr color_type Transparent = { 1.0, 1.0, 1.0, 0.0 };
		static constexpr color_type Oldlace = { 0.9921568627450981, 0.9607843137254902, 0.9019607843137255 };
		static constexpr color_type Palevioletred = { 0.8588235294117647, 0.4392156862745098, 0.5764705882352941 };
		static constexpr color_type Rebeccapurple = { 0.4, 0.2, 0.6 };
		static constexpr color_type Red = { 1.0, 0.0, 0.0 };
		static constexpr color_type Cyan = { 0.0, 1.0, 1.0 };
		static constexpr color_type Olive = { 0.5019607843137255, 0.5019607843137255, 0.0 };
		static constexpr color_type Cadetblue = { 0.37254901960784315, 0.6196078431372549, 0.6274509803921569 };
		static constexpr color_type Salmon = { 0.9803921568627451, 0.5019607843137255, 0.4470588235294118 };
		static constexpr color_type Midnightblue = { 0.09803921568627451, 0.09803921568627451, 0.4392156862745098 };
		static constexpr color_type Aliceblue = { 0.9411764705882353, 0.9725490196078431, 1.0 };
		static constexpr color_type Wheat = { 0.9607843137254902, 0.8705882352941177, 0.7019607843137254 };
		static constexpr color_type Lightgray = { 0.8274509803921568, 0.8274509803921568, 0.8274509803921568 };
		static constexpr color_type Indianred = { 0.803921568627451, 0.3607843137254902, 0.3607843137254902 };
		static constexpr color_type Darkred = { 0.5450980392156862, 0.0, 0.0 };
		static constexpr color_type Navajowhite = { 1.0, 0.8705882352941177, 0.6784313725490196 };
		static constexpr color_type Slateblue = { 0.41568627450980394, 0.35294117647058826, 0.803921568627451 };
		static constexpr color_type Mistyrose = { 1.0, 0.8941176470588236, 0.8823529411764706 };
		static constexpr color_type Peachpuff = { 1.0, 0.8549019607843137, 0.7254901960784313 };
		static constexpr color_type Mediumpurple = { 0.5764705882352941, 0.4392156862745098, 0.8588235294117647 };
		static constexpr color_type Aqua = { 0.0, 1.0, 1.0 };
		static constexpr color_type Webpurple = { 0.5019607843137255, 0.0, 0.5019607843137255 };
		static constexpr color_type Deeppink = { 1.0, 0.0784313725490196, 0.5764705882352941 };
		static constexpr color_type Greenyellow = { 0.6784313725490196, 1.0, 0.1843137254901961 };
		static constexpr color_type Mediumturquoise = { 0.2823529411764706, 0.8196078431372549, 0.8 };


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
