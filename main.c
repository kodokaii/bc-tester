#include "main.h"

static void _initBCT(BCT_t *bct)
{
	bct->opt.combo = false;
	bct->opt.permute = false;
	bct->opt.variant = false;

	bct->separator = DEFAULT_SEPARATOR;
}

static void _printUsage(void)
{
	dprintf(STDERR_FILENO, "Usage: %s [-p] [-o | -O] [-v <variants>] [-s <separator>] <encypted-file> <keysize> <word1> [<word2> ...]\n", program_invocation_short_name);
}

static int _parseOptions(BCT_t *bct, int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "poOv:s:")) != -1)
	{
		switch (opt)
		{
		case 'p':
			bct->opt.print = true;
			break;
		case 'o':
			if (bct->opt.permute)
				return (warnx("Cannot use -o and -O together"), EXIT_FAILURE);
			bct->opt.combo = true;
			break;
		case 'O':
			if (bct->opt.combo)
				return (warnx("Cannot use -o and -O together"), EXIT_FAILURE);
			bct->opt.permute = true;
			break;
		case 'v':
			bct->opt.variant = true;
			int i;
			for (i = 0; optarg[i]; i++)
			{
				char *variant = strchr(VARIANT_CHARS, optarg[i]);
				if (variant == NULL)
					return (warnx("Invalid variant: %c", optarg[i]), EXIT_FAILURE);
				bct->variants[variant - VARIANT_CHARS] = true;
			}
			if (i == 0)
				return (warnx("No variant specified"), EXIT_FAILURE);
			break;
		case 's':
			bct->separator = optarg;
			break;
		default:
			_printUsage();
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

static int _parseKeySize(char *keySize, size_t *keyLen)
{
	char *endptr;
	*keyLen = strtoull(keySize, &endptr, 10);
	if (*endptr != '\0')
		return (warnx("Invalid keysize: %s", keySize), EXIT_FAILURE);
	if (*keyLen < BLOCKSIZE)
		return (warnx("Key must be at least 8 bytes long"), EXIT_FAILURE);
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

static int _parseArgs(BCT_t *bct, int argc, char *argv[], BCT_Word_t ***words, int *wordCount)
{
	if (argc < optind + 3)
	{
		_printUsage();
		return (EXIT_FAILURE);
	}
	if (_parseKeySize(argv[optind + 1], &bct->keysize))
		return (EXIT_FAILURE);
	bct->key = malloc(bct->keysize + 1);
	bct->fileKey = _readKeyFile(argv[optind]);
	if (bct->fileKey == NULL)
		return (EXIT_FAILURE);
	*wordCount = argc - optind - 2;
	*words = malloc(*wordCount * sizeof(BCT_Word_t *));
	_getWords(*words, *wordCount, argv + optind + 2);
	return (EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	static BCT_t bct;
	BCT_Word_t **words;
	int wordCount;

	_initBCT(&bct);
	if (_parseOptions(&bct, argc, argv) || _parseArgs(&bct, argc, argv, &words, &wordCount))
		return (EXIT_FAILURE);
	if (BCTester(&bct, words, wordCount))
	{
		printf(GREEN "SUCCESS:" RESET " %s\n", bct.key);
		_freeWords(words, wordCount);
		free(bct.key);
		return (EXIT_SUCCESS);
	}
	else
	{
		printf(RED "FAILURE\n" RESET);
		_freeWords(words, wordCount);
		free(bct.key);
		return (EXIT_FAILURE);
	}
}