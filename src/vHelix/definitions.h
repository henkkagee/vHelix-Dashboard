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

struct orientation {
    double x;
    double y;
    double z;
    double w;
    orientation(double x = 0, double y = 0, double z = 0, double w = 0) : x(x), y(y), z(z), w(w) {}
};

typedef struct {
    std::vector<std::tuple<unsigned int,unsigned int,coordinates,orientation>> helices;
    std::vector<std::tuple<unsigned int,char *,unsigned int, char*>> connections;
    std::vector<std::string> others;

    void write(std::ostream &out) {
        for (int i = 0; i < helices.size(); i++) {
            unsigned int bases = std::get<1>(helices[i]);
            coordinates coords = std::get<2>(helices[i]);
            orientation ori = std::get<3>(helices[i]);
            out << "hb helix_" << std::get<0>(helices[i]) << " " << bases << " " << coords.x << " " << coords.y << " " << coords.z;
            out << " " << ori.x << " " << ori.y << " " << ori.z << " " << ori.w << std::endl;
        }
        out << std::endl;
        for (int i = 0; i < connections.size(); i++) {
            out << "c helix_" << std::get<0>(connections[i]) << " " << std::get<1>(connections[i]) << " helix_" << std::get<2>(connections[i]) << " " << std::get<3>(connections[i]) << std::endl;
        }
        out << std::endl;
        for (int i = 0; i < others.size(); i++) {
            out << others[i] << std::endl;
        }
    }
} rpoly;

#endif // DEFINITIONS_H
