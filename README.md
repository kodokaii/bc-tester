# BC Tester

`bc-tester` is a command-line tool designed to brute-force files encrypted with bcrypt (typically with the `.bfe` extension). It tests various combinations, permutations, and variants of words to generate keys and validate them against a given encrypted file.

## Usage

```
bc-tester [-p] [-o | -O] [-v <variants>] [(-s | -S) <separators>] [-t <threadCount>] [-i <instanceNumber>/<instanceTotal>] <keysize> <keyfile> <word1> [<word2> ...]
```

### Options

- `-p`: Print the options being tested during execution.
- `-o`: Enable combinations of words.
- `-O`: Enable permutations of words.
- `-v <variants>`: Enable variants (e.g., lowercase, uppercase, capitalized). Specify the variants using characters (e.g., `l` for lowercase, `u` for uppercase, `c` for capitalized and `n` for no variation)
- `-s <separators>`: Specify separators to use between words. Tests both with and without separators.
- `-S <separators>`: Specify separators to use between words. Tests only with separators.
- `-t <threadCount>`: Number of threads to use for testing.
- `-i <instanceNumber>/<instanceTotal>`: Specify the instance number and total instances for distributed testing.
- `<keysize>`: Length of the key to generate. The minimum key size is 8 bytes.
- `<keyfile>`: Path to the encrypted file
- `<word1> [<word2> ...]`: List of words to use for generating keys.

## Examples

### Basic Example

Test the password "password" to decrypt the file `password.bfe`:

```
./bc-tester -p 8 password.bfe password
```

### Using Combinations

Generate combinations of words:

```
./bc-tester -o fract-ol.bfe 10 word1 word2
```

This will test:

```
word1word1
word2word1
word1word2
word2word2
```

### Using Permutations

Generate permutations of words:

```
./bc-tester -O fract-ol.bfe 10 word1 word2
```

This will test:

```
word2word1
word1word2
```

### Using Variants

Enable lowercase (`l`), uppercase (`u`), and capitalized (`c`) variants:

```
./bc-tester -v luc fract-ol.bfe 10 word1 word2
```

This will test:

```
word1word2
Word1word2
WORD1word2
word1Word2
Word1Word2
WORD1Word2
word1WORD2
Word1WORD2
WORD1WORD2
```

### Using Separators

Specify separators (`-`) and test both with and without separators:

```
./bc-tester -s '-' fract-ol.bfe 10 word1 word2
```

This will test:

```
word1-word2
word1word2
```

### Using Only Separators

Specify separators (`-`) and test only with separators:

```
./bc-tester -S '-' fract-ol.bfe 10 word1 word2
```

This will test:

```
word1-word2
```

### Using Threads

Run the tool with 4 threads:

```
./bc-tester -t 4 8 fract-ol.bfe hello world
```

### Distributed Testing

Run the tool as instance 2 of 3:

```
./bc-tester -i 2/3 8 fract-ol.bfe hello world
```

### Combining Permutations, Variants, and Only Separators

Generate permutations with lowercase (`l`) and uppercase (`u`) variants, using only separators (`-`):

```
./bc-tester -O -v lu -S '-' fract-ol.bfe 10 word1 word2
```

This will test:

```
word2-word1
word1-word2
word2-WORD1
WORD1-word2
WORD2-word1
word1-WORD2
WORD2-WORD1
WORD1-WORD2
```

## Notes

- The key file must have been encrypted using the bcrypt program without using the compression option.

For more details, contact me !
