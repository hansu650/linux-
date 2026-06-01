/*
 * task2_cpu_shapes_showcase.c
 * ------------------------------------------------------------
 * Linux System and Analysis - Task 2
 * Multi-threaded CPU Utilization Control
 *
 * This file is a merged and optimized version based on the original
 * CPU heart-shape program and the CPU curve program provided in class.
 *
 * Main idea:
 *   CPU Usage = Busy Computing Time / Total Time
 *
 * In every short sampling period, a worker thread first runs a busy loop
 * to consume CPU time, and then sleeps until the end of the period.
 *
 *   busy_time = period_time * target_cpu_usage
 *   idle_time = period_time - busy_time
 *
 * By changing target_cpu_usage with different mathematical formulas, the
 * CPU curves in System Monitor can show a heart, a straight line, a sine
 * wave, a triangle wave, a step wave, and other shapes.
 *
 * Why pthread is used:
 *   Each worker thread controls one CPU core.  The thread is bound to a
 *   specified CPU core by pthread_setaffinity_np(), so the curve appears
 *   on a fixed CPU line in System Monitor instead of jumping between cores.
 *
 * Compile:
 *   gcc task2_cpu_shapes_showcase.c -o task2_cpu_shapes_showcase -pthread -lm
 *
 * Recommended commands:
 *
 *   1) Formal Task 2 screenshot: only the heart shape on CPU1 and CPU2
 *      ./task2_cpu_shapes_showcase "Qin Tian" "202431123002054" 2
 *
 *   2) Teacher's 4-curve check: heart + 50% line + sine wave
 *      ./task2_cpu_shapes_showcase "Qin Tian" "202431123002054" 4
 *
 *   3) Extra exploration on CPU5, CPU6, CPU7 and CPU8
 *      ./task2_cpu_shapes_showcase "Qin Tian" "202431123002054" extra
 *
 *   4) All eight curves at the same time.  This is mainly for trying.
 *      It can look crowded, so it is not recommended as the main report image.
 *      ./task2_cpu_shapes_showcase "Qin Tian" "202431123002054" 8
 *
 * Optional manual CPU list:
 *   The CPU numbers in the list are Linux CPU IDs, starting from 0.
 *   For example, CPU ID 0 is shown as CPU1 in System Monitor.
 *
 *      ./task2_cpu_shapes_showcase "Qin Tian" "202431123002054" 2 0,5
 *      ./task2_cpu_shapes_showcase "Qin Tian" "202431123002054" 4 0,5,2,6
 *
 * Stop:
 *   Press Ctrl+C in the terminal.
 *
 * Notes for screenshots:
 *   - Keep this terminal visible, because it prints the name, student ID,
 *     PID, mode, and CPU mapping.
 *   - Close unnecessary applications before taking screenshots.
 *   - System Monitor colors are decided by the monitor itself.  This C code
 *     cannot directly set line colors; it can only choose which CPU cores run.
 */

#define _GNU_SOURCE

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SAMPLE_COUNT      800                 /* 800 samples in one complete cycle */
#define SAMPLE_PERIOD_NS  25000000LL          /* 25 ms per sample, 800 * 25 ms = 20 s */
#define MAX_WORKERS       8
#define PI                3.14159265358979323846

/* Keep a margin away from exact 0% and 100%.
 * The System Monitor curve is usually clearer when it does not keep touching
 * the top or bottom border. */
#define MIN_USAGE         0.04
#define MAX_USAGE         0.96

typedef enum {
    SHAPE_HEART_A = 0,
    SHAPE_HEART_B,
    SHAPE_LINE_50,
    SHAPE_SINE,
    SHAPE_TRIANGLE,
    SHAPE_DOUBLE_WAVE,
    SHAPE_STAIR,
    SHAPE_DOUBLE_PULSE
} shape_t;

typedef struct {
    shape_t shape;
    const char *name;
} shape_plan_t;

typedef struct {
    int thread_index;
    int cpu_id;
    shape_t shape;
    const char *shape_name;
} worker_arg_t;

typedef enum {
    MODE_HEART2 = 0,
    MODE_CHECK4,
    MODE_EXTRA5678,
    MODE_ALL8
} display_mode_t;

static volatile sig_atomic_t g_stop = 0;
static volatile sig_atomic_t g_start = 0;

/* Fixed plans.  Nothing is automatically generated from the CPU count. */
static const shape_plan_t PLAN_2[2] = {
    {SHAPE_HEART_A, "Heart curve - part A"},
    {SHAPE_HEART_B, "Heart curve - part B"}
};

