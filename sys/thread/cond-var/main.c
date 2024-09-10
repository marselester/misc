// This program reads up to MAX_INPUT_SIZE digits from the user and prints the sum.
// Reading and printing are done in separate threads.
//
// Warning, the errors are not handled for brevity, e.g., pthread_mutex_lock().
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// The maximum number of characters a user can enter.
#define MAX_INPUT_SIZE 5

// buf_t represents a buffer to share an array of integers between threads.
typedef struct {
    // The condition variable to signal that the buffer has been updated.
    pthread_cond_t cond;
    // The mutex to lock the buffer.
    pthread_mutex_t mu;
    // The buffer to store the parsed input.
    int buf[MAX_INPUT_SIZE];
    // The number of elements in the buffer.
    int n;
} buf_t;

// new_buffer creates and initializes a new buffer.
// The caller is responsible for freeing the buffer when it is no longer needed.
buf_t *new_buffer() {
    buf_t *buf = (buf_t *) malloc(sizeof(buf_t));

    pthread_cond_init(&buf->cond, NULL);
    pthread_mutex_init(&buf->mu, NULL);
    buf->n = 0;

    return buf;
}

void *receive_input(void *arg);
void *print_input(void *arg);

int main() {
    buf_t *buf = new_buffer();

    pthread_t tid_in, tid_out;
    pthread_create(&tid_in, NULL, receive_input, buf);
    pthread_create(&tid_out, NULL, print_input, buf);
    pthread_join(tid_in, NULL);
    pthread_join(tid_out, NULL);

    free(buf);

    return 0;
}

int read_line(int digits[MAX_INPUT_SIZE]);

// receive_input reads a line from the stdin (up to MAX_INPUT_SIZE) and
// stores them in the shared buffer.
// The access to the buffer is protected by a mutex.
// Once the buffer is updated, it signals the other thread to print the buffer.
void *receive_input(void *arg) {
    buf_t *buf = (buf_t *) arg;

    int digits[MAX_INPUT_SIZE];
    int n;

    while (1) {
        printf("Enter up to %d digits:\n", MAX_INPUT_SIZE);
        n = read_line(digits);

        pthread_mutex_lock(&buf->mu);
        for (int i = 0; i < n; i++) {
            // printf("%d\n", digits[i]);
            buf->buf[i] = digits[i];
        }
        buf->n = n;
        pthread_mutex_unlock(&buf->mu);

        // Signal the other thread that the buffer has been updated.
        if (n > 0) {
            pthread_cond_signal(&buf->cond);
        }
    }

    return NULL;
}

// print_input prints the buffer contents to the stdout.
// The thread waits for the signal to print the buffer.
void *print_input(void *arg) {
    buf_t *buf = (buf_t *) arg;
    int sum;

    // Note, this thread waiting on the condition variable might be woken up
    // in rare cases even though no other thread has signaled the condition variable,
    // see https://www.man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html,
    // https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html.
    while (1) {
        // Lock the mutex in preparation for checking the state of the buffer.
        pthread_mutex_lock(&buf->mu);
        // Block this thread until the condition variable is signaled,
        // i.e., the state of the buffer is checked and if it's not ready:
        // - unlock the mutex mu so that other threads can access the buffer,
        // - block this thread until another thread signals the condition variable cond,
        // - relock the mutex mu when this thread is reawakened because
        //   the condition variable has been signaled.
        while (pthread_cond_wait(&buf->cond, &buf->mu));

        sum = 0;
        for (int i = 0; i < buf->n; i++) {
            sum += buf->buf[i];
            // printf("%d ", buf->buf[i]);
        }
        pthread_mutex_unlock(&buf->mu);

        printf("Sum: %d\n", sum);
    }

    return NULL;
}

int read_line(int digits[MAX_INPUT_SIZE]) {
    char c;
    bool is_digit;
    int i = 0;
    int n = 0;

    // Keep going until the end of the line to clear the input buffer.
    while ((c = getchar()) && c != '\n' && c != EOF) {
        // printf("Character \"%c\" (%d)\n", c, c);
        is_digit = c >= '0' && c <= '9';
        if (n < MAX_INPUT_SIZE && is_digit) {
            digits[n] = c - '0';
            n++;
        }

        if (!is_digit) {
            printf("Invalid digit \"%c\"\n", c);
        }

        i++;
    }

    return n;
}
