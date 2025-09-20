import mido
basename = "tvtime_full"
mid_beats = mido.MidiFile(basename+"-Piano_2.mid")
mid_left = mido.MidiFile(basename+"-Square_Synthesizer_1.mid")
mid_right = mido.MidiFile(basename+"-Square_Synthesizer_2.mid")
"""
tempo=400000
basepitch = 27.5
basenote = 21
tstep = 0
channels = [None] * 10
allnotes = []
print(mid.ticks_per_beat)
for msg in mid:
    if msg.type == "note_on":
        nn = msg.note
        #freq = 2**((nn - basenote) / 12) * basepitch
        print(msg.time*60)
        tstep += msg.time*60

        if msg.velocity == 0:
            i = 0
            while i < len(channels):
                if channels[i] is not None and channels[i][0] == nn: break
                i += 1
            st = channels[i][1]
            allnotes.append((i, 2**((nn - basenote) / 12) * basepitch, st, tstep))
            #channels = [c for c in channels if c[0] != nn]
            channels[i] = None
        else:
            i = 0
            while i < len(channels):
                if channels[i] is None:
                    channels[i] = (nn, tstep)
                    break
                i += 1
            # channels.append((nn, tstep))
ls = 0
for note in allnotes:
    if note[0] != 0: continue
    #print("%2d %8.2f %8.2f %8.2f" % note)
    if round(note[2]) != ls: print("{0, %d}, " % (round(note[2]) - ls),end='')
    print("{%f, %d}, " % (note[1], round(note[3]) - round(note[2])),end='')
    ls = round(note[3])
"""
basepitch = 27.5
basenote = 21
tstep = 0
fname = ["tvtime_left", "tvtime_right"]
outf = open("its_tv_time.h", "w")
outf.write("#pragma once\n#include \"music.h\"\n")
a = True
for mid in [mid_left, mid_right]:
    allnotes = []
    outs = ""
    st = 0
    tstep = 0
    for msg in mid:
        if msg.type == "note_on":
            nn = msg.note
            tstep += msg.time*60
            if msg.velocity == 0:
                allnotes.append((2**((nn - basenote) / 12) * basepitch, st, tstep))
            else:
                st = tstep
    ls = 0
    for note in allnotes:
        if round(note[1]) != ls:
            outs += "{0, %d}, " % (round(note[1]) - ls)
        outs += "{%f, %d}, " % (note[0], round(note[2]) - round(note[1]))
        ls = round(note[2])
    outf.write("const AudioNote " + fname[0] + "[] = {" + outs + "};\n")
    fname = fname[1:]
    a = False
tstep = 2.5*60
stimes = [0,0]
outs = ""
nnotes = 0
for msg in mid_beats:
    if msg.type == "note_on":
        idx = [69,74].index(msg.note)
        tstep += msg.time*60
        if msg.velocity == 0:
            outs += "{%d,%d,%d}, " % (stimes[idx], tstep if tstep-stimes[idx] > 0.21*60 else 0, idx)
            nnotes += 1
        else:
            stimes[idx] = tstep
print(nnotes)
outf.write("const Note tvtime_beats[] = {" + outs + "};\n")
outf.close()
