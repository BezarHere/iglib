from dataclasses import dataclass
import math
from pathlib import Path
from PIL import Image
from itertools import product, batched

Color = tuple[int, int, int]

@dataclass(slots=True)
class NumberCheckerboardImageSampler:
	square_size: tuple[int, int]
	colors: tuple[Color]
	
	def generate(self, size: tuple[int, int], mode: str = 'RGBA'):
		img = Image.new(mode, size)
		squares_count = int(math.ceil(size[0] / self.square_size[0])), int(math.ceil(size[1] / self.square_size[1]))
		colors_len = len(self.colors)

		for y, x in product(range(squares_count[1]), range(squares_count[0])):
			clr = self.colors[((y * squares_count[0]) + x) % colors_len]
			offset = x * self.square_size[0], y * self.square_size[1]
			cur_sq_size = min(self.square_size[0], size[0] - offset[0]), min(self.square_size[1], size[1] - offset[1])
			for i, j in product(range(cur_sq_size[0]), range(cur_sq_size[1])):
				img.putpixel((i + offset[0], j + offset[1]), clr)
		return img

def gradient_filter(img: Image.Image):
	for y in range(img.height):
		factor = math.pow((img.height - y) / img.height, 2.0)
		for x in range(img.width):
			pixel = img.getpixel((x, y))
			clr = tuple(int(round(factor * i)) for i in pixel)
			if len(clr) == 4:
				clr = clr[0], clr[1], clr[2], pixel[3]
			img.putpixel((x, y), clr)
	return img

if __name__ == "__main__":
	colors = tuple(product((255, 0), repeat=3)) + ((127, 127, 127), )

	def to_vec2i(s: str):
		return tuple(map(lambda v: int(v.strip()), s.split(',', 2)))
	
	
	
	nc = NumberCheckerboardImageSampler(
		to_vec2i(input('Square size: ')),
		colors
	)

	squares_count = to_vec2i(input('Squares count: '))
	img_size = nc.square_size[0] * squares_count[0], nc.square_size[1] * squares_count[1]

	gradient_filter(nc.generate(tuple(img_size))).save(Path(__file__).parent.joinpath("checkers.png"))

