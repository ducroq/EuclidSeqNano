#include <Arduino.h>

class EuclidRhythm
{
private:
    uint8_t max_steps = 0;        // max nr of steps in sequence
    uint8_t my_steps = 0;         // nr of steps in sequence
    uint8_t my_step = 0;          // current step in sequence
    uint8_t my_hits = 0;          // nr of hits in sequence
    uint8_t my_offset = 0;        // sequence offset
    uint8_t my_pin;               // output pin
    int duration = 0;             // hit duration [ms]
    int *sequence;
    int rem_time = 0;             // remaining time [ms] before hit is turned off

public:
    EuclidRhythm(){};
    EuclidRhythm(uint8_t nr_of_steps, uint8_t pin);
    ~EuclidRhythm();
    bool set_steps(uint8_t steps);
    bool set_hits(uint8_t hits);
    bool set_offset(uint8_t offset);
    uint8_t get_steps();
    uint8_t get_hits();
    uint8_t get_offset();
    void clear_sequence();
    bool compute_sequence();
    String print_sequence();
    void inc_step();
    int get_duration();
    void set_duration(int value);
    uint8_t get_pin();
    void set_rem_time(int time);
    int get_rem_time();
    bool dec_rem_time();
};
