/* Header Files */
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

/* Defines (Directives) */
#define CTRL_KEY(k) ((k) & 0X1f)

/* Terminal Functions */
void die(const char *s);
void disableRawMode(void);
void enableRawMode(void);
char editorReadKey(void);

/* Input */
void editorProcessKeyPress(void);

/* Output */
void editorRefreshScreen(void);
void editorDrawRows(void);

/* Data */
struct editorConfig
{
	struct termios orig_termios;
};

struct editorConfig E; 

/**
 * disableRawMode - Unset raw mode
 */
void disableRawMode(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
		die("tcseattr");
}

/**
 * enableRawMode - Set the terminal to raw mode
 */
void enableRawMode(void)
{
	struct termios raw;

	if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
	atexit(disableRawMode);

	raw = E.orig_termios;
	tcgetattr(STDIN_FILENO, &raw);

	raw.c_lflag &= ~(BRKINT | ICRNL| INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");;

}

/**
 * die - Print error message
 * @s: Error message
 */
void die(const char *s)
{
	write(STDOUT_FILENO, "\x1b[2j", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	perror(s);
	exit(1);
}

/**
 * editorReadKey - Read input for STDIN
 * Return: The char read
 */
char editorReadKey(void)
{
	int nread;
	char c;

	while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
	{
		if (nread == -1 && errno != EAGAIN) die("read");
	}
	return (c);
}

/**
 * editorProcessKeyPress - Process the char read
 */
void editorProcessKeyPress(void)
{
	char c = editorReadKey();

	switch (c)
	{
		case 'q':
			write(STDOUT_FILENO, "\x1b[2j", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
	}
}

/**
 * editorRefreshScreen - Clear standard output
 */
void editorRefreshScreen(void)
{
	write(STDOUT_FILENO, "\x1b[2j", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	editorDrawRows();

	write(STDOUT_FILENO, "\x1b[H", 3);
}

/**
 * editorDrawRows - Draw rows
 */
void editorDrawRows(void)
{
	int y;

	for (y = 0; y < 24; y++)
		write(STDOUT_FILENO, "%\r\n", 3);
}

/**
 * main - A text editor
 *
 * Return: 0
 */
int main(void)
{

	enableRawMode();

	while (1)
	{
		editorRefreshScreen();
		editorProcessKeyPress();
	}
	return (0);
}
