
/*
 *  intsimple.c
 *
 *  This module demonstrates will contain code for handling an
 *  interrupt.
 *
 *  To test is, simply run it.  Note that you may have to do something
 *  to cause the interrupts to be generated (e.g. press a key if
 *  handling the keyboard interrupt).
 *
 *    intsimple
 *
*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>

char            *progname = "intsimple";
struct sigevent int_event;  // the event to wake up the thread
int INTNUM = 1; // keyboard interrupt number
int main (int argc, char **argv)
{

    printf ("%s:  starting...\n", progname);

    // request I/O privilege
    ThreadCtl(_NTO_TCTL_IO,0);
    // set up an event for the handler or the kernel to use to wake up
    // this thread.  Use whatever type of event and event handling you want
    SIGEV_INTR_INIT(&int_event);
    // either register an interrupt handler or the event
    int id = InterruptAttachEvent(INTNUM,&int_event,_NTO_INTR_FLAGS_END|_NTO_INTR_FLAGS_PROCESS);
    while (1) {
        // block here waiting for the event
    	InterruptWait(0,NULL);
        // if using a high frequency interrupt, don't print every interrupt
        printf ("%s:  we got the event and unblocked\n", progname);

        InterruptUnmask(INTNUM,id);

    }   
}
