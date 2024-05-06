#include "./Mat4.h"

namespace Deep {
    Mat4& Mat4::Transpose() {
        float32 temp = m10;
        m10 = m01;
        m01 = temp;

        temp = m20;
        m20 = m02;
        m02 = temp;

        temp = m21;
        m21 = m12;
        m12 = temp;

        temp = m30;
        m30 = m03;
        m03 = temp;

        temp = m31;
        m31 = m13;
        m13 = temp;

        temp = m32;
        m32 = m23;
        m23 = temp;

        return *this;
    }
}