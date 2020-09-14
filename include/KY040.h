#include <Arduino.h>

// https://github.com/brianlow/Rotary/
// maybe this approach is better?

class KY040
{
private:
    uint8_t my_button_pin;
    uint8_t my_data_pin;
    uint8_t my_clk_pin;
    int my_value = 0;
    bool button = false;
    int max_value = 255;
    int min_value = 0;
    bool prev_clk = false;
    bool prev_button = false;
    int prev_value = 0;

public:
    KY040(const uint8_t button_pin, const uint8_t data_pin, const uint8_t clk_pin);
    ~KY040(){};
    bool read();
    void set_max_value(int val);
    void set_min_value(int val);
    int get_value();
    void set_value(int val);
};
