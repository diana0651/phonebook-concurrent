#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "phonebook_opt.h"
#include "debug.h"

entry *findName(char lastname[], entry *pHead)
{
    size_t len = strlen(lastname);
    entry *current = pHead;
    while (current != NULL) {
        if (strncasecmp(lastname, current->lastName, len) == 0
                && (current->lastName[len] == '\n' ||
                    current->lastName[len] == '\0')) {
            current->lastName = (char *) malloc(sizeof(char) *
                                                MAX_LAST_NAME_SIZE);
            memset(current->lastName, '\0', MAX_LAST_NAME_SIZE);
            strcpy(current->lastName, lastname);
            current->dtl = (pdetail) malloc(sizeof(detail));
            return current;
        }
        // dprintf("find string = %s\n", current->lastName);
        current = current->pNext;
    }
    return NULL;
}

append_a *new_append_a(char *startptr, char *endptr, int tid, int ntd,
                       entry *start)
{
    append_a *app = (append_a *) malloc(sizeof(append_a));

    app->ptr = startptr;
    app->eptr = endptr;
    app->tid = tid;
    app->nthread = ntd;
    app->entryStart = start;

    app->pHead = (app->pLast = app->entryStart);
    return app;
}

void append(void *arg)
{
    struct timespec start, end;
    // double cpu_time;

    clock_gettime(CLOCK_REALTIME, &start);

    append_a *app = (append_a *) arg;

    int count = 0;
    entry *current = app->entryStart;
    for (char *i = app->ptr; i < app->eptr;
            i += MAX_LAST_NAME_SIZE * app->nthread,
            current += app->nthread,count++) {
        app->pLast->pNext = current;
        app->pLast = app->pLast->pNext;

        app->pLast->lastName = i;
        // dprintf("thread %d append string = %s\n",
        //         app->tid, app->pLast->lastName);
        app->pLast->pNext = NULL;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    // cpu_time = diff_in_second(start, end);

    // dprintf("thread take %lf sec, count %d\n", cpu_time, count);

    pthread_exit(NULL);
}

void show_entry(entry *pHead)
{
    while (pHead != NULL) {
        printf("lastName = %s\n", pHead->lastName);
        pHead = pHead->pNext;
    }
}
/*
static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}*/

