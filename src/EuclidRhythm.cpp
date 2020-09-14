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
	@param n is the length of the sequence (onsets)
	@param k is the number of 1s (beats or pulses)
	@param o is the offsit (shift )
	@param s is a pointer to store the resulting sequence
	returns true on succes and false on failure
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

EuclidRhythm::EuclidRhythm(uint8_t nr_of_steps, uint8_t pin)
{
	max_steps = nr_of_steps;
	my_pin = pin;
	sequence = new int[max_steps];
	for (int i = 0; i < max_steps; i++)
		sequence[i] = 0;
}

EuclidRhythm::~EuclidRhythm()
{
	delete sequence;
}

bool EuclidRhythm::set_steps(uint8_t steps)
// set nr of steps
{
	if ((steps >= 0) && (steps <= max_steps))
	{
		my_steps = steps;
		return true;
	}
	return false; // error
}

bool EuclidRhythm::set_hits(uint8_t hits)
// set nr of hits
{
	if ((hits >= 0) && (hits <= my_steps))
	{
		my_hits = hits;
		return true;
	}

	return false; // error
}

bool EuclidRhythm::set_offset(uint8_t offset)
// set offset
{
	my_offset = offset % my_steps;
	return true;
}

uint8_t EuclidRhythm::get_steps()
// get nr of steps
{
	return my_steps;
}

uint8_t EuclidRhythm::get_hits()
// get nr of hits
{

	return my_hits;
}

uint8_t EuclidRhythm::get_offset()
// get offset
{
	return my_offset;
}

void EuclidRhythm::clear_sequence()
{
	for (int i = 0; i < max_steps; i++)
		sequence[i] = 0;
}

bool EuclidRhythm::compute_sequence()
{
	bool ret = false;
	if ((my_steps == 0) || (my_hits == 0))
		clear_sequence();
	else
		ret = bresenhamEuclidean(my_steps, my_hits, my_offset, sequence); // recompute pattern
	return ret;
}

String EuclidRhythm::print_sequence()
{
	String my_string;

	for (int i = 0; i < my_steps; i++)
		my_string += String(sequence[i]) + " ";

	my_string.trim(); // trim thefinal white space off the string:

	return my_string;
}

void EuclidRhythm::inc_step()
// increment current step in sequence
{
	my_step++;
	if (my_step >= my_steps)
		my_step = 0;
}

int EuclidRhythm::get_duration()
// return duration of hit [ms] at current step
{
	return sequence[my_step];
}

void EuclidRhythm::set_duration(int value)
// set duration of hit [ms] at current step
{
	sequence[my_step] = value;
}

uint8_t EuclidRhythm::get_pin()
{
	return my_pin;
}

void EuclidRhythm::set_rem_time(int time)
{
	rem_time = time;
}

int EuclidRhythm::get_rem_time()
{
	return rem_time;
}

bool EuclidRhythm::dec_rem_time()
{
	rem_time--;
	return (rem_time > 0);
}