static const shape_plan_t PLAN_4[4] = {
    {SHAPE_HEART_A, "Heart curve - part A"},
    {SHAPE_HEART_B, "Heart curve - part B"},
    {SHAPE_LINE_50, "50% straight line"},
    {SHAPE_SINE,    "Sine curve"}
};

static const shape_plan_t PLAN_EXTRA[4] = {
    {SHAPE_TRIANGLE,     "Triangle curve"},
    {SHAPE_DOUBLE_WAVE,  "Smooth double-wave curve"},
    {SHAPE_STAIR,        "Step curve"},
    {SHAPE_DOUBLE_PULSE, "Double-pulse curve"}
};

static const shape_plan_t PLAN_8[8] = {
    {SHAPE_HEART_A,      "Heart curve - part A"},
    {SHAPE_HEART_B,      "Heart curve - part B"},
    {SHAPE_LINE_50,      "50% straight line"},
    {SHAPE_SINE,         "Sine curve"},
    {SHAPE_TRIANGLE,     "Triangle curve"},
    {SHAPE_DOUBLE_WAVE,  "Smooth double-wave curve"},
    {SHAPE_STAIR,        "Step curve"},
    {SHAPE_DOUBLE_PULSE, "Double-pulse curve"}
};

static void handle_signal(int signo)
{
    (void)signo;
    g_stop = 1;
}

static int online_cpu_count(void)
{
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 1) {
        return 1;
    }
    if (n > CPU_SETSIZE) {
        return CPU_SETSIZE;
    }
    return (int)n;
}

static int64_t now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec;
}

static struct timespec ns_to_timespec(int64_t ns)
{
    struct timespec ts;
    ts.tv_sec = (time_t)(ns / 1000000000LL);
    ts.tv_nsec = (long)(ns % 1000000000LL);
    if (ts.tv_nsec < 0) {
        ts.tv_nsec += 1000000000L;
        ts.tv_sec -= 1;
    }
    return ts;
}

static void sleep_until_ns(int64_t deadline_ns)
{
    struct timespec deadline = ns_to_timespec(deadline_ns);

    while (!g_stop) {
        int rc = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
        if (rc == 0) {
            break;
        }
        if (rc != EINTR) {
            break;
        }
    }
}

static void cpu_relax(void)
{
#if defined(__x86_64__) || defined(__i386__)
    __asm__ __volatile__("pause" ::: "memory");
#else
    __asm__ __volatile__("" ::: "memory");
#endif
}

static void busy_until_ns(int64_t deadline_ns)
{
    while (!g_stop && now_ns() < deadline_ns) {
        cpu_relax();
    }
}

