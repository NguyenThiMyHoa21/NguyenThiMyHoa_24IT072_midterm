#include "ls.h"

#include <stdlib.h>
#include <string.h>

static int
compare_name(const void *a, const void *b)
{
    const LSEntry *ea = (const LSEntry *)a;
    const LSEntry *eb = (const LSEntry *)b;

    return strcmp(ea->name, eb->name);
}

static int
compare_size(const void *a, const void *b)
{
    const LSEntry *ea = (const LSEntry *)a;
    const LSEntry *eb = (const LSEntry *)b;

    if (ea->st.st_size < eb->st.st_size) {
        return 1;
    }

    if (ea->st.st_size > eb->st.st_size) {
        return -1;
    }

    return strcmp(ea->name, eb->name);
}

static int
compare_time(const void *a, const void *b, const LSOptions *opt)
{
    const LSEntry *ea = (const LSEntry *)a;
    const LSEntry *eb = (const LSEntry *)b;
    time_t ta;
    time_t tb;

    if (opt->use_ctime) {
        ta = ea->st.st_ctime;
        tb = eb->st.st_ctime;
    } else if (opt->use_atime) {
        ta = ea->st.st_atime;
        tb = eb->st.st_atime;
    } else {
        ta = ea->st.st_mtime;
        tb = eb->st.st_mtime;
    }

    if (ta < tb) {
        return 1;
    }

    if (ta > tb) {
        return -1;
    }

    return strcmp(ea->name, eb->name);
}

static void
reverse_entries(LSEntry *entries, size_t count)
{
    size_t i;

    for (i = 0; i < count / 2; i++) {
        LSEntry temp;

        temp = entries[i];
        entries[i] = entries[count - 1 - i];
        entries[count - 1 - i] = temp;
    }
}

void
sort_entries(LSEntry *entries, size_t count,
             const LSOptions *opt)
{
    if (entries == NULL || count < 2 || opt == NULL) {
        return;
    }

    /*
     * -f disables sorting.
     */
    if (opt->unsorted) {
        return;
    }

    if (opt->sort_size) {
        qsort(entries, count, sizeof(*entries), compare_size);
    } else if (opt->sort_time) {
        size_t i;
        size_t j;

        /*
         * The time comparator needs LSOptions so that -u and -c
         * can select atime and ctime respectively.
         */
        for (i = 0; i < count; i++) {
            for (j = i + 1; j < count; j++) {
                if (compare_time(&entries[i], &entries[j], opt) > 0) {
                    LSEntry temp;

                    temp = entries[i];
                    entries[i] = entries[j];
                    entries[j] = temp;
                }
            }
        }
    } else {
        qsort(entries, count, sizeof(*entries), compare_name);
    }

    if (opt->reverse) {
        reverse_entries(entries, count);
    }
}

void
free_entries(LSEntry *entries, size_t count)
{
    size_t i;

    if (entries == NULL) {
        return;
    }

    for (i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].path);
    }

    free(entries);
}
