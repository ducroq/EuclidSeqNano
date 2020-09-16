# EuclidSeqNano
Euclidean sequencer implemented on an Arduino Nano. Inspiration from various sources, see e.g. https://louridas.github.io/rwa/assignments/musical-rhythms/ and  https://medium.com/code-music-noise/euclidean-rhythms-391d879494df.

Euclidean rhythms are essentially a way of spacing out n events (onsets) across m positions (pulses or beats) as evenly possible. For simplicity, we are assuming quarter notes with 4 ticks per quarter note (tpqn), so 1 onset is 1/16th of a bar or semi-quaver. We get some interesting polyrhythms if we simultaneously play multiple Euclidean rhythms, where the number of beats in each of the rhythms are relatively prime (they share no common positive divisors except 1). That's where the EuclidSeqNano sequencer comes in.

In this example, 5 channels are implemented, mapped to 5 output pins of an Arduino Nano. Each channel output can be connected to a percussion synthesizer input.
For each channel, a sequence is generated based on the number of beats or pulses (n) in the sequence, the number of onsets (k), and the offset of the pattern (o). These  parameters are set by user with 3 rotary encoders (KY-40). To generate a sequence, Bresenham’s line algorithm is implemented, which is normaly used for drawing a line in a raster graphics environment.

User can select a channel by turning a multi-position rotary switch connected to an analogue input pin. In between the switch positions there are equal resistors (2k2) and the first position is connected to ground, while the last position is connect to the supply voltage. Changing position of the rotary switch basically puts a different fraction of the supply voltage on the analog input.

A potmeter regulates the overal tempo (bpm) of the sequence between a minimal and a maximum tempo.
Alternatively, user can connect an external clock signal providing a pulse (quarters) to lock on to.

In a 1 ms timer interrupt service routine, the 5 rhythm objects are parsed to check if the channel outputs should be switched on or off.
The general loop is used to check all the knobs and change the rhythm objects' parameters accordingly. If one of the rhythm parameters is changed, also a JSON document is generated and serialized for further saving/displaying settings on a host computer.
