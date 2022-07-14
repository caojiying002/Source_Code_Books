
#include <linux/rtc.h>

#include <sys/ioctl.h>

#include <sys/time.h>

#include <fcntl.h>

#include <pthread.h>

#include <linux/mman.h>


/* Read the lower half of the Pentium Time Stamp Counter
   using the rdtsc instruction */
#define rdtscl(val) __asm__ __volatile__ ("rdtsc" : "=A" (val))


int
main()
{
    unsigned long ts0, ts1, now, worst; /* Store TSC ticks */
    struct sched_param sched_p;         /* Information related to
                                           scheduling priority */
    int fd, i=0;
    unsigned long data;
    
    /* Change the scheduling policy to SCHED_FIFO */
    sched_getparam(getpid(), &sched_p);
    sched_p.sched_priority = 50; /* RT Priority */
    sched_setscheduler(getpid(), SCHED_FIFO, &sched_p);

    /* Avoid paging and related indeterminism */
    mlockall(MCL_CURRENT);

    /* Open the RTC */
    fd = open("/dev/rtc", O_RDONLY);

    /* Set the periodic interrupt frequency to 8192Hz
       This should give an interrupt rate of 122uS */
    ioctl(fd, RTC_IRQP_SET, 8192);

    /* Enable periodic interrupts */
    ioctl(fd, RTC_PIE_ON, 0);
    rdtscl(ts0);
    worst = 0;
    while (i++ < 10000) {
        /* Block until the next periodic interrupt */
        read(fd, &data, sizeof(unsigned long));
        /* Use the TSC to precisely measure the time consumed.
           Reading the lower half of the TSC is sufficient */
        rdtscl(ts1);
        now = (ts1-ts0);
        /* Update the worst case latency */
        if (now > worst) worst = now;
        ts0 = ts1;
        /* Do work that is to be done periodically */
        do_work(); /* NOP for the purpose of this measurement */
    }

    printf("Worst latency was %8ld\n", worst);

    /* Disable periodic interrupts */
    ioctl(fd, RTC_PIE_OFF, 0);
}

