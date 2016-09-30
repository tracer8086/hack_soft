#pragma once
#ifndef IO_EXTENDED_H_
#define IO_EXTENDED_H_

#include <iostream>
#include <Windows.h>
#include <ctime>
#define MAX_BYTE 256
#define DELAY 2

inline void clear_istream(std::istream& stream)
{
	while (stream.get() != '\n')
		;
}

template <typename numeric> void check_error(std::istream& stream, numeric& x)
{
	while (!(stream >> x))
	{
		stream.clear();
		clear_istream(stream);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "You entered wrong value. Please enter a number: ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
}

void make_delay()
{
	clock_t delay = CLOCKS_PER_SEC * DELAY;
	clock_t current_time = clock();

	while (clock() != current_time + delay)
		;
}

#endif