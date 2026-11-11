#include "Easing.h"

float Easing::OutSine(const float time) { return static_cast<float>(sin((time * M_PI) / 2)); }

float Easing::InOutQuad(const float time) {
	if (time < 0.5f) {
		return 2.0f * time * time;
	} else {
		return 1.0f - powf(-2.0f * time + 2.0f, 2.0f) / 2.0f;
	}
}

float Easing::InQuad(const float time) { return time * time; }

float Easing::OutQuad(const float time) { return 1 - (1 - time) * (1 - time); }

float Easing::Lerp(const float& start, const float& end, const float t) { return (1.0f - t) * start + t * end; }