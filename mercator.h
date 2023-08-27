#ifndef MERCATOR_H
#define MERCATOR_H

extern const double EARTH_RADIUS;

double degreeLatitudeToY(double latitude);

double degreeLongitudeToX(double longitude);

double radLatitudeToY(double radianLatitude);

double radLongitudeToX(double radianLongitude);

double yToDegreeLatitude(double y);

double xToDegreeLongitude(double x);

double yToRadLatitude(double y);

double xToRadLongitude(double x);

#endif // MERCATOR_H
