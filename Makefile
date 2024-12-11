NAME 		= bc-tester

CC 			= cc
RM			= rm -f
CFLAGS 		+= -Wall -Wextra -g #-O3
CLINKS		= -lm

SRC 		= bckey-tester.c\
				bc-tester.c\
				blowfish.c\
				combo.c\
				main.c\
				order.c\
				permute.c\
				progress.c\
				separator.c\
				variants.c

OBJ 		= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(CLINKS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY:		all clean fclean re
