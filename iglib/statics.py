import os
from pathlib import Path
import string
from glassy import utils # my lib, haven't been realesed yet
from io import StringIO

__folder__ = utils.parent_path(__file__)

def averge_line_length(p: str, exclude_empty: bool = False):
  total = 0
  lines = p.splitlines()
  counter = 0
  for i in lines:
    if exclude_empty and all(k in string.whitespace for k in i):
      continue
    total += len(i)
    counter += 1
  if not counter:
    return 0.0
  return total / counter

def to_csv(p: list[list[str]], sep = ','):
  column_size = 0
  buffer = StringIO()
  for n in p[0]:
    column_size = max(len(n) + 2, column_size)
  for i in p:
    for j in i:
      jstr = str(j)
      buffer.write(jstr + sep)
    buffer.write('\n')
  return buffer.getvalue().rstrip()
  
  

def main():
  current_dir = Path(__file__).parent
  scan : list[os.DirEntry[str]] = list(os.scandir(current_dir))
  data = {}
  
  for i in scan:
    if i.is_dir():
      for j in os.scandir(i.path):
        scan.append(j)
    elif utils.extension(i.path).lower() in ("cpp", "c", "h", "hpp"):
      with open(i.path) as f:
        data[i.path] = f.read()
  
  csv_list: list[list[str]] = [["name", "type", "size", "lines", "average line length", "average line len (excluding empty)"]]
  for i in data:
    csv_list.append([
      utils.get_basename("\\".join(i.split('\\')[2:])),
      utils.extension(i),
      len(data[i]),
      data[i].count('\n'),
      round(averge_line_length(data[i], False), 2),
      round(averge_line_length(data[i], True), 2)
    ])
  with open(current_dir.joinpath("statics.csv"), 'w') as f:
    f.write(to_csv(csv_list))
  


if __name__ == "__main__":
  main()
