#pragma once
#include "KamataEngine.h"

class AABBClass {
public:
	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};

	static bool IsCollision(const AABB& aabb1, const AABB& aabb2);
};
