// This program reads a number from the stdin and sends it to the server.
// The server replies whether the number is even or odd.
// gcc -o client -I./sys/socket -I./sys/file ./sys/socket/client/main.c && ./client 0.0.0.0 3000
#include <stdio.h>
#include <stdlib.h>
#include "cs-socket.h"
#include "cs-io.h"

// The maximum number of characters a user can enter.
#define MAX_LINE_SIZE 6

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 0;
    }

    char *host = argv[1];
    char *port = argv[2];
    int client_fd = open_client_fd(host, port);
    if (client_fd < 0) {
        return -1;
    }

    fprintf(stdout, "connected to server\n");

    rio_t r = {
        .fd = client_fd,
        .unread_cnt = 0,
    };
    r.next = r.buf;

    char buf[MAX_LINE_SIZE];
    ssize_t nwritten, nread;
    // Read a line from the stdin until EOF (user typed Ctrl+D or
    // text lines were exhausted in a redirected input file).
    // We expect a user to enter a number to send it to the server.
    while (fgets(buf, MAX_LINE_SIZE, stdin) != NULL) {
        nwritten = rio_writen(client_fd, buf, strlen(buf));
        if (nwritten < 0) {
            perror("send number to server");

            return -1;
        }

        nread = rio_readline(&r, buf, MAX_LINE_SIZE);
        if (nread < 0) {
            perror("receive response from server");

            return -1;
        }

        // Print the server response.
        rio_writen(STDOUT_FILENO, buf, nread);
    }

    // Closing the descriptor results in the server receiving EOF on read.
    close(client_fd);

    return 0;
}
