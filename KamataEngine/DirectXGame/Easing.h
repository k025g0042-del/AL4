#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

class Easing {
public:
	static float OutSine(const float time);

	static float InOutQuad(const float time);

	static float InQuad(const float time);

	static float OutQuad(const float time);

	static float Lerp(const float& start, const float& end, const float t);
};
