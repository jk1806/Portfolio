#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define WATCHDOG_TIMEOUT 30  /* seconds */
#define FEED_INTERVAL 10     /* seconds */

static volatile int watchdog_active = 1;
static time_t last_feed_time = 0;

void watchdog_timeout_handler(int sig)
{
    printf("[WATCHDOG] Timeout! System not responding\n");
    /* Take action: reboot, alert, etc. */
    exit(1);
}

void feed_watchdog(void)
{
    last_feed_time = time(NULL);
    printf("[WATCHDOG] Fed at %ld\n", last_feed_time);
}

void watchdog_init(void)
{
    struct itimerval timer;
    
    /* Setup timer */
    timer.it_value.tv_sec = WATCHDOG_TIMEOUT;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = WATCHDOG_TIMEOUT;
    timer.it_interval.tv_usec = 0;
    
    signal(SIGALRM, watchdog_timeout_handler);
    setitimer(ITIMER_REAL, &timer, NULL);
    
    feed_watchdog();
    printf("[WATCHDOG] Initialized (timeout: %d seconds)\n", WATCHDOG_TIMEOUT);
}

void watchdog_thread(void)
{
    while (watchdog_active) {
        sleep(FEED_INTERVAL);
        
        /* Check if main thread is alive */
        time_t now = time(NULL);
        if (now - last_feed_time > WATCHDOG_TIMEOUT) {
            printf("[WATCHDOG] Main thread not responding!\n");
            exit(1);
        }
        
        feed_watchdog();
    }
}

