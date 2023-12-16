#include "./Mat3.h"

namespace Deep {
    Mat3& Mat3::Transpose() {
        float32 temp = m10;
        m10 = m01;
        m01 = temp;

        temp = m20;
        m20 = m02;
        m02 = temp;

        temp = m21;
        m21 = m12;
        m12 = temp;

        return *this;
    }
}