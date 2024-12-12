// These helper functions are from Computer Systems book.
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// open_client_fd establishes a connection with a server running on hostname:port
// (at least one of them must be specified).
// The hostname can be a domain name or an IP address.
// It returns an open socket descriptor on success, -1 on error.
int open_client_fd(char *hostname, char *port) {
    // Hints tells getaddrinfo how to filter potential server addresses.
    // Its fields should be initialized to zero.
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    // We're interested in a stream socket type (TCP), not a datagram (UDP).
    hints.ai_socktype = SOCK_STREAM;
    // This flag forces getaddrinfo to treat the port argument to be a port number,
    // not a service name such as http.
    hints.ai_flags = AI_NUMERICSERV;
    // This flag tells getaddrinfo to return IPv4 addresses only
    // if the local host is configued for IPv4. Similarly for IPv6.
    hints.ai_flags |= AI_ADDRCONFIG;

    // The result points to the first addrinfo structure in a linked list.
    struct addrinfo *result;

    // Get a list of potential server addresses (addrinfo structures).
    // Each struct points to a socket address structure that is suitable for
    // establishing a connection with the server.
    int errcode = getaddrinfo(hostname, port, &hints, &result);
    // Print an error message if getaddrinfo fails, see netdb.h.
    if (errcode != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errcode));

        return -1;
    }

    int fd = -1;
    struct addrinfo *addr = result;
    // Walk the linked list of server addresses, trying to establish a connection.
    for (; addr != NULL; addr = addr->ai_next) {
        // Create a socket descriptor, e.g., AF_INET (IPv4 addresses),
        // SOCK_STREAM (sequenced, reliable, two-way connection based byte streams),
        // IPPROTO_TCP (TCP protocol that implements SOCK_STREAM semantics).
        // See https://www.man7.org/linux/man-pages/man7/ip.7.html.
        fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        // If socket() fails, try the next address.
        if (fd < 0) {
            continue;
        }

        // Connect to the server. It blocks until the connection is established or an error occurs.
        errcode = connect(fd, addr->ai_addr, addr->ai_addrlen);
        if (errcode != -1) {
            // Connection established. It's characterized by the socket pair
            // (local IP address:local port, remote IP address:remote port).
            // A port identifies a process on a host.
            break;
        }

        // Close the socket descriptor when connect fails and try another address.
        close(fd);
    }

    // Free the linked list.
    freeaddrinfo(result);
    if (addr == NULL) {
        // No address succeeded.
        return -1;
    }

    return fd;
}

// The backlog defines the maximum length for the queue of pending connections.
#define backlog 8

int open_listen_fd(char *port) {
    // Hints tells getaddrinfo how to filter potential server addresses.
    // Its fields should be initialized to zero.
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    // We're interested in a stream socket type (TCP), not a datagram (UDP).
    hints.ai_socktype = SOCK_STREAM;
    // This flag forces getaddrinfo to treat the port argument to be a port number,
    // not a service name such as http.
    hints.ai_flags = AI_NUMERICSERV;
    // This flag tells getaddrinfo to return IPv4 addresses only
    // if the local host is configued for IPv4. Similarly for IPv6.
    hints.ai_flags |= AI_ADDRCONFIG;
    // Instructs getaddrinfo to return socket addresses
    // that can be used by servers as listening sockets.
    hints.ai_flags |= AI_PASSIVE;

    // The result points to the first addrinfo structure in a linked list.
    struct addrinfo *result;

    // Get a list of potential server addresses (addrinfo structures).
    // Each struct points to a socket address structure that is suitable for
    // establishing a connection with the server.
    int errcode = getaddrinfo(NULL, port, &hints, &result);
    // Print an error message if getaddrinfo fails, see netdb.h.
    if (errcode != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errcode));

        return -1;
    }

    int fd = -1;
    int optval = 1;
    struct addrinfo *addr = result;
    // Walk the linked list of server addresses, trying to bind to.
    for (; addr != NULL; addr = addr->ai_next) {
        // Create a socket descriptor.
        fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        // If socket() fails, try the next address.
        if (fd < 0) {
            continue;
        }

        // Enable the socket descriptor to be reused
        // to bind to the same address after the server terminates.
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        // Bind the descriptor to the address.
        errcode = bind(fd, addr->ai_addr, addr->ai_addrlen);
        if (errcode == 0) {
            // Bind successful.
            break;
        }

        // Close the socket descriptor when bind fails, and try another address.
        close(fd);
    }

    // Free the linked list.
    freeaddrinfo(result);
    if (addr == NULL) {
        // No address succeeded.
        return -1;
    }

    // Make the socket descriptor a listening socket ready to accept connection requests.
    errcode = listen(fd, backlog);
    if (errcode < 0) {
        close(fd);

        return -1;
    }

    return fd;
}
