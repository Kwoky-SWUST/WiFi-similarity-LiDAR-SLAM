#ifndef ALLEMANIACS3D__UTILS__FAST_SQRT_H
#define ALLEMANIACS3D__UTILS__FAST_SQRT_H

/** This function approximates the square root of f by Newton's method
 *  and is faster than the normal sqrt() function because it stops
 *  computation below a relative error of 1%.
 *  @author Philipp Vorst (DA)
 */
float fast_sqrt( float f );

#endif
