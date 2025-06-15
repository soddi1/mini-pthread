#include "util.h"
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>

extern sigset_t signal_mask;

addr_t mangle(addr_t address)
{
    addr_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
                 : "=g"(ret)
                 : "0"(address));
    return ret;
}

void enable_interrupts()
{
    sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
}

void disable_interrupts()
{
    sigprocmask(SIG_BLOCK, &signal_mask, NULL);
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to write a formatted string to standard output
void write_formatted(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Lock the mutex before entering the critical section
    pthread_mutex_lock(&mutex);

    // Iterate through the format string
    while (*format != '\0')
    {
        if (*format == '%')
        {
            fflush(fdopen(STDOUT_FILENO, "w"));
            format++; // Move past '%'
            // Handle format specifiers
            switch (*format)
            {
            case 'd':
            {
                int value = va_arg(args, int);
                char buffer[20]; // Assuming a reasonable buffer size
                int len = 0;
                while (value != 0)
                {
                    buffer[len++] = '0' + value % 10;
                    value /= 10;
                }
                // Handle the case where the value is 0
                if (len == 0)
                {
                    buffer[len++] = '0';
                }
                // Write the reversed buffer to standard output
                while (len > 0)
                {
                    write(STDOUT_FILENO, &buffer[--len], 1);
                }
                break;
            }
            case 's':
            {
                char *str = va_arg(args, char *);
                // Write the string to standard output
                while (*str != '\0')
                {
                    write(STDOUT_FILENO, str++, 1);
                }
                break;
            }

            // Handle other characters if needed
            default:
                write(STDOUT_FILENO, format, 1);
            }
        }
        else
        {
            // Just write non-format characters
            write(STDOUT_FILENO, format, 1);
        }

        fflush(stdout);
        format++; // Move to the next character in the format string
    }

    // Unlock the mutex before exiting the function
    pthread_mutex_unlock(&mutex);

    va_end(args);
}