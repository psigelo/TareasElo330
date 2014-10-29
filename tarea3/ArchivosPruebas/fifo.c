#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define FIFONAME    "myfifo"

int
main(void)
{
    int n, fd[2];
    char buf[1024];
    printf("00000\n");
    /*
     * Remove any previous FIFO.
     */
    unlink(FIFONAME);
    printf("0101010\n");
    /*
     * Create the FIFO.
     */
    if (mkfifo(FIFONAME, 0666) < 0) {
        perror("mkfifo");
        exit(1);
    }

    /*
     * Open the FIFO for reading.
     */
     pipe(fd);
     printf("11111\n");
    if ((fd[1] = open(FIFONAME, O_RDONLY)) < 0) {
        perror("open");
        exit(1);
    }
    printf("hooola\n");
    /*
     * Read from the FIFO until end-of-file and
     * print what we get on the standard output.
     */
    while ((n = read(fd, buf, sizeof(buf))) > 0)
        write(1, buf, n);

    close(fd);
    exit(0);
}