#include "ls.h"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    LSOptions opt;
    int first_operand;
    int status;
    int i;
    int operand_count;

    options_init(&opt);

    if (options_parse(argc, argv, &opt, &first_operand) != 0) {
        return EXIT_FAILURE;
    }

    operand_count = argc - first_operand;

    /*
     * If no operand is specified, ls lists the current directory.
     */
    if (operand_count == 0) {
        status = list_path(".", &opt, 0);
        return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    status = 0;

    /*
     * Process each operand in the order required by the ls interface.
     * The list module is responsible for handling files and directories.
     */
    for (i = first_operand; i < argc; i++) {
        if (list_path(argv[i], &opt, operand_count > 1) != 0) {
            status = 1;
        }
    }

    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
