// This iterative server listens on port 3000 and replies whether a number is even or odd.
// gcc -o iter-server -I./sys/socket -I./sys/file ./sys/socket/iter-server/main.c && ./iter-server
#include <stdlib.h>
#include "cs-socket.h"
#include "cs-io.h"

void reply(int conn_fd);

int main() {
    char *port = "3000";
    int listen_fd = open_listen_fd(port);
    if (listen_fd < 0) {
        return -1;
    }

    fprintf(stdout, "listening on port %s\n", port);

    // IP socket address structure of the client.
    // We could have also used a protocol-independent sockaddr_storage structure.
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (1) {
        // Wait for a connection request to arrive on the listening descriptor,
        // and fill in the client_addr with the client's IP address and port.
        int conn_fd = accept(
            listen_fd,
            (struct sockaddr *)&client_addr,
            &client_addr_len
        );
        if (conn_fd < 0) {
            perror("accept connection");

            return -1;
        }

        // Reply to the client until it closes the connection.
        fprintf(stdout, "connection accepted\n");
        reply(conn_fd);

        // Close the connected descriptor.
        // The connection is terminated since the client and the server closed
        // their respective descriptors.
        close(conn_fd);
        fprintf(stdout, "connection closed\n");
    }

    return 0;
}

#define MAX_LINE_SIZE 6

// reply repeatedly reads and writes lines of text until the client closes the connection, i.e., server reads EOF.
// It expects to read a number from the client, and sends back whether the number is even or odd.
void reply(int conn_fd) {
    rio_t r = {
        .fd = conn_fd,
        .unread_cnt = 0,
    };
    r.next = r.buf;

    ssize_t nread, nwritten;
    char buf[MAX_LINE_SIZE];
    while ((nread = rio_readline(&r, buf, MAX_LINE_SIZE)) != 0) {
        char *resp = (atoi(buf) % 2 == 0) ? "even\n" : "odd\n";
        nwritten = rio_writen(conn_fd, resp, strlen(resp));
        if (nwritten < 0) {
            perror("writing response");
        }
    }

    if (nread < 0) {
        perror("reading a line from client");
    }
}
