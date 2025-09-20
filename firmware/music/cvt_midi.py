import sys
import re
import mido
arg = sys.argv[1]
lines = [x for x in open(arg).read().split('\n') if x]
base_dur = 0.4/8
out_mid = mido.MidiFile()
out_mid.ticks_per_beat = 480
track = mido.MidiTrack()
out_mid.tracks.append(track)
tempo = 400000
mnotes = [40, 50]
lend = 0
lchan = 0
# 480 ticks/beat
# 400000 us/beat
# 0.0012 ticks/us
# 1200 ticks/s
def snap(x):
    return round(x * 1200)
track.append(mido.MetaMessage('set_tempo', tempo=tempo))
track.append(mido.MetaMessage('time_signature', numerator=4,denominator=4))
for l in lines:
    parts = list(map(float, l.split(',')))
    start = parts[0]
    chan = round(parts[1])
    end = parts[2]
    if end == 0: end = start + base_dur
    track.append(mido.Message('note_on', note=mnotes[chan], time=snap(start-lend),velocity=127))
    track.append(mido.Message('note_off', note=mnotes[chan], time=snap(end-start), velocity=127))
    lend=end
    lchan = chan
out_mid.save('its_tv_time_cvt.mid')
