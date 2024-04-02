#pragma once
#include <random>

// is there a simpler way to get a random float? 

// from 0 to 1
float randomFloat();

int randomInt(int a, int b);

float randomFloat(float a, float b);

// instead of writing individually every time all of this, this is nested in a struct that stores the single value, the range value and which one you're using
template <typename T>
struct SingleOrRandom {
	bool usingSingleValue;
	union {
		T singleValue;
		struct {
			T lowerLimit;
			T upperLimit;
		} rangeValue;
	};
};
