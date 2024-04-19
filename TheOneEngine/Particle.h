#pragma once
// only has information of itself
// the emmiter can change part of its values

// instead of having initColor and endColor, having deltaColor and multiplying it with dt and adding it to the color

#include "Types.h"

class Particle {
public:
	// pass the duration of the particle and calculate other values needed
	void SetDuration(float duration);

	// reset all attributes other than duration-related ones
	void ResetAttributes();

	void Update(double dt);

	float duration;
	float lifetime;

	float lifetimeOverOne;
	float dtOverOne;

	vec4 color;

	vec3 position;

	// normalized
	vec3 direction;
	float speed;

	vec4 rotation;

	vec3 scale;
};