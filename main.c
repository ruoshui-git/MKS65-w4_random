#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <errno.h>
#include <string.h>

#define NUM_RANDOM_INTS 10

int randint(void);
void report_error(char *msg);

int main(void)
{
    int randints[NUM_RANDOM_INTS];
    puts("Reading ints from /dev/urandom...");
    int i;
    for (i = 0; i < NUM_RANDOM_INTS; i++)
    {
        randints[i] = randint();
    }

    puts("What's in the array?");
    for (i = 0; i < NUM_RANDOM_INTS; i++)
    {
        printf("\trandom %d: %d\n", i, randints[i]);
    }

    puts("\nWriting numbers to a file...");

    int numsf = open("nums.txt", O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (numsf < 0)
    {
        report_error("Error while creating/opening temp text file.");
        return 1;
    }

    int bytes_written = write(numsf, randints, sizeof(int) * NUM_RANDOM_INTS);
    if (bytes_written == -1)
    {
        report_error("Error while writing to text file.");
        close(numsf);
        return 1;
    }

    puts("Done writing. Reading from file...");

    int orandints[NUM_RANDOM_INTS];
    int fin = open("nums.txt", O_RDONLY);
    int bytes_read = read(fin, orandints, sizeof(int) * NUM_RANDOM_INTS);
    if (bytes_read == -1)
    {
        report_error("Error while reading from text file.");
        close(numsf);
        return 1;
    }

    puts("Verify read:");
    for (i = 0; i < NUM_RANDOM_INTS; i++)
    {
        printf("\trandom %d: %d\n", i, orandints[i]);
    }

    puts("Done. Exiting...");
    close(numsf);
}

/**
 * returns: random integer
 **/
int randint(void)
{
    int randf = open("/dev/urandom", O_RDONLY);
    if (randf < 0)
    {
        report_error("Error opening urandom.");
        exit(1);
    }
    int buff, bytes_read;
    bytes_read = read(randf, &buff, sizeof(int));
    if (bytes_read == -1)
    {
        int e = errno;
        report_error("Error reading from /dev/urandom.");
        close(randf);
        exit(1);
    }
    close(randf);
    return buff;
}

void report_error(char *msg)
{
    int e = errno;
    fprintf(stderr, "%s\n\tError %d: %s\n", msg, e, strerror(e));
}