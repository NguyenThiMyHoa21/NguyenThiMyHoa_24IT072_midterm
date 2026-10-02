#include "ls.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int
is_hidden_name(const char *name)
{
    return name != NULL && name[0] == '.';
}

int
should_show(const char *name, const LSOptions *opt)
{
    if (name == NULL || opt == NULL) {
        return 0;
    }

    if (!is_hidden_name(name)) {
        return 1;
    }

    if (opt->all) {
        return 1;
    }

    if (opt->almost_all) {
        if (strcmp(name, ".") == 0 ||
            strcmp(name, "..") == 0) {
            return 0;
        }
        return 1;
    }

    return 0;
}

const char *
display_name(const char *name, char *buf,
             size_t bufsz, const LSOptions *opt)
{
    size_t i;
    size_t pos;

    if (name == NULL || buf == NULL || bufsz == 0) {
        return "";
    }

    /*
     * -w: print the name without replacing characters.
     */
    if (opt != NULL && opt->raw) {
        (void)snprintf(buf, bufsz, "%s", name);
        return buf;
    }

    /*
     * -q: replace non-printable characters with '?'.
     */
    pos = 0;

    for (i = 0; name[i] != '\0' && pos + 1 < bufsz; i++) {
        unsigned char c = (unsigned char)name[i];

        if (c >= 32 && c != 127) {
            buf[pos++] = (char)c;
        } else {
            buf[pos++] = '?';
        }
    }

    buf[pos] = '\0';

    return buf;
}

int
is_executable(const struct stat *st)
{
    if (st == NULL) {
        return 0;
    }

    return (st->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0;
}

char
classify_char(const struct stat *st)
{
    if (st == NULL) {
        return '\0';
    }

    if (S_ISDIR(st->st_mode)) {
        return '/';
    }

    if (S_ISLNK(st->st_mode)) {
        return '@';
    }

    if (S_ISSOCK(st->st_mode)) {
        return '=';
    }

    if (S_ISFIFO(st->st_mode)) {
        return '|';
    }

    if (is_executable(st)) {
        return '*';
    }

    return '\0';
}
