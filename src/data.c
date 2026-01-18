#include "data.h"
#include "csv.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static struct task *todolist;

struct data {
    int n, taskstart;
};

static void cb_field(void *input, size_t len, void *data) {
    auto gdata = (struct data *)data;

    if (gdata->taskstart) {
        strcpy(todolist[gdata->n].strtask, (char *)input);
        gdata->taskstart = 0;
    }
    if (!gdata->taskstart) todolist[gdata->n].metadata = atoi((char *)input);
}

static void cb_row(int c, void *data) {
    auto gdata = (struct data *)data;

    gdata->n++;
    gdata->taskstart = 1;
}

int load(const char *restrict file, struct task **list) {
    struct data gdata = {.n = 0, .taskstart = 1};

    struct csv_parser parser;
    if (csv_init(&parser, CSV_STRICT | CSV_APPEND_NULL) == -1) {
        fprintf(stderr, "Failed to initialize csv_parser");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(file, "r");
    if (fp == nullptr) {
        fp = fopen(file, "w");
        if (ferror(fp)) {
            perror("load(2) : Problem faced when loading file!\n");
            exit(EXIT_FAILURE);
        }
    }
    char buffer[1024];
    size_t bytes_read, n = 10;
    todolist = calloc(n, sizeof(struct task));
    if (todolist == nullptr) {
        if (errno & ENOMEM) {
            perror("load(2) : Error allocating memory!\n");
            exit(EXIT_FAILURE);
        }
    }
    while ((bytes_read = fread(buffer, 1, 1024, fp)) > 0) {
        if (csv_parse(&parser, buffer, bytes_read, cb_field, cb_row, &gdata) != bytes_read) {
            fprintf(stderr, "Error while parsing todo file: %s\n%s", file, csv_strerror(csv_error(&parser)));
        }
        if (n == gdata.n) {
            todolist = realloc(todolist, (n = (n * 3) / 2) * sizeof(struct task));
            if (errno & ENOMEM) {
                perror("load(2) : Error allocating memory!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    csv_fini(&parser, cb_field, cb_row, &gdata);

    csv_free(&parser);
    fclose(fp);

    *list = todolist;

    return gdata.n;
}

void task_add(const char *restrict file, char *restrict strtask, int strtasklen, unsigned int metadata) {
    FILE *fp = fopen(file, "a");
    if (ferror(fp)) {
        perror("load(2) : Problem faced when loading file!\n");
        exit(EXIT_FAILURE);
    }

    csv_fwrite(fp, strtask, strtasklen - 1);
    char str[10];
    sprintf(str, ",%d\n", metadata);
    fputs(str, fp);

    fclose(fp);
}

void task_remove(const char *restrict file, int i) {
    const int n = load(file, &todolist);

    if (i > n) {
        puts("Check taskID!");
        exit(-1);
    }
    memmove(todolist + i - 1, todolist + i, ((n - i) * (sizeof(struct task))));

    save(file, todolist, n - 1);
}

void save(const char *file, struct task *list, const int n) {
    FILE *fp = fopen(file, "w");
    if (ferror(fp)) {
        perror("load(2) : Problem faced when loading file!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i) {
        csv_fwrite(fp, list[i].strtask, strlen(list[i].strtask));
        char str[10];
        sprintf(str, ",%d\n", list[i].metadata);
        fputs(str, fp);
    }

    fclose(fp);
}