static double clamp_double(double value, double low, double high)
{
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static double apply_usage_margin(double value)
{
    return clamp_double(value, MIN_USAGE, MAX_USAGE);
}

/*
 * The two heart functions keep the original four-piece heart idea.
 * x is in the range [0, 2).
 */
static double heart_part_a(double x)
{
    double y;

    if (x < 0.5) {
        y = -8.0 * (x - 0.25) * (x - 0.25) + 1.0;
    } else if (x < 1.0) {
        y = -8.0 * (x - 0.75) * (x - 0.75) + 1.0;
    } else if (x < 1.5) {
        y = 0.667 * (x - 2.0) * (x - 2.0) - 0.167;
    } else {
        y = 0.667 * (x - 1.0) * (x - 1.0) - 0.167;
    }

    return apply_usage_margin(y);
}

static double heart_part_b(double x)
{
    double y;

    if (x < 0.5) {
        y = 0.667 * (x - 1.0) * (x - 1.0) - 0.167;
    } else if (x < 1.0) {
        y = 0.667 * x * x - 0.167;
    } else if (x < 1.5) {
        y = -8.0 * (x - 1.25) * (x - 1.25) + 1.0;
    } else {
        y = -8.0 * (x - 1.75) * (x - 1.75) + 1.0;
    }

    return apply_usage_margin(y);
}

/* Distance on a circular [0, 1) time axis, used by the pulse curve. */
static double circular_distance(double a, double b)
{
    double d = fabs(a - b);
    return (d > 0.5) ? (1.0 - d) : d;
}

static double target_usage(shape_t shape, int sample_index)
{
    double phase = (double)sample_index / (double)SAMPLE_COUNT;  /* [0, 1) */
    double angle = 2.0 * PI * phase;
    double x = 2.0 * phase;                                      /* [0, 2) */
    double y;

    switch (shape) {
    case SHAPE_HEART_A:
        y = heart_part_a(x);
        break;

    case SHAPE_HEART_B:
        y = heart_part_b(x);
        break;

    case SHAPE_LINE_50:
        y = 0.50;
        break;

    case SHAPE_SINE:
        /* Smooth sine wave, about 10% to 90%. */
        y = 0.50 + 0.40 * sin(angle);
        break;

    case SHAPE_TRIANGLE:
        /* One clean mountain shape in a 20-second cycle. */
        if (phase < 0.5) {
            y = 0.12 + 0.76 * (2.0 * phase);
        } else {
            y = 0.12 + 0.76 * (2.0 * (1.0 - phase));
        }
        break;

    case SHAPE_DOUBLE_WAVE:
        /* A smooth two-peak wave.  It is less sharp than a square wave. */
        y = 0.50 + 0.28 * sin(2.0 * angle) + 0.10 * sin(4.0 * angle);
        break;

    case SHAPE_STAIR: {
        /* A step curve: easy to explain as discrete busy/idle ratios. */
        static const double level[8] = {0.15, 0.28, 0.42, 0.58, 0.82, 0.58, 0.42, 0.28};
        int k = (int)(phase * 8.0);
        if (k < 0) {
            k = 0;
        }
        if (k > 7) {
            k = 7;
        }
        y = level[k];
        break;
    }

    case SHAPE_DOUBLE_PULSE: {
        /* Two separated smooth pulses. */
        double d1 = circular_distance(phase, 0.25);
        double d2 = circular_distance(phase, 0.75);
        double sigma = 0.055;
        double p1 = exp(-(d1 * d1) / (2.0 * sigma * sigma));
        double p2 = exp(-(d2 * d2) / (2.0 * sigma * sigma));
        y = 0.12 + 0.72 * (p1 + p2);
        break;
    }

    default:
        y = 0.50;
        break;
    }

    return apply_usage_margin(y);
}

static int bind_thread_to_cpu(pthread_t thread, int cpu_id)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu_id, &set);
    return pthread_setaffinity_np(thread, sizeof(set), &set);
}

static int bind_current_thread_to_cpu(int cpu_id)
{
    return bind_thread_to_cpu(pthread_self(), cpu_id);
}

static int cpu_is_used(int cpu_id, const int *cpu_ids, int count)
{
    int i;
    for (i = 0; i < count; ++i) {
        if (cpu_ids[i] == cpu_id) {
            return 1;
        }
    }
    return 0;
}

static int choose_main_cpu(const int *worker_cpu_ids, int worker_count, int cpu_count)
{
    int cpu;

    /* Prefer the last unused CPU, because the first CPUs are usually used for curves. */
    for (cpu = cpu_count - 1; cpu >= 0; --cpu) {
        if (!cpu_is_used(cpu, worker_cpu_ids, worker_count)) {
            return cpu;
        }
    }

    return -1;
}

static void *worker_main(void *arg_ptr)
{
    worker_arg_t *arg = (worker_arg_t *)arg_ptr;

    int rc = bind_current_thread_to_cpu(arg->cpu_id);
    if (rc != 0) {
        fprintf(stderr,
                "Warning: thread %d failed to bind to CPU ID %d: %s\n",
                arg->thread_index,
                arg->cpu_id,
                strerror(rc));
    }

#if defined(__linux__)
    char thread_name[16];
    snprintf(thread_name, sizeof(thread_name), "cpu_shape_%d", arg->thread_index);
    pthread_setname_np(pthread_self(), thread_name);
#endif

    /* Wait until all threads have been created, then start together. */
    while (!g_start && !g_stop) {
        struct timespec small_sleep = {0, 1000000L};  /* 1 ms */
        nanosleep(&small_sleep, NULL);
    }

    int sample_index = 0;
    int64_t period_start = now_ns();

    while (!g_stop) {
        /*
         * This is the key part of Task 2.
         * In one period, the thread first keeps the CPU busy, then sleeps.
         *
         *     cpu_usage = busy_time / total_period_time
         *     busy_time = total_period_time * cpu_usage
         *     idle_time = total_period_time - busy_time
         *
         * Different shapes only change the target cpu_usage value.
         * The busy/idle control method is the same for every worker thread.
         */
        double cpu_usage = target_usage(arg->shape, sample_index);
        int64_t busy_time_ns = (int64_t)((double)SAMPLE_PERIOD_NS * cpu_usage);
        int64_t idle_time_ns = SAMPLE_PERIOD_NS - busy_time_ns;
        int64_t busy_end = period_start + busy_time_ns;
        int64_t period_end = busy_end + idle_time_ns;

        busy_until_ns(busy_end);      /* busy part: occupy CPU */
        sleep_until_ns(period_end);   /* idle part: release CPU */

        sample_index = (sample_index + 1) % SAMPLE_COUNT;
        period_start = period_end;

        /* If the desktop freezes for a moment, reset timing instead of catching up. */
        int64_t current = now_ns();
        if (current - period_start > 2 * SAMPLE_PERIOD_NS) {
            period_start = current;
        }
    }

    return NULL;
}

