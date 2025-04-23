#ifndef CROCO__UTILS__PROCESSOR_H
#define CROCO__UTILS__PROCESSOR_H

#include <string>


/** Get the processor model */
std::string getProcessorModelInfo();

/** Get the number of processors */
std::string getNumberOfProcessors();

/** Get the CPU frequency */
std::string getCPUMHz();

#endif // CROCO__UTILS__PROCESSOR_H
