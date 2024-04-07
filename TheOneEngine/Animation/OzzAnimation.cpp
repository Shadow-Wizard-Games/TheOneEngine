//#include <wipch.h>
#include "OzzAnimation.h"

void OzzAnimation::SetSkeleton(ozz::animation::Skeleton* skeleton)
{
	skeleton_ = skeleton;

	OnSkeletonSet();
}