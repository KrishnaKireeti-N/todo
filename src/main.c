#include "utilities.h"
#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        FILE *help = fopen("config/help.txt", "r");

        int n;
        char buffer[64 * 1024];
        while ((n = fread(buffer, 1, sizeof(buffer), help))) {
            fwrite(buffer, 1, n, stdout);
        }

        return 0;
    }

    --argc;
    argv++;

    char todofp[128];
    if (strcmp(argv[0], ".")) {
        set_filepath(todofp, "todo.csv");
    } else {
        char buf[64 + 8];
        sprintf(buf, "todo%lu.csv", hash_str64(getcwd(buf, 32)));
        set_filepath(todofp, buf);

        --argc;
        argv++;
        if (!argc) {
            printf("No need of any 'init' just start doing your work!\n");
            return 0;
        }
    }
    // printf("%s\n", todofp);

    if (!strcmp(argv[0], "list") || !strcmp(argv[0], "-l")) {
        list(todofp);
    } else if (!strcmp(argv[0], "pipe") || !strcmp(argv[0], "-p")) {
        struct task *todolist;
        int n = load(todofp, &todolist);

        for (int i = 0; i < n; ++i) {
            printf("%s\n", todolist[i].strtask);
        }
    } else if (!strcmp(argv[0], "add") || !strcmp(argv[0], "-a")) {
        --argc;
        argv++;

        add(todofp, argv, argc);
    } else if (!strcmp(argv[0], "remove") || !strcmp(argv[0], "-r")) {
        --argc;
        argv++;

        task_remove(todofp, atoi(argv[0]));
    } else if (!strcmp(argv[0], "done") || !strcmp(argv[0], "-d")) {
        --argc;
        argv++;

        done(todofp, argc, argv);
    } else if (!strcmp(argv[0], "clear") || !strcmp(argv[0], "-c")) {
        FILE *f = fopen(todofp, "w");
        fclose(f);
    } else {
        puts("Enter 'todo' to get help");
    }
}
