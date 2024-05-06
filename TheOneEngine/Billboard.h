#pragma once
#include "Types.h"

enum BillboardType {
	NONE,
	SCREEN_ALIGNED,
	VIEWPORT_ORIENTED,
	AXIS_ALIGNED
};

static class Billboard {
public:
	static void BeginCylindricBillboard(vec3 objectPosition, vec3 cameraPosition);
	static mat4 CalculateSphericalBillboardRotationMatrix(const vec3& objectPosition, const vec3& cameraPosition);
};