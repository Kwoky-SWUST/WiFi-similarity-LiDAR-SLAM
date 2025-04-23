#ifndef CROCO__UTILS__MEM_H
#define CROCO__UTILS__MEM_H

/** Looks up the VmPeak value in the /proc/<PID>/status file of
 *  the current process */
long getCurrentVmPeak();

#endif // CROCO__UTILS__MEM_H
