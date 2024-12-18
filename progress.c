#include "bc-tester.h"

static void _calculateEstimateTime(BCT_t *bct)
{
	if (bct->ctx.oldEstimate + bct->ctx.estimateStep <= bct->ctx.tested || bct->ctx.tested == bct->ctx.instance.total)
	{
		double elapsed = difftime(time(NULL), bct->ctx.startTime);
		bct->ctx.testPerSecond = (double)bct->ctx.tested / elapsed;
		bct->ctx.oldEstimate = bct->ctx.tested;
	}
}

static void _estimateTime(BCT_t *bct)
{
	_calculateEstimateTime(bct);

	size_t remaining = (double)(bct->ctx.instance.total - bct->ctx.tested) / bct->ctx.testPerSecond;
	static char estimate[42];

	if (remaining < 86400000)
	{
		snprintf(estimate, sizeof(estimate), "Remaining: %03ldd %02ldh %02ldm %02lds ", remaining / 86400, (remaining % 86400) / 3600, (remaining % 3600) / 60, remaining % 60);
		write(STDOUT_FILENO, estimate, 31);
	}
	else
	{
		snprintf(estimate, sizeof(estimate), "Remaining: The heat death of the universe");
		write(0, estimate, 41);
	}
}

static void _percent(size_t total, size_t current)
{
	static char percent[10] = {'0', '0', '0', '.', '0', '0', '%', ' ', '|', ' '};
	size_t perTenThousand = (10000 * current) / total;
	if (10000 < perTenThousand)
		perTenThousand = 10000;

	percent[0] = (perTenThousand / 10000) % 10 + '0';
	percent[1] = (perTenThousand / 1000) % 10 + '0';
	percent[2] = (perTenThousand / 100) % 10 + '0';
	percent[4] = (perTenThousand / 10) % 10 + '0';
	percent[5] = perTenThousand % 10 + '0';
	write(0, percent, sizeof(percent));
}

static void _progressBar(size_t total, size_t current)
{
	size_t perfifty = (50 * current) / total;
	if (50 < perfifty)
		perfifty = 50;

	write(STDIN_FILENO, "\r\e[J[\e[38;2;163;255;149;1m", 26);
	write(STDIN_FILENO, "##################################################", perfifty);
	write(STDIN_FILENO, "                                                  ", 50 - perfifty);
	write(STDIN_FILENO, "\e[0m] ", 6);
}

void progress(BCT_t *bct, char *key, int threadIndex)
{
	static pthread_mutex_t printf_lock = PTHREAD_MUTEX_INITIALIZER;

	++bct->ctx.tested;
	if (bct->data->opt.print)
	{
		pthread_mutex_lock(&printf_lock);
		printf("%s\n", key);
		pthread_mutex_unlock(&printf_lock);
	}
	else if (threadIndex == 0 && (bct->ctx.oldProgress + bct->ctx.progressStep <= bct->ctx.tested || bct->ctx.tested == bct->ctx.instance.total))
	{
		_progressBar(bct->ctx.instance.total, bct->ctx.tested);
		_percent(bct->ctx.instance.total, bct->ctx.tested);
		_estimateTime(bct);
		bct->ctx.oldProgress = bct->ctx.tested;
	}
}
