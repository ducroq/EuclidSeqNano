#include "KY040.h"

KY040::KY040(const uint8_t button_pin, const uint8_t data_pin, const uint8_t clk_pin)
{
    my_button_pin = button_pin;
    my_data_pin = data_pin;
    my_clk_pin = clk_pin;

    pinMode(my_button_pin, INPUT_PULLUP);
    pinMode(my_data_pin, INPUT);
    pinMode(my_clk_pin, INPUT);

    prev_clk = digitalRead(my_clk_pin);
}

bool KY040::read()
{
    bool ret = false;
    bool curr_clk = digitalRead(my_clk_pin);

    button = digitalRead(my_button_pin);
    if (curr_clk != prev_clk)
    // Knob is rotating, we should do some debouncing here
    {
        // Determine direction by reading pin B.
        if (digitalRead(my_data_pin) != curr_clk)
            // Rotating Clockwise, since pin A Changed first
            my_value++;
        else
            // Otherwise B changed first and we're moving CCW
            my_value--;
        prev_clk = curr_clk;
    }
    if ((my_value != prev_value) || (button != prev_button)) // give trigger also when value did not change!
        ret = true;
    if (my_value > max_value)
        my_value = max_value;
    if (my_value < min_value)
        my_value = min_value;
    prev_value = my_value;
    prev_button = button;
    return ret;
}

void KY040::set_max_value(int val)
{
    max_value = val;
    if (my_value > val)
        my_value = val;
}

void KY040::set_min_value(int val)
{
    min_value = val;
    if (my_value < val)
        my_value = val;
}

int KY040::get_value()
{
    return my_value;
}

void KY040::set_value(int val)
{

    my_value = val;
    prev_value = val;
}
