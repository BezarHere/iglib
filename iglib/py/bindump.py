from pathlib import Path
from typing import BinaryIO
from itertools import batched

SHORT_UNIT_LENGTH = USHORT_UNIT_LENGTH = 2
INT_UNIT_LENGTH = UINT_UNIT_LENGTH = 4
LONG_UNIT_LENGTH = ULONG_UNIT_LENGTH = 8

def binary_dump_str(values: bytes | bytearray, unit_length = INT_UNIT_LENGTH):
	for i in batched(values, unit_length):
		yield int.from_bytes(i, signed=False)

def binary(f: BinaryIO | Path | str):
	if isinstance(f, BinaryIO):
		return f.read()
	with open(f, 'rb') as bf:
		return bf.read()

