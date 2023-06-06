#pragma once
#include <iostream>
#include <iomanip>

class ProgressBar
{
public:
	inline ~ProgressBar();
	inline ProgressBar(size_t _max_units, unsigned int bar_length, char empty_ch = ' ', char filled_ch = '#', char left_border = '[', char right_border = ']');
	inline ProgressBar(const ProgressBar& other);
	inline void display() const;
	inline void update(size_t new_unit_sum);
	inline void add(size_t added_unit_sum);
	inline void restart();
	inline void fill();
private:
	char* bar;
	float precentage;
	const size_t max_units;
	const int length;
	size_t gained_units;
	const char left, right, empty, filled;
};

ProgressBar::ProgressBar(const ProgressBar& other)
	:max_units(other.max_units), gained_units(other.gained_units), length(other.length),
	precentage(other.precentage), bar(new char[other.length + 3]),
	empty(other.empty), filled(other.filled), right(other.right), left(other.left)
{
	for (int i = 0; i < length + 3; ++i)
	{
		bar[i] = other.bar[i];
	}
}

void ProgressBar::add(size_t added_unit_sum)
{
	update(gained_units + added_unit_sum);
}

void ProgressBar::fill()
{
	for (int i = 0; i < length; ++i)
	{
		bar[i + 1] = filled;
	}
	precentage = 100.0f;
	gained_units = max_units;
}

void ProgressBar::restart()
{
	for (int i = 0; i < length; ++i)
	{
		bar[i + 1] = empty;
	}
	precentage = 0.0f;
	gained_units = 0;
}

ProgressBar::~ProgressBar()
{
	delete[] bar;
	bar = nullptr;
}

ProgressBar::ProgressBar(size_t _max_units, unsigned int bar_length, char empty_ch, char filled_ch, char left_border, char right_border)
	:max_units(_max_units), gained_units(0), length(bar_length < 1 ? 1 : bar_length),
	precentage(0.0f), bar(new char[bar_length + 3]),
	empty(empty_ch), filled(filled_ch), right(right_border), left(left_border)
{
	bar[0] = left;
	for (int i = 0; i < length; ++i)
	{
		bar[i + 1] = empty;
	}
	bar[length + 1] = right;
	bar[length + 2] = '\0';
}

void ProgressBar::display() const
{
	std::cout << '\r' << bar << " " 
		<< std::setprecision(precentage >= 10.0f ? 3 : 2) 
		<< precentage << " %  " << std::flush;
}

void ProgressBar::update(size_t new_unit_sum)
{
	gained_units = new_unit_sum;
	float filled_ratio = float(gained_units) / max_units;
	if (filled_ratio > 1.0f) filled_ratio = 1.0f;
	int filled_chars = int(length * filled_ratio);
	for (int i = 0; i < filled_chars; ++i)
	{
		bar[i + 1] = filled;
	}
	precentage = filled_ratio * 100.0f;
}