#include "MathFunction.h"
#define _USE_MATH_DEFINES
#include <math.h>

float MathFunction::DegreeToRadian(float degree) { return degree * static_cast<float>(M_PI / 180); }
