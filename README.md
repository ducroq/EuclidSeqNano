# EuclidSeqNano
Euclidean sequencer implemented with an Arduino Nano. Inspiration from various sources, see e.g. https://louridas.github.io/rwa/assignments/musical-rhythms/ and  https://medium.com/code-music-noise/euclidean-rhythms-391d879494df.

Euclidean rhythms E(n,m) are essentially a way of spacing out n events (onsets) across m positions (pulses or beats) as evenly possible. For simplicity, we are assuming a meter with quarter notes with 4 ticks per quarter note (tpqn), so 1 onset is 1/16th of a bar or semi-quaver. We get some interesting polyrhythms if we simultaneously play multiple Euclidean rhythms with a number of beats in the rhythms that are relatively prime (they share no common positive divisors except 1).   

E.g. a simple 2-beat E(1,2) against a 3-beat E(1,3) polyrhythm:  
| x _ x _ x _ |  
| x _ _ x _ _ |  

Or a simple 3-beat E(1,3) against a 4-beat E(1,4) polyrhythm:  
| x _ _ x _ _ x _ _ x _ _ |  
| x _ _ _ x _ _ _ x _ _ _ |  

[Listen to examples](https://github.com/ducroq/EuclidSeqNano/blob/master/examples/)

The goal is to build some hardware that produces a number of Euclidean rhythms. That's where the EuclidSeqNano sequencer comes in. With just a couple of knobs, the user can build polyrhythmic sequences that can be fed to a modular synthesizer.
A sequencer front plate could look this:

![Front plate](https://github.com/ducroq/EuclidSeqNano/blob/master/circuit/front.JPG)

In this example, `nr_of_channels = 5` channels are implemented, mapped to 5 output pins of an Arduino Nano. Each channel output can be connected to a percussion synthesizer input.
```
EuclidRhythm rhythm[nr_of_channels] = {EuclidRhythm(nr_of_beats * tpqn, A0),
                                       EuclidRhythm(nr_of_beats * tpqn, A1),
                                       EuclidRhythm(nr_of_beats * tpqn, A2),
                                       EuclidRhythm(nr_of_beats * tpqn, A3),
                                       EuclidRhythm(nr_of_beats * tpqn, A4)};
```
The ```EuclidRhythm``` object contains a rhythm, it's parameters, as well as getters and setters. To generate a sequence a seperate method must be called. A sequence is generated based on the number of beats or pulses (n) in the sequence, the number of onsets (k), and the offset of the pattern (o). 
These  parameters are set by user with 3 rotary encoders (KY-40), and a sequence is only recomputed if one of the rotary encoders changes. 


To generate a sequence, Bresenham’s line algorithm is implemented, which is normaly used for drawing a line in a raster graphics environment.
```
rhythm[chan].set_positions(encoder[2].get_value()); // change nr of positions in rhythm
encoder[1].set_max_value(encoder[2].get_value());
rhythm[chan].set_onsets(encoder[1].get_value());    // change nr of onsets in rhythm
rhythm[chan].set_offset(-encoder[0].get_value()); // change pattern offset
rhythm[chan].compute_sequence();                  // generate sequence
```
User can select a channel by turning a multi-position rotary switch connected to an analogue input pin. In between the switch positions there are equal resistors (2k2) and the first position is connected to ground, while the last position is connect to the supply voltage. Changing position of the rotary switch basically puts a different fraction of the supply voltage on the analog input. Only the rhythm parameters of the active channel are influenced with the rotary encoders.
```
int val2 = (nr_of_channels * analogRead(ROT)) / 1024;
```
A potmeter regulates the overall tempo (bpm) of the sequences between a minimal and a maximum tempo.
Alternatively, user can connect an external clock signal providing a pulse (quarter notes) to lock on to (not yet implemented).
- [ ] Implement external clock synchronization

Within a 1 ms timer interrupt service routine, the rhythm objects are parsed to check if a sequence channel outputs should be activated.
```
SIGNAL(TIMER0_COMPA_vect)
{
  static int tick_count = 0; // save for tick counting
  tick_count++; // proceed

  // perform output gate logic
  for (int ch = 0; ch < nr_of_channels; ch++)
    // check if current event (onset) is still active
    if (rhythm[ch].dec_rem_time())
      digitalWrite(rhythm[ch].get_pin(), HIGH);
    else
      digitalWrite(rhythm[ch].get_pin(), LOW);

  // lock to tick
  if (tick_count >= tick_period_ms)
  // tick per quarter note has passed so check schedule
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // schedule outputs
    for (int ch = 0; ch < nr_of_channels; ch++)
    {
      rhythm[ch].set_rem_time(100 * rhythm[ch].get_duration());
      rhythm[ch].inc_position();
    }
    tick_count = 0; // reset counter
  }
}
```
The general loop is used to check all the knobs and change the rhythm objects' parameters accordingly. If one of the rhythm parameters is changed, also a JSON document is generated and serialized for further saving/displaying settings on a host computer.


The circuit can for example be wired like this

![Schematic](https://github.com/ducroq/EuclidSeqNano/blob/master/circuit/Schematic_EuclidSeqNano_2020-09-19_20-01-37.png)

And a realization

![Hardware](https://github.com/ducroq/EuclidSeqNano/blob/master/circuit/back.JPG)

