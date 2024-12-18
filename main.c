#include "main.h"

static void _initBCTData(BCT_Data_t *data)
{
	data->opt.print = false;
	data->opt.variants = false;
	data->opt.combos = false;
	data->opt.permutes = false;

	data->separatorCount = DEFAULT_SEPARATOR_COUNT;
	data->separators = DEFAULT_SEPARATOR;
	data->variantCount = 0;
	memset(data->variants, false, sizeof(data->variants));
	data->threadCount = 1;
	data->instanceTotal = 1;
	data->instanceIndex = 0;
}

static void _printUsage(void)
{
	dprintf(STDERR_FILENO, "Usage: %s [-p] [-o | -O] [-v <variants>] [(-s | -S) <separators>] [-t <threadCount>] [-i <instanceNumber>/<instanceTotal>] <keysize> <keyfile> <word1> [<word2> ...]\n", program_invocation_short_name);
}

static int _parseVariant(BCT_Data_t *data, char *variants)
{
	int i;
	for (i = 0; variants[i]; i++)
	{
		char *v = strchr(VARIANT_CHARS, variants[i]);
		if (v == NULL)
			return (warnx("Invalid variant: %c", variants[i]), EXIT_FAILURE);
		int index = v - VARIANT_CHARS;
		if (!data->variants[index])
		{
			data->variants[index] = true;
			data->variantCount++;
		}
	}
	if (i == 0)
		return (warnx("No variant specified"), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static int _parseThread(BCT_Data_t *data, char *threadCountArg)
{
	char *endptr;
	long threadCount = strtol(threadCountArg, &endptr, 10);
	if (endptr == threadCountArg || *endptr != '\0' || data->threadCount < 1 || INT_MAX < threadCount)
		return (warnx("Invalid thread count: %s", threadCountArg), EXIT_FAILURE);
	data->threadCount = threadCount;
	return (EXIT_SUCCESS);
}

static int _parseInstance(BCT_Data_t *data, char *instanceArg)
{
	char *endptr;
	long instanceNumber = strtol(instanceArg, &endptr, 10);
	if (endptr == instanceArg || *endptr != '/' || instanceNumber < 1)
		return (warnx("Invalid instance number: %s", instanceArg), EXIT_FAILURE);
	long instanceTotal = strtol(endptr + 1, &endptr, 10);
	if (endptr == instanceArg || *endptr != '\0' || instanceTotal < instanceNumber)
		return (warnx("Invalid instance total: %s", instanceArg), EXIT_FAILURE);
	data->instanceIndex = instanceNumber - 1;
	data->instanceTotal = instanceTotal;
	return (EXIT_SUCCESS);
}

static int _parseOptions(BCT_Data_t *data, int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "poOv:s:S:t:i:")) != -1)
	{
		switch (opt)
		{
		case 'p':
			data->opt.print = true;
			break;
		case 'o':
			if (data->opt.permutes)
				return (warnx("Cannot use -o and -O together"), EXIT_FAILURE);
			data->opt.combos = true;
			break;
		case 'O':
			if (data->opt.combos)
				return (warnx("Cannot use -o and -O together"), EXIT_FAILURE);
			data->opt.permutes = true;
			break;
		case 'v':
			if (_parseVariant(data, optarg))
				return (EXIT_FAILURE);
			data->opt.variants = true;
			break;
		case 's':
			data->separatorCount = strlen(optarg) + 1;
			data->separators = optarg;
			break;
		case 'S':
			data->separatorCount = strlen(optarg);
			if (data->separatorCount == 0)
				return (warnx("Cannot use empty separators with -S"), EXIT_FAILURE);
			data->separators = optarg;
			break;
		case 't':
			if (_parseThread(data, optarg))
				return (EXIT_FAILURE);
			break;
		case 'i':
			if (_parseInstance(data, optarg))
				return (EXIT_FAILURE);
			break;
		default:
			_printUsage();
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

static int _parseKeysize(BCT_Data_t *data, char *keysizeArg)
{
	char *endptr;
	size_t keysize = strtoull(keysizeArg, &endptr, 10);
	if (endptr == keysizeArg || *endptr != '\0' || errno == ERANGE)
		return (warnx("Invalid keysize: %s", keysizeArg), EXIT_FAILURE);
	if (keysize < BLOCKSIZE)
		return (warnx("Key must be at least 8 bytes long"), EXIT_FAILURE);
	data->keysize = keysize;
	return (EXIT_SUCCESS);
}

static void _sysError(char const *filename, int fd)
{
	warn("%s", filename);
	close(fd);
}

static void _fileError(char const *filename, char *error, int fd)
{
	warnx("%s: %s", filename, error);
	close(fd);
}

static uint8_t *_readKeyFile(char const *filename)
{
	static uint8_t fileKey[MAXKEYBYTES + BLOCKSIZE];
	uint8_t flags[FLAG_COUNT];
	ssize_t readBytes;

	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (_sysError(filename, fd), NULL);

	readBytes = read(fd, flags, sizeof(flags));
	if (readBytes == -1)
		return (_sysError(filename, fd), NULL);
	if (readBytes != sizeof(flags))
		return (_fileError(filename, "File is too short", fd), NULL);
	if (flags[ENDIAN_FLAGS] != LITTLE_ENDIAN_FLAG)
		return (_fileError(filename, "File is not little-endian", fd), NULL);
	if (flags[COMPRESSED_FLAG] == 1)
		return (_fileError(filename, "File is compressed", fd), NULL);

	lseek(fd, -sizeof(fileKey), SEEK_END);

	readBytes = read(fd, fileKey, sizeof(fileKey));
	if (readBytes == -1)
		return (_sysError(filename, fd), NULL);
	if (readBytes != sizeof(fileKey))
		return (_fileError(filename, "File is too short", fd), NULL);

	close(fd);
	return (fileKey);
}

static void _getWords(BCT_Word_t **words, int wordCount, char *argv[])
{
	for (int i = 0; i < wordCount; i++)
	{
		words[i] = malloc(sizeof(BCT_Word_t));
		words[i]->str = argv[i];
		words[i]->len = strlen(argv[i]);
	}
}

static void _freeWords(BCT_Word_t **words, int wordCount)
{
	for (int i = 0; i < wordCount; i++)
		free(words[i]);
	free(words);
}

static int _parseArgs(BCT_Data_t *data, int argc, char *argv[], char **key, BCT_Word_t ***words, int *wordCount)
{
	if (argc < optind + 3)
	{
		_printUsage();
		return (EXIT_FAILURE);
	}
	if (_parseKeysize(data, argv[optind + 1]))
		return (EXIT_FAILURE);
	data->fileKey = _readKeyFile(argv[optind]);
	if (data->fileKey == NULL)
		return (EXIT_FAILURE);
	*key = malloc(data->keysize + 1);
	*wordCount = argc - optind - 2;
	*words = malloc(*wordCount * sizeof(BCT_Word_t *));
	_getWords(*words, *wordCount, argv + optind + 2);
	return (EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	BCT_Data_t data;
	char *key;
	BCT_Word_t **words;
	int wordCount;

	_initBCTData(&data);
	if (_parseOptions(&data, argc, argv) || _parseArgs(&data, argc, argv, &key, &words, &wordCount))
		return (EXIT_FAILURE);
	if (BCTester(&data, key, words, wordCount))
	{
		printf(GREEN "SUCCESS:" RESET " %s\n", key);
		_freeWords(words, wordCount);
		free(key);
		return (EXIT_SUCCESS);
	}
	else
	{
		printf(RED "FAILURE\n" RESET);
		_freeWords(words, wordCount);
		free(key);
		return (EXIT_FAILURE);
	}
}