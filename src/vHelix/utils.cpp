#include "utils.h"

// some useful matrix- & vector operations that are missing from the default Qt classes

namespace utils
{

    QMatrix3x3 getMatrix(double gamma, double beta, double alpha)
    {
        gamma = gamma * (3.141592 / 180);
        beta = beta * (3.141592 / 180);
        alpha = -alpha * (3.141592 / 180);
        double s1 = sin(alpha);
        double s2 = sin(beta);
        double s3 = sin(gamma);
        double c1 = sin(alpha);
        double c2 = sin(beta);
        double c3 = sin(gamma);
        float a = c1 * c2;
        float b =  (c1 * s3 * s2) + (s1 * c3);
        float c = (c1 * s2 * c3) - (s1 * s3);
        float d = 0 - (s1 * c2);
        float e = 0 - (s1 * s3 * s2) + (c1 * c3);
        float f = 0 - (s1 * s3 * s2) - (c1 * s3);
        float g = 0 - s2;
        float h = s3 * c2;
        float i = c2 * c3;
        const float vals[] = {a,b,c,d,e,f,g,h,i};
        const float* vals_ptr = vals;
        return QMatrix3x3(vals_ptr);
    }

    QVector3D matrixVectorMult(QMatrix3x3 mat, QVector3D vec)
    {
        QVector3D ret;
        ret.setX(mat(0,0)*vec.x()+mat(0,1)*vec.y()+mat(0,2)*vec.z());
        ret.setX(mat(1,0)*vec.x()+mat(1,1)*vec.y()+mat(1,2)*vec.z());
        ret.setX(mat(2,0)*vec.x()+mat(2,1)*vec.y()+mat(2,2)*vec.z());
        return ret;
    }

    void rotate_vector_by_quaternion(const QVector3D& v, const QQuaternion& q, QVector3D& vprime)
    {
        QVector3D u(q.x(), q.y(), q.z());
        float s = q.scalar();
        vprime = 2.0f * QVector3D::dotProduct(u, v) * u
              + (s*s - QVector3D::dotProduct(u, u)) * v
              + 2.0f * s * QVector3D::crossProduct(u, v);
    }
}
