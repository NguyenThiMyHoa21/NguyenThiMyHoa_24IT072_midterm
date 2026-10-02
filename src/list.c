#include "ls.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *
make_path(const char *dir, const char *name)
{
    size_t dir_len;
    size_t name_len;
    size_t need_slash;
    char *path;

    if (dir == NULL || name == NULL) {
        return NULL;
    }

    dir_len = strlen(dir);
    name_len = strlen(name);

    need_slash = (dir_len > 0 && dir[dir_len - 1] != '/');

    path = malloc(dir_len + need_slash + name_len + 1);

    if (path == NULL) {
        return NULL;
    }

    memcpy(path, dir, dir_len);

    if (need_slash) {
        path[dir_len] = '/';
        memcpy(path + dir_len + 1, name, name_len);
        path[dir_len + 1 + name_len] = '\0';
    } else {
        memcpy(path + dir_len, name, name_len);
        path[dir_len + name_len] = '\0';
    }

    return path;
}

static int
load_directory(const char *path,
               const LSOptions *opt,
               LSEntry **entries_out,
               size_t *count_out)
{
    DIR *dir;
    struct dirent *entry;
    LSEntry *entries;
    size_t count;
    size_t capacity;

    if (path == NULL || opt == NULL ||
        entries_out == NULL || count_out == NULL) {
        return -1;
    }

    dir = opendir(path);

    if (dir == NULL) {
        fprintf(stderr, "ls: %s: %s\n",
                path, strerror(errno));
        return -1;
    }

    entries = NULL;
    count = 0;
    capacity = 0;

    while ((entry = readdir(dir)) != NULL) {
        LSEntry item;

        if (!should_show(entry->d_name, opt)) {
            continue;
        }

        item.name = strdup(entry->d_name);

        if (item.name == NULL) {
            fprintf(stderr, "ls: memory allocation failed\n");
            free_entries(entries, count);
            closedir(dir);
            return -1;
        }

        item.path = make_path(path, entry->d_name);

        if (item.path == NULL) {
            fprintf(stderr, "ls: memory allocation failed\n");
            free(item.name);
            free_entries(entries, count);
            closedir(dir);
            return -1;
        }

        if (lstat(item.path, &item.st) == -1) {
            fprintf(stderr, "ls: %s: %s\n",
                    item.path, strerror(errno));
            free(item.name);
            free(item.path);
            continue;
        }

        if (count == capacity) {
            size_t new_capacity;
            LSEntry *new_entries;

            if (capacity == 0) {
                new_capacity = 32;
            } else {
                new_capacity = capacity * 2;
            }

            new_entries = realloc(
                entries,
                new_capacity * sizeof(*entries)
            );

            if (new_entries == NULL) {
                fprintf(stderr,
                        "ls: memory allocation failed\n");
                free(item.name);
                free(item.path);
                free_entries(entries, count);
                closedir(dir);
                return -1;
            }

            entries = new_entries;
            capacity = new_capacity;
        }

        entries[count] = item;
        count++;
    }

    if (closedir(dir) == -1) {
        fprintf(stderr, "ls: %s: %s\n",
                path, strerror(errno));
        free_entries(entries, count);
        return -1;
    }

    *entries_out = entries;
    *count_out = count;

    return 0;
}

int
list_directory(const char *path,
               const LSOptions *opt,
               int print_header)
{
    LSEntry *entries;
    size_t count;

    if (path == NULL || opt == NULL) {
        return -1;
    }

    if (print_header) {
        printf("%s:\n", path);
    }

    if (load_directory(path, opt,
                       &entries, &count) != 0) {
        return -1;
    }

    sort_entries(entries, count, opt);

    print_entries(entries, count, opt, path);

    /*
     * -R recursively visits directories.
     *
     * We use lstat(), so symbolic links are not followed
     * as directories.
     */
    if (opt->recursive) {
        size_t i;

        for (i = 0; i < count; i++) {
            char *child_path;

            if (!S_ISDIR(entries[i].st.st_mode)) {
                continue;
            }

            if (strcmp(entries[i].name, ".") == 0 ||
                strcmp(entries[i].name, "..") == 0) {
                continue;
            }

            child_path = entries[i].path;

            printf("\n%s:\n", child_path);

            if (list_directory(child_path,
                               opt, 0) != 0) {
                /*
                 * Keep processing other directories even if
                 * one recursive directory cannot be read.
                 */
            }
        }
    }

    free_entries(entries, count);

    return 0;
}

int
list_path(const char *path,
          const LSOptions *opt,
          int print_header)
{
    struct stat st;

    if (path == NULL || opt == NULL) {
        return -1;
    }

    if (lstat(path, &st) == -1) {
        fprintf(stderr, "ls: %s: %s\n",
                path, strerror(errno));
        return -1;
    }

    /*
     * -d forces directories to be treated as plain files.
     */
    if (!S_ISDIR(st.st_mode) || opt->directory) {
        LSEntry entry;
        LSEntry *entries;

        entry.name = strdup(path);
        entry.path = strdup(path);
        entry.st = st;

        if (entry.name == NULL || entry.path == NULL) {
            free(entry.name);
            free(entry.path);
            fprintf(stderr,
                    "ls: memory allocation failed\n");
            return -1;
        }

        entries = malloc(sizeof(*entries));

        if (entries == NULL) {
            free(entry.name);
            free(entry.path);
            fprintf(stderr,
                    "ls: memory allocation failed\n");
            return -1;
        }

        entries[0] = entry;

        print_entries(entries, 1, opt, NULL);

        free_entries(entries, 1);

        return 0;
    }

    return list_directory(path, opt, print_header);
}
