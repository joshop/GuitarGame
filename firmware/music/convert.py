import sys
import re
arg = sys.argv[1]
lines = [x for x in open(arg).read().split('\n') if x]
"""
Note format:
start, end, channel
"""
outs = "#include \"music.h\"\nconst Note song[] = {\n";
def cvt(x):
    return round(x * 60)
minval = 1e100
for l in lines:
    minval = min(minval, float(l.split(',')[0]))
offset = minval - 2.5
ratio = 0.41 / 0.4
for l in lines:
    parts = list(map(float, l.split(',')))
    start = cvt(parts[0]/ratio- offset)
    chan = round(parts[1])
    end = cvt(parts[2]/ratio - offset if parts[2] != 0 else 0)
    outs += "\t{%d, %d, %x},\n" % (start,end,chan);
outs += "};";
open('.'.join(arg.split(".")[:-1]) + ".h", 'w').write(outs)
