
#pragma once

#include <irrlicht.h>
using namespace irr;

class Polar
{
	public:
		Polar();
		Polar( f32 longitude, f32 latitude );
		Polar( f32 longitude, f32 latitude, f32 radius );
		Polar( core::vector3df cartesian, f32 radius );

		f32 Longitude;
		f32 Latitude;
		f32 Radius;

		core::vector3df ToCartesian();
		Polar* Clone();
};
