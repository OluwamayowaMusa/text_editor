CC = gcc

mad: mad.c
	$(CC) -Wall -Werror -Wextra -pedantic -std=gnu89 mad.c -o mad
