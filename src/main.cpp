#include <Arduino.h>
#include <ArduinoJson.h>
#include "EuclidRhythm.h"
#include "KY040.h"
//
// todo: plus/minus button to change duration of current position on channel
//  attach interrupt to external clock pin, and override master clk
//

// some pin defines, rest is below
#define CLK A5 // external clock analog input
#define POT A6 // potentiometer analog input
#define ROT A7 // rotary switch analog input

// constants
const int nr_of_channels = 5;
const int nr_of_beats = 4; // 4/4 measure, assuming quarter notes
const int tpqn = 4;        // ticks per quarter note (tpqn), so 1 tick is 1/16th of a bar or Semi Quaver.
const int min_tempo = 10;  // [bpm]
const int max_tempo = 250; // [bpm]

// global variables, also used by timer interrupt vector
EuclidRhythm rhythm[nr_of_channels] = {EuclidRhythm(nr_of_beats * tpqn, A0),
                                       EuclidRhythm(nr_of_beats * tpqn, A1),
                                       EuclidRhythm(nr_of_beats * tpqn, A2),
                                       EuclidRhythm(nr_of_beats * tpqn, A3),
                                       EuclidRhythm(nr_of_beats * tpqn, A4)};
float tick_period_ms;

// timer 0 interrupt vector is called once per millisecond
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

void setup()
{
  // set the output pins
  pinMode(LED_BUILTIN, OUTPUT);   // built in LED on GPIO2
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off (HIGH is the voltage level)
                                  // pinMode(BUT_1 , INPUT_PULLUP); // duration up
                                  // pinMode(BUT_2 , INPUT_PULLUP); // duration down
  for (int ch = 0; ch < nr_of_channels; ch++)
  {
    digitalWrite(rhythm[ch].get_pin(), LOW);
    pinMode(rhythm[ch].get_pin(), OUTPUT);
  }

  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the Compare A vector
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  // We'll send Json over serial to show settings
  Serial.begin(115200);
  Serial.println("Simple drum sequencer");
  Serial.println("start...");
}

void loop()
// all about reading knobs...
{
  const float alpha = .01;        // smoothing factor for analog readings
  static int chan = 0, tempo = 0; // save values
  static KY040 encoder[3] = {KY040(4, 5, 6),
                             KY040(7, 8, 9),
                             KY040(10, 11, 12)};

  // read potmeter
  float val1 = map(analogRead(POT), 0, 1023, min_tempo, max_tempo);

  // read rotary switch
  int val2 = (nr_of_channels * analogRead(ROT)) / 1024;

  // update tempo
  tempo = (alpha * tempo + (1.0 - alpha) * val1);
  if (abs(tempo - val1) > 0.01)
    // tempo has changed more than 1%
    tick_period_ms = 60000 / (tpqn * tempo); // tick period [ms]

  // check if rotary switch has changed
  if (chan != val2)
  // reload encoder settings
  {
    delay(10); // wait a bit for bounce to settle
    chan = (nr_of_channels * analogRead(ROT)) / 1024;
    encoder[0].set_value(rhythm[chan].get_offset());
    encoder[1].set_value(rhythm[chan].get_onsets());
    encoder[2].set_value(rhythm[chan].get_positions());
    // Serial.println(chan);
  }

  // check rotary encoders
  if (encoder[0].read() || encoder[1].read() || encoder[2].read())
  // adapt rhythm parameters
  {
    rhythm[chan].set_positions(encoder[2].get_value()); // change nr of positions in rhythm
    encoder[1].set_max_value(encoder[2].get_value());
    rhythm[chan].set_onsets(encoder[1].get_value());    // change nr of onsets in rhythm
    rhythm[chan].set_offset(-encoder[0].get_value()); // change pattern offset
    rhythm[chan].compute_sequence();                  // generate sequence

    // generate and serialize a JSON document, for saving/displaying settings
    StaticJsonDocument<nr_of_channels * 100 + 100> doc; // Use arduinojson.org/v6/assistant to compute the RAM size.
    doc["time"] = millis();
    doc["tempo"] = tempo;
    for (int ch = 0; ch < nr_of_channels; ch++)
    {
      JsonObject nested1 = doc.createNestedObject("channel " + String(ch));
      nested1["sequence"] = rhythm[ch].print_sequence();
    }
    serializeJson(doc, Serial);
    Serial.println();
  }
}