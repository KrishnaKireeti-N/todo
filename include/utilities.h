#include "ansi_color.h"
#include "data.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/*
        No header+source combo so that the compiler inlines the functions
*/

/* ----------------------------------------------Helper Functions-------------------------------------- */
static inline void set_filepath(char *restrict fpath, const char *restrict fname) {
#if defined(__linux__)
    const char *home = getenv("HOME");
    if (!home) {
        strcpy(fpath, fname);
        return;
    }
    strcpy(fpath, home);

    strcat(fpath, "/.local/share/todo/");
    if (mkdir(fpath, 0700) && errno != EEXIST) {
        perror("mkdir");
        exit(1);
    };

    strcat(fpath, fname);
#else
    char *todofp = "todo.csv";
#endif
}

static inline uint64_t hash_str64(const char *restrict str) {
    uint64_t hash = 2166136261u;
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619u;
    }
    return hash;
}

// For getcwd(2) function
#if defined(__linux__)
#include <unistd.h>
#else
// UNIMPLEMENTED
static char *getcwd(char *buf, size_t n) {
    return nullptr;
}
#endif

static inline unsigned long nstrcat(char *restrict dest, const char *restrict const *restrict start, const int n) {
    int index = 0, len;
    for (int i = 0; i != n; ++i) {
        len = strlen(start[i]);
        strncpy(dest + index, start[i], len);
        index += len;
        dest[index++] = ' ';
    }
    dest[index - 1] = '\0';
    return index;
};

static inline int compar_int_r(const void *a, const void *b) {
    return -(*(int *)a - *(int *)b);
}
/* ---------------------------------------------------------------------------------------------------- */

static inline void list(const char *restrict todofp) {
    struct task *todolist;
    const int n = load(todofp, &todolist);

    int j = 1, i;
    for (i = 0; i != n; ++i) {
        if (todolist[i].metadata & TASK_DONE) {
        } else {
            auto color = ANSI_COLOR_WHITE;
            const auto imp = (todolist[i].metadata & TASK_IMPORTANT);
            const auto urgent = (todolist[i].metadata & TASK_URGENT);

            color = imp ? ANSI_COLOR_GREEN : (urgent ? ANSI_COLOR_CYAN : color);
            color = imp && urgent ? ANSI_COLOR_RED_BOLD : color;

            printf("[%d]: %s%s\n" ANSI_COLOR_RESET, i + 1, color, todolist[i].strtask);
            j++;
        }
    }

    printf("\n");
    printf("----------------------------------------\n");
    printf("Progress : %f\n", ((float)(n - (j - 1)) / (float)n) * 100);
    printf("----------------------------------------\n");
    printf("\n");

    for (i = 0; i != n; ++i) {
        if (todolist[i].metadata & TASK_DONE) {
            printf("[=]: %s\n", todolist[i].strtask);
        }
    }
}

static inline void add(const char *restrict todofp, const char *const *argv, int argc) {
    // input : strtask
    char task[256];
    const int n = nstrcat(task, argv, argc);

    // input : metadata
    unsigned int metadata = 0;
    char c;

    printf("Important[y]? ");
    if (scanf("%c", &c))
        while (fgetc(stdin) != '\n');
    metadata |= (c == 'y' ? TASK_IMPORTANT : metadata);

    printf("Urgent[y]? ");
    if (scanf("%c", &c) != 1)
        while (fgetc(stdin) != '\n');
    metadata |= (c == 'y' ? TASK_URGENT : metadata);

    task_add(todofp, task, n, metadata);
}

static inline void done(const char *restrict todofp, const int argc, const char *const *argv) {
    int indexs[argc];
    struct task *todolist;
    const int n = load(todofp, &todolist);

    for (int i = 0; i != argc; ++i) {
        indexs[i] = atoi(argv[i]) - 1;
        if (indexs[i] > n) {
            puts("Enter a valid task ID to be removed!");
            exit(0);
        }
    }

    for (int i = 0; i != argc; ++i) {
        todolist[indexs[i]].metadata |= TASK_DONE;
    }

    save(todofp, todolist, n);
}
