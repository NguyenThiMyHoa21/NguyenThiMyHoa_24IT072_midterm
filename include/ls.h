#ifndef LS_H
#define LS_H

#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

#define LS_MAX_NAME 4096

typedef struct {
    int all;            /* -a */
    int almost_all;     /* -A */
    int use_ctime;      /* -c */
    int directory;      /* -d */
    int classify;       /* -F */
    int unsorted;       /* -f */
    int human;          /* -h */
    int inode;          /* -i */
    int kilobytes;      /* -k */
    int long_format;    /* -l */
    int numeric;        /* -n */
    int quote;          /* -q */
    int recursive;      /* -R */
    int reverse;        /* -r */
    int sort_size;      /* -S */
    int blocks;         /* -s */
    int sort_time;      /* -t */
    int use_atime;      /* -u */
    int raw;            /* -w */
} LSOptions;

typedef struct {
    char *name;
    char *path;
    struct stat st;
} LSEntry;

void options_init(LSOptions *opt);

int options_parse(int argc, char **argv,
                  LSOptions *opt, int *first_operand);

int list_path(const char *path,
              const LSOptions *opt, int print_header);

int list_directory(const char *path,
                   const LSOptions *opt, int print_header);

int is_hidden_name(const char *name);

int should_show(const char *name,
                const LSOptions *opt);

const char *display_name(const char *name,
                         char *buf, size_t bufsz,
                         const LSOptions *opt);

void sort_entries(LSEntry *entries,
                  size_t count,
                  const LSOptions *opt);

void free_entries(LSEntry *entries,
                  size_t count);

void print_entries(LSEntry *entries,
                   size_t count,
                   const LSOptions *opt,
                   const char *dirpath);

int is_executable(const struct stat *st);

char classify_char(const struct stat *st);

#endif
