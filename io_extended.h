#ifndef IO_EXTENDED_H_
#define IO_EXTENDED_H_

#include <iostream>
#include <Windows.h>

inline void clear_istream(std::istream& stream)
{
	while (stream.get() != '\n')
		;
}

void check_error(std::istream& stream)
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

#endif