static int same_text(const char *a, const char *b)
{
    return strcmp(a, b) == 0;
}

static int parse_mode(const char *text, display_mode_t *mode)
{
    if (same_text(text, "2") || same_text(text, "heart") || same_text(text, "heart2")) {
        *mode = MODE_HEART2;
        return 0;
    }
    if (same_text(text, "4") || same_text(text, "check") || same_text(text, "check4")) {
        *mode = MODE_CHECK4;
        return 0;
    }
    if (same_text(text, "extra") || same_text(text, "5678") || same_text(text, "5-8")) {
        *mode = MODE_EXTRA5678;
        return 0;
    }
    if (same_text(text, "8") || same_text(text, "all") || same_text(text, "all8")) {
        *mode = MODE_ALL8;
        return 0;
    }
    return -1;
}

static const char *mode_name(display_mode_t mode)
{
    switch (mode) {
    case MODE_HEART2:
        return "2-core heart mode";
    case MODE_CHECK4:
        return "4-core teacher check mode";
    case MODE_EXTRA5678:
        return "extra exploration mode on CPU5-CPU8";
    case MODE_ALL8:
        return "all-eight-curves mode";
    default:
        return "unknown mode";
    }
}

static void get_plan(display_mode_t mode, const shape_plan_t **plan, int *count)
{
    switch (mode) {
    case MODE_HEART2:
        *plan = PLAN_2;
        *count = 2;
        break;
    case MODE_CHECK4:
        *plan = PLAN_4;
        *count = 4;
        break;
    case MODE_EXTRA5678:
        *plan = PLAN_EXTRA;
        *count = 4;
        break;
    case MODE_ALL8:
        *plan = PLAN_8;
        *count = 8;
        break;
    default:
        *plan = PLAN_2;
        *count = 2;
        break;
    }
}

static int parse_cpu_list(const char *text, int *cpu_ids, int expected_count, int cpu_count)
{
    char buffer[256];
    char *token;
    char *saveptr = NULL;
    int used[CPU_SETSIZE] = {0};
    int count = 0;

    if (strlen(text) >= sizeof(buffer)) {
        return -1;
    }

    strcpy(buffer, text);
    token = strtok_r(buffer, ",", &saveptr);
    while (token != NULL) {
        char *endptr = NULL;
        long id;

        if (count >= expected_count) {
            return -1;
        }

        id = strtol(token, &endptr, 10);
        if (endptr == token || *endptr != '\0') {
            return -1;
        }
        if (id < 0 || id >= cpu_count) {
            return -1;
        }
        if (used[id]) {
            return -1;
        }

        cpu_ids[count++] = (int)id;
        used[id] = 1;
        token = strtok_r(NULL, ",", &saveptr);
    }

    return (count == expected_count) ? 0 : -1;
}

static void default_cpu_mapping(display_mode_t mode, int *cpu_ids, int worker_count, int cpu_count)
{
    int i;

    if (mode == MODE_EXTRA5678 && cpu_count >= 8) {
        /* Linux CPU IDs 4,5,6,7 are shown as CPU5, CPU6, CPU7, CPU8. */
        cpu_ids[0] = 4;
        cpu_ids[1] = 5;
        cpu_ids[2] = 6;
        cpu_ids[3] = 7;
        return;
    }

    for (i = 0; i < worker_count; ++i) {
        cpu_ids[i] = i;
    }
}

