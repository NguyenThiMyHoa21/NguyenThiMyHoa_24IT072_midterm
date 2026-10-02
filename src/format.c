#include "ls.h"

#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void
mode_string(mode_t mode, char *buf)
{
    if (S_ISREG(mode)) {
        buf[0] = '-';
    } else if (S_ISDIR(mode)) {
        buf[0] = 'd';
    } else if (S_ISLNK(mode)) {
        buf[0] = 'l';
    } else if (S_ISCHR(mode)) {
        buf[0] = 'c';
    } else if (S_ISBLK(mode)) {
        buf[0] = 'b';
    } else if (S_ISFIFO(mode)) {
        buf[0] = 'p';
    } else if (S_ISSOCK(mode)) {
        buf[0] = 's';
    } else {
        buf[0] = '?';
    }

    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';

    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';

    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';

    if (mode & S_ISUID) {
        buf[3] = (mode & S_IXUSR) ? 's' : 'S';
    }

    if (mode & S_ISGID) {
        buf[6] = (mode & S_IXGRP) ? 's' : 'S';
    }


    buf[10] = '\0';
}

static const char *
owner_name(const struct stat *st, const LSOptions *opt)
{
   static char numeric[32];
    struct passwd *pw;

    if (opt->numeric) {
        (void)snprintf(numeric, sizeof(numeric),
                       "%u", (unsigned int)st->st_uid);
        return numeric;
    }

    pw = getpwuid(st->st_uid);

    if (pw != NULL) {
        return pw->pw_name;
    }

    (void)snprintf(numeric, sizeof(numeric),
                   "%u", (unsigned int)st->st_uid);

    return numeric;
}

static const char *
group_name(const struct stat *st, const LSOptions *opt)
{
    static char numeric[32];
    struct group *gr;

    if (opt->numeric) {
        (void)snprintf(numeric, sizeof(numeric),
                       "%u", (unsigned int)st->st_gid);
        return numeric;
    }

    gr = getgrgid(st->st_gid);

    if (gr != NULL) {
        return gr->gr_name;
    }

    (void)snprintf(numeric, sizeof(numeric),
                   "%u", (unsigned int)st->st_gid);

    return numeric;
}

static void
format_size(off_t size, const LSOptions *opt,
            char *buf, size_t bufsz)
{
    if (opt->human) {
        double value = (double)size;
        const char *units[] = {
            "B", "K", "M", "G", "T", "P"
        };
        size_t unit = 0;

        while (value >= 1024.0 &&
               unit < sizeof(units) / sizeof(units[0]) - 1) {
            value /= 1024.0;
            unit++;
        }

        if (unit == 0) {
            (void)snprintf(buf, bufsz, "%lld%s",
                           (long long)size, units[unit]);
        } else if (value < 10.0) {
            (void)snprintf(buf, bufsz, "%.1f%s",
                           value, units[unit]);
        } else {
            (void)snprintf(buf, bufsz, "%.0f%s",
                           value, units[unit]);
        }

        return;
    }

    if (opt->kilobytes) {
        long long kb;

        kb = ((long long)size + 1023LL) / 1024LL;

        (void)snprintf(buf, bufsz, "%lld", kb);
        return;
    }

    (void)snprintf(buf, bufsz, "%lld", (long long)size);
}

static void
format_time(time_t value, char *buf, size_t bufsz)
{
    struct tm *tm_info;

    tm_info = localtime(&value);

    if (tm_info == NULL) {
        (void)snprintf(buf, bufsz, "??? ?? ??:??");
        return;
    }

    (void)strftime(buf, bufsz, "%b %e %H:%M", tm_info);
}

void
print_entries(LSEntry *entries, size_t count,
              const LSOptions *opt, const char *dirpath)
{
    size_t i;

    (void)dirpath;

    for (i = 0; i < count; i++) {
        char namebuf[LS_MAX_NAME + 1];
        char mode[11];
        char sizebuf[64];
        char timebuf[64];
        char classified[LS_MAX_NAME + 2];
        const char *name;

        name = display_name(entries[i].name,
                            namebuf, sizeof(namebuf), opt);

        if (opt->long_format) {
            mode_string(entries[i].st.st_mode, mode);

            format_size(entries[i].st.st_size,
                        opt, sizebuf, sizeof(sizebuf));

            format_time(entries[i].st.st_mtime,
                        timebuf, sizeof(timebuf));

            printf("%s %3lu %-8s %-8s %8s %s ",
                   mode,
                   (unsigned long)entries[i].st.st_nlink,
                   owner_name(&entries[i].st, opt),
                   group_name(&entries[i].st, opt),
                   sizebuf,
                   timebuf);
        }

        if (opt->inode) {
            printf("%lu ",
                   (unsigned long)entries[i].st.st_ino);
        }

        if (opt->blocks) {
            long blocks;

            blocks = (long)((entries[i].st.st_blocks * 512) / 1024);

            if (blocks < 1 && entries[i].st.st_size > 0) {
                blocks = 1;
            }

            printf("%ld ", blocks);
        }

        if (opt->classify) {
            char marker;

            marker = classify_char(&entries[i].st);

            if (marker != '\0') {
                (void)snprintf(classified,
                                sizeof(classified),
                                "%s%c", name, marker);
                printf("%s", classified);
            } else {
                printf("%s", name);
            }
        } else {
            printf("%s", name);
        }

        putchar('\n');
    }
}
