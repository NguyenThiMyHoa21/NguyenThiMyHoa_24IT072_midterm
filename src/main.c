#include "ls.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Ham so sanh theo thu tu tu dien */
static int
compare_names(const void *a, const void *b)
{
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

int
main(int argc, char **argv)
{
    LSOptions opt;
    int first_operand;
    int operand_count;
    int status = 0;
    int i;
    char **files;
    char **dirs;
    int file_cnt = 0;
    int dir_cnt = 0;

    options_init(&opt);

    if (options_parse(argc, argv, &opt, &first_operand) != 0) {
        return EXIT_FAILURE;
    }

    operand_count = argc - first_operand;

    /* Khong co operand: liet ke thu muc hien tai */
    if (operand_count == 0) {
        status = list_path(".", &opt, 0);
        return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    /* Neu chi co 1 operand: chay truc tiep */
    if (operand_count == 1) {
        status = list_path(argv[first_operand], &opt, 0);
        return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    /* Nhieu hon 1 operand: Phan loai file va directory */
    files = malloc((size_t)operand_count * sizeof(char *));
    dirs  = malloc((size_t)operand_count * sizeof(char *));

    if (files == NULL || dirs == NULL) {
        perror("ls: malloc");
        free(files);
        free(dirs);
        return EXIT_FAILURE;
    }

    for (i = first_operand; i < argc; i++) {
        struct stat sb;
        int stat_res;

        /* Neu co -d, moi thu muc deu coi nhu file */
        if (opt.directory) {
            files[file_cnt++] = argv[i];
            continue;
        }

        stat_res = stat(argv[i], &sb);

        if (stat_res != 0) {
            /* File khong ton tai hoac loi: dua vao files de list_path bao loi */
            files[file_cnt++] = argv[i];
        } else if (S_ISDIR(sb.st_mode)) {
            dirs[dir_cnt++] = argv[i];
        } else {
            files[file_cnt++] = argv[i];
        }
    }

    /* Sap xep rieng tung nhom neu khong co co -f */
    if (!opt.unsorted) {
        if (file_cnt > 1) {
            qsort(files, (size_t)file_cnt, sizeof(char *), compare_names);
        }
        if (dir_cnt > 1) {
            qsort(dirs, (size_t)dir_cnt, sizeof(char *), compare_names);
        }
    }

    /* 1. In toan bo file truoc */
    for (i = 0; i < file_cnt; i++) {
        if (list_path(files[i], &opt, 0) != 0) {
            status = 1;
        }
    }

    /* 2. In toan bo directory sau (in kem ten thu muc lam header) */
    for (i = 0; i < dir_cnt; i++) {
        if (list_path(dirs[i], &opt, 1) != 0) {
            status = 1;
        }
    }

    free(files);
    free(dirs);

    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
