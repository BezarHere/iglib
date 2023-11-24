from dataclasses import dataclass
from itertools import product, repeat
import itertools
from typing import Any, Callable, TextIO
from bindump import *
from PIL import Image
import pyperclip
from io import StringIO

@dataclass(frozen=True, slots=True)
class padded_hex:
	size: int

	def __call__(self, value: int) -> Any:
		h = hex(value)
		if len(h) < self.size + 2:
			h = '0x' + ('0' * ((self.size + 2) - len(h))) + h[2:]
		return h

def _get_channels_count(img: Image.Image):
	p = img.getpixel((0, 0))
	return len(p) if isinstance(p, tuple) else 1

def _get_image_pixel_data(img: Image.Image, channels: int):
	data = bytearray()
	for i in range(img.height):
		for j in range(img.width):
			p = img.getpixel((j, i))
			if channels > 0:
				for n in range(channels):
					data.append(p[n])
			else:
				data.append(p)
	return bytes(data)


def demult(img):
	"""inserts a duplicate byte of each byte after it"""
	def pair(x):
		for i in x:
			yield i
			yield i
	return bytes(i for i in pair(img))

def bindump(
			f: Path | str | BinaryIO | Image.Image,
			dumper: Callable | Path | str | TextIO | StringIO | None = None,
			*,
			bytes_per_unit = 1,
			max_row_length = 8,
			pixel_size = -1,
			dump_preprocessor: Callable | None = None,
			postprocessor: Callable | None = None,
			preprocessor: Callable | None = None,
			binary: bool = False):
	
	img = f if isinstance(f, Image.Image) else Image.open(f)
	channels = pixel_size if pixel_size >= 0 else _get_channels_count(img)
	dump = dump_preprocessor(_get_image_pixel_data(img, channels))
	len_d = len(dump)

	strbuild = StringIO()

	if dumper is not None:
		if isinstance(dumper, (TextIO, StringIO)):
			strbuild = dump
		elif isinstance(dumper, (str, Path)):
			strbuild = open(dumper, 'w')
	
	mapper_func = str if binary else padded_hex(bytes_per_unit * 2)

	if postprocessor is None:
		postprocessor = lambda _: _
	if preprocessor is None:
		preprocessor = lambda _: _

	if bytes_per_unit > 1:
		dump = batched(dump, bytes_per_unit)
		orig_mapper_func = mapper_func
		mapper_func = lambda v: orig_mapper_func(int.from_bytes(v, signed=False))
		for i in batched(dump, max_row_length):
			strbuild.write(f"{', '.join(postprocessor(map(mapper_func, preprocessor(i))))},\n")
	else:
		for i in batched(dump, max_row_length):
			strbuild.write(f"{', '.join(postprocessor(map(mapper_func, preprocessor(i))))},\n")

	if dumper is None:
		strbuild.seek(0)
		pyperclip.copy(strbuild.read())
	elif isinstance(dumper, Callable):
		strbuild.seek(0)
		dumper(strbuild.read())
	elif isinstance(dumper, (str, Path)):
		strbuild.close()



if __name__ == "__main__":
	img_path = input("Image: ").strip().strip('"').strip("'").strip()
	img = Image.open(img_path)
	
	bindump(img, Path(__file__).parent.joinpath("dump.txt"), max_row_length=16, bytes_per_unit=8, pixel_size=1, dump_preprocessor=demult)
