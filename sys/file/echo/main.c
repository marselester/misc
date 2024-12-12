// This program reads from stdin and writes to stdout one byte at a time.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    char c;

    // read() copies at most 1 byte from the standard input to the buffer c.
    // It returns the number of bytes read (0 indicates EOF, -1 indicates an error).
    // Short counts can happen if a program is interrupted by a signal or when
    // reading from a terminal (one text line at a time), a pipe, or a network socket
    // (network delays, buffering).
    while (read(STDIN_FILENO, &c, 1) != EOF) {
        write(STDOUT_FILENO, &c, 1);
    }

    return 0;
}
