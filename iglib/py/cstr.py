from typing import Iterable


def c_escape(s: str):
	return s.replace('\n', '\\n').replace('\t', '\\t').replace('\r', '\\r')

def c_join(s: Iterable[str], delimter: str = ' '):
	return f"\"{f'"{delimter}"'.join(s)}\""

def to_c_paragraphs(s: Iterable[str]):
	return c_join((c_escape(i) for i in s), '\n')
