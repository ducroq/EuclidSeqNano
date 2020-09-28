#include <Arduino.h>
#include "EuclidRhythm.h"
/*
Inspiration from various sources
See e.g. https://louridas.github.io/rwa/assignments/musical-rhythms/
*/

bool bresenhamEuclidean(int n, int k, int o, int *s)
/*
	Constructs a cyclic n-bit binary sequence with k 1s,
	such that the 1s are distributed as evenly as possible.
	@param n is the length of the sequence (beats or pulses)
	@param k is the number of 1s (onsets)
	@param o is the offsit (shift)
	@param s is a pointer to store the resulting sequence
	returns true on success and false on failure
	See: https://medium.com/code-music-noise/euclidean-rhythms-391d879494df
*/
{
	if (k > n)
		// raise ValueError
		return false;

	int *c = new int[n];
	float slope = float(k) / float(n);
	int prev_y = -1;

	for (int i = 0; i < n; i++)
	// approximate a pixelated line and mark vertical changes
	{
		int y = floor((float(i) * slope));
		c[i] = (y != prev_y) ? 1 : 0;
		prev_y = y;
	}
	for (int i = 0; i < n; i++)
		// shift the pattern to produce sequence
		s[i] = (i + o < n) ? c[i + o] : c[i + o - n];

	delete c;

	return true;
}

EuclidRhythm::EuclidRhythm(uint8_t nr_of_positions, uint8_t pin)
{
	max_positions = nr_of_positions;
	my_pin = pin;
	sequence = new int[max_positions];
	for (int i = 0; i < max_positions; i++)
		sequence[i] = 0;
}

EuclidRhythm::~EuclidRhythm()
{
	delete sequence;
}

bool EuclidRhythm::set_positions(uint8_t positions)
// set nr of positions
{
	if ((positions >= 0) && (positions <= max_positions))
	{
		my_positions = positions;
		return true;
	}
	return false; // error
}

bool EuclidRhythm::set_onsets(uint8_t onsets)
// set nr of onsets
{
	if ((onsets >= 0) && (onsets <= my_positions))
	{
		my_onsets = onsets;
		return true;
	}

	return false; // error
}

bool EuclidRhythm::set_offset(uint8_t offset)
// set offset
{
	my_offset = offset % my_positions;
	return true;
}

uint8_t EuclidRhythm::get_positions()
// get nr of positions
{
	return my_positions;
}

uint8_t EuclidRhythm::get_onsets()
// get nr of onsets
{

	return my_onsets;
}

uint8_t EuclidRhythm::get_offset()
// get offset
{
	return my_offset;
}

void EuclidRhythm::clear_sequence()
{
	for (int i = 0; i < max_positions; i++)
		sequence[i] = 0;
}

bool EuclidRhythm::compute_sequence()
{
	bool ret = false;
	if ((my_positions == 0) || (my_onsets == 0))
		clear_sequence();
	else
		ret = bresenhamEuclidean(my_positions, my_onsets, my_offset, sequence); // recompute pattern
	return ret;
}

String EuclidRhythm::print_sequence()
{
	String my_string;

	for (int i = 0; i < my_positions; i++)
		my_string += String(sequence[i]) + " ";

	my_string.trim(); // trim thefinal wonsete space off the string:

	return my_string;
}

void EuclidRhythm::inc_position()
// increment current position in sequence
{
	my_position++;
	if (my_position >= my_positions)
		my_position = 0;
}

int EuclidRhythm::get_duration()
// return duration of onset [ms]
{
	return duration;
}

void EuclidRhythm::set_duration(int value)
// set duration of onset [ms]
{
	if ((value >= 0) && (value <= 1000))
		duration = value;
}

uint8_t EuclidRhythm::get_pin()
{
	return my_pin;
}

void EuclidRhythm::set_rem_time()
// set remaining time [ms] of current event
{
	rem_time = sequence[my_position]*duration;
}

int EuclidRhythm::get_rem_time()
{
	return rem_time;
}

bool EuclidRhythm::dec_rem_time()
// decrease remaining time [ms] and return boolean to indicate if event is still active
{
	rem_time--;
	return (rem_time > 0);
}