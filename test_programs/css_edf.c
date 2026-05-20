/*
 * css_edf_sigstop.c
 *
 * Proper EDF preemption test using SIGSTOP/SIGCONT.
 *
 * This creates REAL simultaneous runnable EDF tasks.
 *
 * Expected behavior:
 *
 * TASK2 (deadline=10ms) should run first
 * TASK3 (deadline=20ms) second
 * TASK1 (deadline=30ms) last
 *
 * OR:
 *
 * TASK1 starts
 * TASK2 preempts TASK1
 * TASK3 preempts TASK1
 *
 * depending on timing.
 *
 * Compile:
 *
 * aarch64-linux-gnu-gcc \
 *     -static \
 *     css_edf_sigstop.c \
 *     -o css_edf
 */

#define _GNU_SOURCE

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <signal.h>

#ifndef SCHED_EDF
#define SCHED_EDF 8
#endif

struct sched_attr {

    uint32_t size;

    uint32_t sched_policy;
    uint64_t sched_flags;

    int32_t  sched_nice;
    uint32_t sched_priority;

    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;

    /*
     * Custom EDF fields
     */
    uint64_t sched_edf_runtime;
    uint64_t sched_edf_deadline;
    uint64_t sched_edf_period;
};

#ifndef SYS_sched_setattr
#define SYS_sched_setattr 274
#endif

static int sched_setattr(pid_t pid,
                         struct sched_attr *attr,
                         unsigned int flags)
{
    return syscall(SYS_sched_setattr,
                   pid,
                   attr,
                   flags);
}

static void busy_work(int id,
                      unsigned long loops)
{
    volatile unsigned long i;

    for (i = 0; i < loops; i++) {

        if ((i % 500000000UL) == 0) {

            printf("[TASK %d] running\n", id);
        }

        __asm__ volatile("" ::: "memory");
    }
}

static void run_task(int id,
                     uint64_t runtime,
                     uint64_t deadline,
                     uint64_t period)
{
    struct sched_attr attr;

    memset(&attr, 0, sizeof(attr));

    attr.size = sizeof(attr);

    attr.sched_policy = SCHED_EDF;

    attr.sched_edf_runtime  = runtime;
    attr.sched_edf_deadline = deadline;
    attr.sched_edf_period   = period;

    printf("[TASK %d] pid=%d setting EDF\n",
           id,
           getpid());

    if (sched_setattr(0, &attr, 0) < 0) {

        perror("sched_setattr");

        exit(1);
    }

    printf("[TASK %d] runtime=%lu deadline=%lu period=%lu\n",
           id,
           runtime,
           deadline,
           period);

    printf("[TASK %d] STOPPING\n",
           id);

    /*
     * Wait until parent releases all tasks.
     */
    raise(SIGSTOP);

    printf("[TASK %d] START\n",
           id);

    busy_work(id, runtime);

    printf("[TASK %d] END\n",
           id);

    exit(0);
}

int main(void)
{
    pid_t p1, p2, p3;

    printf("\n");
    printf("=================================\n");
    printf(" EDF SIGSTOP Scheduler Test\n");
    printf("=================================\n");
    printf("\n");


struct sched_attr attr;

    memset(&attr, 0, sizeof(attr));

    attr.size = sizeof(attr);

    attr.sched_policy = SCHED_EDF;

    //make parent lowest prio so it gets preempted
    attr.sched_edf_deadline = 1000000000000ULL;
    attr.sched_edf_runtime  = 0;
    attr.sched_edf_period   = 0;

    printf("[parent task] pid=%d setting EDF\n",
           getpid());

    if (sched_setattr(0, &attr, 0) < 0) {

        perror("sched_setattr");

        exit(1);
    }

    /*
     * TASK 1
     * Lowest EDF priority
     */
    p1 = fork();

    if (p1 == 0) {

        run_task(
            1,
            4000000000ULL,
            30000000ULL,
            30000000ULL
        );
    }

    /*
     * TASK 2
     * Highest EDF priority
     */
    p2 = fork();

    if (p2 == 0) {

        run_task(
            2,
            1000000000ULL,
            10000000ULL,
            10000000ULL
        );
    }

    /*
     * TASK 3
     * Medium EDF priority
     */
    p3 = fork();

    if (p3 == 0) {

        run_task(
            3,
            2000000000ULL,
            20000000ULL,
            20000000ULL
        );
    }

    /*
     * Wait for all children to stop.
     */
    waitpid(p1, NULL, WUNTRACED);
    waitpid(p2, NULL, WUNTRACED);
    waitpid(p3, NULL, WUNTRACED);

    printf("\n");
    printf("=================================\n");
    printf(" RELEASING ALL TASKS\n");
    printf("=================================\n");
    printf("\n");

    /*
     * Release simultaneously.
     */
    kill(p1, SIGCONT);
    kill(p2, SIGCONT);
    kill(p3, SIGCONT);

 wait(NULL);
    wait(NULL);
    wait(NULL);
    printf("\n");
    printf("=================================\n");
    printf(" PARENT TASK FINISHED\n");
    printf("=================================\n");
    printf("\n");

    return 0;
}
