/*
 * DNA.cpp
 *
 *  Created on: 5 jul 2011
 *      Author: johan
 *
 *  Modified: Henrik Granö 2020
*/

#include "dna.h"
#include <algorithm>

namespace DNA {
    static const char *strands_str[] = { STRAND_NAMES };

    const char *GetStrandName(int index) {
        return strands_str[index];
    }

    bool CalculateBasePairPositions(double index, QVector3D &forward, QVector3D &backward, double offset, double totalNumBases) {
        double rad = (offset * M_PI)/180 + index * (-PITCH * M_PI)/180;
        forward.setX(ONE_MINUS_SPHERE_RADIUS * sin(rad));
        forward.setY(ONE_MINUS_SPHERE_RADIUS * cos(rad));
        forward.setZ(index * STEP + Z_SHIFT - totalNumBases * STEP / 2);
        rad += (OPPOSITE_ROTATION * M_PI)/180;
        backward.setX(ONE_MINUS_SPHERE_RADIUS * sin(rad));
        backward.setY(ONE_MINUS_SPHERE_RADIUS * cos(rad));
        backward.setZ(index * STEP + Z_SHIFT - totalNumBases * STEP / 2);
        return true;
    }
}

