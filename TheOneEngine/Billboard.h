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

	static void BeginSphericalBillboard(vec3 objectPosition, vec3 cameraPosition);

	static void EndBillboard();

};