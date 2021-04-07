#ifndef UTILS_H
#define UTILS_H

#include "QVector3D"
#include "QMatrix3x3"
#include "QQuaternion"

namespace utils
{
    QMatrix3x3 getMatrix(double gamma, double beta, double alpha);

    QVector3D matrixVectorMult(QMatrix3x3 mat, QVector3D vec);

    void rotate_vector_by_quaternion(const QVector3D &v, const QQuaternion& q, QVector3D& vprime);
}

#endif
