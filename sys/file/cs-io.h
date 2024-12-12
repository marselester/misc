// These helper functions are from Computer Systems book.
#include <errno.h>

// rio_readn reliabily reads at most n bytes from a file descriptor into a buffer.
// It restarts read() if it's interrupted by the return from an app signal handler.
ssize_t rio_readn(int fd, void *usrbuf, size_t n) {
    // Number of bytes left to read.
    size_t nleft = n;
    // Number of bytes read by read(), ssize_t is defined as signed long.
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0) {
        nread = read(fd, bufp, nleft);
        switch (nread) {
            // EOF.
            case 0:
                return n - nleft;
            // Error.
            case -1:
                if (errno != EINTR) {
                    return -1;
                }
                // Interrupted by a signal handler return, restart read().
                nread = 0;
                break;
            // Read nread bytes.
            default:
                nleft -= nread;
                bufp += nread;
                break;
        }
    }

    return n - nleft;
}

// rio_writen reliabily writes n bytes to a file descriptor from a buffer.
// It restarts write() if it's interrupted by the return from an app signal handler.
// It never returns a short count.
ssize_t rio_writen(int fd, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
        nwritten = write(fd, bufp, nleft);
        if (nwritten <= 0) {
            if (errno != EINTR) {
                return -1;
            }
            nwritten = 0;
        }

        nleft -= nwritten;
        bufp += nwritten;
    }

    return n;
}

#define BUF_SIZE 8192
typedef struct {
    int fd; // Descriptor for this internal buffer.
    int unread_cnt; // Unread bytes in internal buffer.
    char *next; // Next unread byte in internal buffer.
    char buf[BUF_SIZE]; // Internal buffer.
} rio_t;

// rio_read is like read(), but bufferred.
// First, it copies at most n bytes from internal buffer to user buffer.
// If internal buffer is empty, it calls read() to fill it up.
static ssize_t rio_read(rio_t *r, char *usrbuf, size_t n) {
    int cnt;

    while (r->unread_cnt <= 0) {
        r->unread_cnt = read(r->fd, r->buf, sizeof(r->buf));
        if (r->unread_cnt < 0) {
            if (errno != EINTR) {
                return -1;
            }
        } else if (r->unread_cnt == 0) { // EOF.
            return 0;
        } else {
            r->next = r->buf;
        }
    }

    // Copy min(n, r->unread_cnt) bytes from internal buffer to user buffer.
    cnt = n;
    if (r->unread_cnt < n) {
        cnt = r->unread_cnt;
    }
    memcpy(usrbuf, r->next, cnt);
    r->next += cnt;
    r->unread_cnt -= cnt;

    return cnt;
}

// rio_readline reads a line from a file descriptor to a user buffer.
ssize_t rio_readline(rio_t *r, void *usrbuf, size_t maxlen) {
    int n, rc;
    char c, *bufp = usrbuf;

    // Read from the internal buffer 1 byte at a time, until a newline is found.
    for (n = 1; n < maxlen; n++) {
        if ((rc = rio_read(r, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n') {
                n++;
                break;
            }
        } else if (rc == 0) {
            if (n == 1) { // EOF, no data read.
                return 0;
            } else {
                break; // EOF, some data read.
            }
        } else {
            return -1; // Error.
        }
    }

    *bufp = 0;

    return n - 1;
}
