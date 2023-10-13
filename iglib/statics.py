import os
from glassy import utils # my lib, haven't been realesed yet
from io import StringIO

__folder__ = utils.parent_path(__file__)

def averge_line_length(p: str):
    total = 0
    lines = p.splitlines()
    for i in lines:
        total += len(i)
    return total / len(lines)

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
    scan : list[os.DirEntry[str]] = list(os.scandir())
    data = {}
    for i in scan:
        if i.is_dir():
            for j in os.scandir(i.path):
                scan.append(j)
        elif utils.extension(i.path).lower() in ("cpp", "c", "h", "hpp"):
            with open(i.path) as f:
                data[i.path] = f.read()
    csv_list: list[list[str]] = [["name", "type", "size", "lines", "averge line length"]]
    for i in data:
        csv_list.append([
            utils.get_basename("\\".join(i.split('\\')[2:])),
            utils.extension(i),
            len(data[i]),
            data[i].count('\n'),
            round(averge_line_length(data[i]), 1)
        ])
    with open("statics.csv", 'w') as f:
        f.write(to_csv(csv_list))
    


if __name__ == "__main__":
    main()
