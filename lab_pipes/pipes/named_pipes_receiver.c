#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_PATHNAME "/tmp/fifo.pipe"
#define ACK_PATHNAME "/tmp/ack.pipe"
#define BUFFER_SIZE (128)

int main() {
    // Open pipe for reading
    // This waits for someone to open it for writing
    int rx = open(FIFO_PATHNAME, O_RDONLY);
    if (rx == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char pipeack[BUFFER_SIZE];
    ssize_t aux = read(rx, pipeack, BUFFER_SIZE - 1);
    
    
    int ackwx = open(ACK_PATHNAME, O_WRONLY);
    if (ackwx == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(ackwx,"1",1);
    while (true) {
        char buffer[BUFFER_SIZE];
        ssize_t ret = read(rx, buffer, BUFFER_SIZE - 1);
        write(ackwx,"1",1);
        if (ret == 0) {
            // ret == 0 indicates EOF
            fprintf(stderr, "[INFO]: pipe closed\n");
            return 0;
        } else if (ret == -1) {
            // ret == -1 indicates error
            fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "[INFO]: received %zd B\n", ret);
        buffer[ret] = 0;
        fputs(buffer, stdout);
    }

    close(rx);
    close(ackwx);
}
