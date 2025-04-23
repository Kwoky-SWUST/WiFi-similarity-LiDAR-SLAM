#ifndef SCIROCO__UTILS__RANDOM_H
#define SCIROCO__UTILS__RANDOM_H


/** This function generates a random number from the uniform distrubution
 *  over [0.0, 1.0). Is is based on the standard C++ random routine.
 *
 *  @author Philipp Vorst
 */
double random01();


/** This function generates a random number from the uniform distrubution
 *  over [-1.0, 1.0). Is is based on the standard C++ random routine.
 *
 *  @author Philipp Vorst
 */
double random11();

/** Generate a random seed based on the current time
 *  @author Philipp Vorst
 */
void sciroco_generate_time_based_random_seed();

#endif // SCIROCO__UTILS__RANDOM_H
