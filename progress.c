#include "progress.h"

static void _printProgress(size_t pertenthousand)
{
	static char str[8] = {'0', '0', '0', '.', '0', '0', '%', ' '};

	str[0] = (pertenthousand / 10000) % 10 + '0';
	str[1] = (pertenthousand / 1000) % 10 + '0';
	str[2] = (pertenthousand / 100) % 10 + '0';
	str[4] = (pertenthousand / 10) % 10 + '0';
	str[5] = pertenthousand % 10 + '0';
	write(0, str, sizeof(str));
}

void progress_bar(size_t total, size_t current)
{
	size_t pertenthousand = (10000 * current) / total;

	write(STDIN_FILENO, "\r\e[J[\e[38;2;163;255;149;1m", 26);
	write(STDIN_FILENO, "####################################################################################################", pertenthousand / 100);
	write(STDIN_FILENO, "                                                                                                    ", 100 - pertenthousand / 100);
	write(STDIN_FILENO, "\e[0m] ", 6);
	_printProgress(pertenthousand);
}