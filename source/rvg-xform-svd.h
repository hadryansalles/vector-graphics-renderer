#ifndef RVG_XFORM_SVD_H
#define RVG_XFORM_SVD_H

#include "rvg-xform.h"

namespace rvg {

// Computes something that is almost the SVD decomposition A
// The only difference is that we require U to be a rotation matrix.
// I.e., it must have determinant 1.
// In cases where the SVD would return U with determinant of -1, we simply
// negate the second column of U and the second line of S.
void asvd(const linear &A, rotation &U, scaling &S, linear &Vt);
void asvd(const linear &A, rotation &U, scaling &S);

} // namespace rvg::xform

#endif
