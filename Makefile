NAME 		= bc-tester

CC 			= cc
RM			= rm -f
CFLAGS 		+= -Wall -Wextra -O3
CLINKS		= -lm

SRC 		= bckey-tester.c\
				bc-tester.c\
				blowfish.c\
				combos.c\
				utils.c\
				main.c\
				orders.c\
				permutes.c\
				progress.c\
				separators.c\
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
