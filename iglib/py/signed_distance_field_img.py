from dataclasses import dataclass
from functools import cache
from itertools import product
import math
from pathlib import Path
from typing import Any, Callable
from PIL import Image
import numpy

@dataclass(slots=True, frozen=True)
class Thershold[T]:
	value: T
	bottom: T = 0
	top: T = 1.0

	def __call__(self, clr) -> Any:
		if isinstance(clr, (float, int)):
			return self.bottom if clr < self.value else self.top
		match len(clr):
			case 1:
				return self(clr[0])
			case 2:
				return self(clr[0] * clr[1])
			case 3:
				return self((clr[0] + clr[1] + clr[2]) / 3)
			case 4:
				return self(clr[:3]) * clr[3]
			case _:
				raise ValueError(clr)

KernalMatrix = tuple[tuple[float, ...]]
@dataclass
class Kernal:
	matrix: tuple[tuple[float, ...]]
	size: tuple[int, int]

	def __call__(self, x: int, y : int, val: float) -> Any:
		return self.matrix[y][x] * val


def sdf(img: Image.Image, output_size: tuple[int, int], distance_range = 5.0, gamma: float = 1.0, pixel_value: Callable | None = None):
	ret = Image.new("LA", output_size)
	def gen_tuple():
		for x in range(img.width):
			yield tuple(pixel_value(tuple(i / 255 for i in img.getpixel((x, y)))) for y in range(img.height))
	pvals = tuple(gen_tuple())

	size_factors = ret.width / img.width, ret.height / img.height

	hd = distance_range / 2.0

	for y in range(img.height):
		yrange = max(int(y - hd), 0), min(int(y + hd), img.height)

		for x in range(img.width):
			xrange = max(int(x - hd), 0), min(int(x + hd), img.width)

			max_value = 0
			for yd, xd in product(range(*yrange), range(*xrange)):
				pos = xd - x, yd - y

				if pos[0] == 0 and pos[1] == 0:
					continue
				if pvals[xd][yd] == 0:
					continue

				max_value = max(max_value, pvals[xd][yd] / math.sqrt(pos[0] * pos[0] + pos[1] * pos[1]))
			# print(min_pd)
			ret.putpixel((int(size_factors[0] * x), int(size_factors[1] * y)), (255, int(255 * pow(max_value, 1.0 / gamma))))
	return ret

def apply(img: Image.Image, output_size: tuple[int, int], kernal: Kernal, pixel_value: Callable | None = None):
	ret = Image.new("LA", output_size)
	def gen_tuple():
		for x in range(img.width):
			yield tuple(pixel_value(tuple(i / 255 for i in img.getpixel((x, y)))) for y in range(img.height))
	pvals = tuple(gen_tuple())

	size_factors = ret.width / img.width, ret.height / img.height

	hd = kernal.size[0] // 2, kernal.size[1] // 2
	kernal_area = kernal.size[0] * kernal.size[1]

	for y in range(img.height):
		yrange = max(int(y - hd[1]), 0), min(int(y + hd[1]), img.height)

		for x in range(img.width):
			xrange = max(int(x - hd[0]), 0), min(int(x + hd[0]), img.width)
			value: float = 0.0

			for yd, xd in product(range(*yrange), range(*xrange)):
				pos = xd - x, yd - y

				if pvals[xd][yd] == 0:
					continue

				value += kernal(pos[0] + hd[0], pos[1] + hd[1], pvals[xd][yd])
			# print(min_pd)
			ret.putpixel((int(size_factors[0] * x), int(size_factors[1] * y)), (255, int(255 * value / kernal_area)))
	return ret

def circle_kernal(size: tuple[int, int], gamma = 1.0):
	mid = size[0] / 2, size[1] / 2
	def gen_row(i: int):
		i2 = (i - mid[1]) * (i - mid[1])
		for j in range(size[0]):
			j2 = (j - mid[0]) * (j - mid[0])
			yield 1.0 / ((math.sqrt(i2 + j2) + 1.0) ** gamma)
	return tuple(tuple(gen_row(j)) for j in range(size[1]))

@cache
def length(k: KernalMatrix, sz: tuple[int, int]):
	v = 0
	for i, j in product(range(sz[0]), range(sz[1])):
		v += k[i][j]
	return abs(v) / (sz[0] * sz[1])

def normlized(k: KernalMatrix, sz: tuple[int, int]):
	l = length(k, sz)
	print(l)
	def gen_row(i: int):
		for j in range(sz[0]):
			yield k[i][j] / l
	
	return tuple(tuple(gen_row(j)) for j in range(sz[1]))

if __name__ == "__main__":
	img_path = input("Image: ").strip().strip('"').strip("'").strip()
	img = Image.open(img_path)
	ksz = 4, 4
	k = circle_kernal(ksz, gamma=0.4)
	print(k)
	k = normlized(k, ksz)
	print(k)
	apply(img, (512, 128), Kernal(k, ksz), Thershold(0.9, 0, 1.0)).save(Path(__file__).parent.joinpath("sdf.png"))
