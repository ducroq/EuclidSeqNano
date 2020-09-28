#include <Arduino.h>

class EuclidRhythm
{
private:
    uint8_t max_positions = 0;    // max nr of positions in sequence
    uint8_t my_positions = 0;     // nr of positions in sequence
    uint8_t my_position = 0;      // current position in sequence
    uint8_t my_onsets = 0;        // nr of onsets in sequence
    uint8_t my_offset = 0;        // sequence offset
    uint8_t my_pin;               // output pin
    int duration = 10;            // onset duration [ms]
    int *sequence;
    int rem_time = 0;             // remaining time [ms] before onset is turned off

public:
    EuclidRhythm(){};
    EuclidRhythm(uint8_t nr_of_positions, uint8_t pin);
    ~EuclidRhythm();
    bool set_positions(uint8_t positions);
    bool set_onsets(uint8_t onsets);
    bool set_offset(uint8_t offset);
    uint8_t get_positions();
    uint8_t get_onsets();
    uint8_t get_offset();
    void clear_sequence();
    bool compute_sequence();
    String print_sequence();
    void inc_position();
    int get_duration();
    void set_duration(int value);
    uint8_t get_pin();
    void set_rem_time();//int time);
    int get_rem_time();
    bool dec_rem_time();
};
