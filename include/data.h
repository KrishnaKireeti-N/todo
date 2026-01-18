#define TASK_DONE 1 << 0
#define TASK_IMPORTANT 1 << 1
#define TASK_URGENT 1 << 2
#define TODO_NEXT 1 << 3
#define TODO_NEXT_LEN 30

struct task {
    char strtask[256];
    unsigned int metadata;
};

int load(const char *file, struct task **todolist);
void task_add(const char *file, char *strtask, int strtasklen, unsigned int metadata);
void task_remove(const char *file, int i);
void save(const char *file, struct task *ptodolist, int n);
