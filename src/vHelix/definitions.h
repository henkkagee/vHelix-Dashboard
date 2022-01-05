#ifndef DEFINITIONS_H
#define DEFINITIONS_H


#define _CRT_SECURE_NO_WARNINGS /* Visual Studios annoying warnings */
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define NOMINMAX /* Windows defines min, max that messes up std::min, std::max */

#define ANGLE_EPSILON (10e-05)

struct coordinates {
    double x;
    double y;
    double z;

    coordinates(double x=0, double y=0, double z=0)
        : x(x), y(y), z(z)
    {
    }

    coordinates operator-(const coordinates& a) const {
        return coordinates(x - a.x,y - a.y, z - a.z);
    }
};

#endif // DEFINITIONS_H
