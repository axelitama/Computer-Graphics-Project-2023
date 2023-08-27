#include "mercator.h"

// #define _USE_MATH_DEFINES // needed to use math.h defines
#include <math.h>

#define M_PI 3.14159265358979323846 // pi

const double EARTH_RADIUS = 6371000.0; // Earth's radius in meters

double degreeLatitudeToY(double latitude) {
    double radianLatitude = latitude * (M_PI / 180);
    return EARTH_RADIUS * log(tan((M_PI / 4) + (radianLatitude / 2)));
}

double degreeLongitudeToX(double longitude) {
    double radianLongitude = longitude * (M_PI / 180);
    return EARTH_RADIUS * radianLongitude;
}

double radLatitudeToY(double radianLatitude) {
    return EARTH_RADIUS * log(tan((M_PI / 4) + (radianLatitude / 2)));
}

double radLongitudeToX(double radianLongitude) {
    return EARTH_RADIUS * radianLongitude;
}

double yToDegreeLatitude(double y) {
    double radianLatitude = 2 * atan(exp(y / EARTH_RADIUS)) - (M_PI / 2);
    return radianLatitude * (180 / M_PI);
}

double xToDegreeLongitude(double x) {
    double radianLongitude = x / EARTH_RADIUS;
    return radianLongitude * (180 / M_PI);
}

double yToRadLatitude(double y) {
    return 2 * atan(exp(y / EARTH_RADIUS)) - (M_PI / 2);
}

double xToRadLongitude(double x) {
    return x / EARTH_RADIUS;
}
