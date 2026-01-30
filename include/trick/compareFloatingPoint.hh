#ifndef COMPARE_FLOATING_POINT_HH
#define COMPARE_FLOATING_POINT_HH

/* author: John M. Penn */

namespace Trick
{

bool dbl_is_near(double A, double B, double tolerance);
bool flt_is_near(float A, float B, float tolerance);

} // namespace Trick

#endif