static void print_usage(const char *program)
{
    fprintf(stderr,
            "Usage:\n"
            "  %s \"Qin Tian\" \"202431123002054\" 2\n"
            "  %s \"Qin Tian\" \"202431123002054\" 4\n"
            "  %s \"Qin Tian\" \"202431123002054\" extra\n"
            "  %s \"Qin Tian\" \"202431123002054\" 8\n"
            "\n"
            "Optional manual CPU list, Linux CPU IDs start from 0:\n"
            "  %s \"Qin Tian\" \"202431123002054\" 2 0,5\n"
            "  %s \"Qin Tian\" \"202431123002054\" 4 0,5,2,6\n",
            program, program, program, program, program, program);
}

int main(int argc, char **argv)
{
    const char *student_name;
    const char *student_id;
    display_mode_t mode;
    const shape_plan_t *plan = NULL;
    int worker_count = 0;
    int cpu_count = online_cpu_count();
    int worker_cpu_ids[MAX_WORKERS];
    pthread_t threads[MAX_WORKERS];
    worker_arg_t args[MAX_WORKERS];
    int i;
    int rc;
    int main_cpu;

    if (argc < 4 || argc > 5) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    student_name = argv[1];
    student_id = argv[2];

    if (parse_mode(argv[3], &mode) != 0) {
        fprintf(stderr, "Error: unknown mode '%s'.\n\n", argv[3]);
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    get_plan(mode, &plan, &worker_count);

    if (worker_count > cpu_count) {
        fprintf(stderr,
                "Error: this mode needs %d CPU cores, but only %d cores are online.\n",
                worker_count,
                cpu_count);
        return EXIT_FAILURE;
    }

    default_cpu_mapping(mode, worker_cpu_ids, worker_count, cpu_count);

    if (argc == 5) {
        if (parse_cpu_list(argv[4], worker_cpu_ids, worker_count, cpu_count) != 0) {
            fprintf(stderr,
                    "Error: CPU list '%s' is invalid. It must contain %d unique CPU IDs in [0, %d].\n",
                    argv[4],
                    worker_count,
                    cpu_count - 1);
            return EXIT_FAILURE;
        }
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    main_cpu = choose_main_cpu(worker_cpu_ids, worker_count, cpu_count);
    if (main_cpu >= 0) {
        rc = bind_current_thread_to_cpu(main_cpu);
        if (rc != 0) {
            fprintf(stderr, "Warning: failed to bind main thread to CPU ID %d: %s\n",
                    main_cpu,
                    strerror(rc));
        }
    }

    printf("============================================================\n");
    printf("Linux System and Analysis - Task 2\n");
    printf("Multi-threaded CPU Utilization Control\n");
    printf("Name: %s\n", student_name);
    printf("Student ID: %s\n", student_id);
    printf("PID: %ld\n", (long)getpid());
    printf("Online CPU cores: %d\n", cpu_count);
    printf("Mode: %s\n", mode_name(mode));
    printf("Principle: CPU Usage = Busy Computing Time / Total Time\n");
    if (main_cpu >= 0) {
        printf("Main thread is moved to Linux CPU ID %d, shown as CPU%d in System Monitor.\n",
               main_cpu,
               main_cpu + 1);
    } else {
        printf("Main thread is not moved because all online CPUs are used by worker threads.\n");
    }
    printf("------------------------------------------------------------\n");
    printf("Worker mapping:\n");

    for (i = 0; i < worker_count; ++i) {
        printf("  Thread %d -> Linux CPU ID %d (System Monitor CPU%d) -> %s\n",
               i + 1,
               worker_cpu_ids[i],
               worker_cpu_ids[i] + 1,
               plan[i].name);
    }

    printf("------------------------------------------------------------\n");
    printf("To check threads in another terminal:\n");
    printf("  ps -T -p %ld\n", (long)getpid());
    printf("Stop this program with Ctrl+C.\n");
    printf("============================================================\n");
    fflush(stdout);

    for (i = 0; i < worker_count; ++i) {
        args[i].thread_index = i + 1;
        args[i].cpu_id = worker_cpu_ids[i];
        args[i].shape = plan[i].shape;
        args[i].shape_name = plan[i].name;

        rc = pthread_create(&threads[i], NULL, worker_main, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "Error: pthread_create failed for thread %d: %s\n",
                    i + 1,
                    strerror(rc));
            g_stop = 1;
            while (--i >= 0) {
                pthread_join(threads[i], NULL);
            }
            return EXIT_FAILURE;
        }
    }

    g_start = 1;

    for (i = 0; i < worker_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("\nProgram stopped. Threads have been joined and released.\n");
    return EXIT_SUCCESS;
}
