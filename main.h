#pragma once

#include "bc-tester.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

#define DEFAULT_SEPARATOR_COUNT 1
#define DEFAULT_SEPARATOR ""

#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"

extern char *optarg;
extern int optind;
extern char *program_invocation_short_name;
