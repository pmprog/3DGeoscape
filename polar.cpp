
#include "polar.h"

Polar::Polar()
{
	Longitude = 0;
	Latitude = 0;
	Radius = 1;
}

Polar::Polar( f32 longitude, f32 latitude )
{
	Longitude = longitude;
	Latitude = latitude;
	Radius = 1;
}

Polar::Polar( f32 longitude, f32 latitude, f32 radius )
{
	Longitude = longitude;
	Latitude = latitude;
	Radius = radius;
}

Polar::Polar( core::vector3df cartesian, f32 radius )
{
	Radius = radius;
	Latitude = core::radToDeg(asin( cartesian.Y / radius ));
	Longitude = core::radToDeg(atan2( cartesian.Z, cartesian.X ));
	if( Longitude < 0 )
		Longitude += 360;

}

core::vector3df Polar::ToCartesian()
{
	f32 longitude = core::degToRad(Longitude);
	f32 latitude = core::degToRad(Latitude);
	core::vector3df converted(0, 0, 0);
	converted.X = (Radius * cos(latitude) * cos(longitude));
	converted.Z = (Radius * cos(latitude) * sin(longitude));
	converted.Y = (Radius * sin(latitude));
	return converted;
}

Polar* Polar::Clone()
{
	return new Polar( Longitude, Latitude, Radius );
}