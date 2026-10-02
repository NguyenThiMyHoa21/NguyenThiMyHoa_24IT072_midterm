#include "ls.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void
options_init(LSOptions *opt)
{
    memset(opt, 0, sizeof(*opt));
}

int
options_parse(int argc, char **argv,
              LSOptions *opt, int *first_operand)
{
    int ch;

    /*
     * The supported options are the options specified
     * by the provided ls manual.
     */
    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A':
            opt->almost_all = 1;
            break;

        case 'a':
            opt->all = 1;
            break;

        case 'c':
            opt->use_ctime = 1;
            opt->use_atime = 0;
            break;

        case 'd':
            opt->directory = 1;
            opt->recursive = 0;
            break;

        case 'F':
            opt->classify = 1;
            break;

        case 'f':
            opt->unsorted = 1;
            break;

        case 'h':
            opt->human = 1;
            opt->kilobytes = 0;
            break;

        case 'i':
            opt->inode = 1;
            break;

        case 'k':
            opt->kilobytes = 1;
            opt->human = 0;
            break;

        case 'l':
            opt->long_format = 1;
            opt->numeric = 0;
            break;

        case 'n':
            opt->numeric = 1;
            opt->long_format = 1;
            break;

        case 'q':
            opt->quote = 1;
            opt->raw = 0;
            break;

        case 'R':
            opt->recursive = 1;
            opt->directory = 0;
            break;

        case 'r':
            opt->reverse = 1;
            break;

        case 'S':
            opt->sort_size = 1;
            break;

        case 's':
            opt->blocks = 1;
            break;

        case 't':
            opt->sort_time = 1;
            break;

        case 'u':
            opt->use_atime = 1;
            opt->use_ctime = 0;
            break;

        case 'w':
            opt->raw = 1;
            opt->quote = 0;
            break;

        default:
            fprintf(stderr, "usage: ls [-AacdFfhiklnqRrSstuw] [file ...]\n");
            return -1;
        }
    }

    /*
     * -f means do not sort. It takes precedence over
     * the normal sorting step.
     */
    if (opt->unsorted) {
        opt->sort_size = 0;
        opt->sort_time = 0;
    }

    *first_operand = optind;
    return 0;
}
