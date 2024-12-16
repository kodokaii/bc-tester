#include "bc-tester.h"

static void _printPercent(size_t total, size_t current)
{
	size_t perHundredThousand = (100000 * current) / total;
	static char percent[9] = {'0', '0', '0', '.', '0', '0', '0', '%', ' '};

	percent[0] = (perHundredThousand / 100000) % 10 + '0';
	percent[1] = (perHundredThousand / 10000) % 10 + '0';
	percent[2] = (perHundredThousand / 1000) % 10 + '0';
	percent[4] = (perHundredThousand / 100) % 10 + '0';
	percent[5] = (perHundredThousand / 10) % 10 + '0';
	percent[6] = perHundredThousand % 10 + '0';
	write(0, percent, sizeof(percent));
}

static void _progressBar(size_t total, size_t current)
{
	size_t perfifty = (50 * current) / total;

	write(STDIN_FILENO, "\r\e[J[\e[38;2;163;255;149;1m", 26);
	write(STDIN_FILENO, "##################################################", perfifty);
	write(STDIN_FILENO, "                                                  ", 50 - perfifty);
	write(STDIN_FILENO, "\e[0m] ", 6);
}

void progress(BCT_t *bct)
{
	++bct->ctx.instance.tested;
	if (bct->data->opt.print)
		printf("%s\n", bct->data->key);
	else if (bct->ctx.oldStep + bct->ctx.progressStep <= bct->ctx.instance.tested || bct->ctx.instance.tested == bct->ctx.instance.total)
	{
		_progressBar(bct->ctx.instance.total, bct->ctx.instance.tested);
		_printPercent(bct->ctx.instance.total, bct->ctx.instance.tested);
		bct->ctx.oldStep = bct->ctx.instance.tested;
	}
}
