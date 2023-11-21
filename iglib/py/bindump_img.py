from typing import Callable, TextIO
from bindump import *
from PIL import Image
import pyperclip
from io import StringIO

def _get_channels_count(img: Image.Image):
	p = img.getpixel((0, 0))
	return len(p) if isinstance(p, tuple) else 1

def _get_image_pixel_data(img: Image.Image):
	data = bytearray()
	for i in range(img.width):
		for j in range(img.height):
			p = img.getpixel((i, j))
			if isinstance(p, tuple):
				for n in p:
					data.append(n)
			else:
				data.append(p)
	return bytes(data)

def bindump(
			f: Path | str | BinaryIO | Image.Image,
			dumper: Callable | Path | str | TextIO | StringIO | None = None,
			*,
			bytes_per_unit = 1,
			max_row_length = 8,
			binary: bool = False):
	
	img = f if isinstance(f, Image.Image) else Image.open(f)
	channels = _get_channels_count(img)
	dump = _get_image_pixel_data(img)
	len_d = len(dump)
	print(channels)

	strbuild = StringIO()

	if dumper is not None:
		if isinstance(dumper, (TextIO, StringIO)):
			strbuild = dump
		elif isinstance(dumper, (str, Path)):
			strbuild = open(dumper, 'w')
	
	mapper_func = str if binary else hex

	if bytes_per_unit > 1:
		dump = batched(dump, bytes_per_unit)
		orig_mapper_func = mapper_func
		mapper_func = lambda v: orig_mapper_func(int.from_bytes(v, signed=False))
		for i in batched(dump, max_row_length):
			strbuild.write(f"{', '.join(map(mapper_func, i))},\n")
	else:
		for i in batched(dump, max_row_length):
			strbuild.write(f"{', '.join(map(mapper_func, i))},\n")

	if dumper is None:
		strbuild.seek(0)
		pyperclip.copy(strbuild.read())
	elif isinstance(dumper, Callable):
		strbuild.seek(0)
		dumper(strbuild.read())
	elif isinstance(dumper, (str, Path)):
		strbuild.close()

bindump(input("Image: ").strip().strip('"').strip("'").strip(), Path(__file__).parent.joinpath("dump.txt"), max_row_length=64)
