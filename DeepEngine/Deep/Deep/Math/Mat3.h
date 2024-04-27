#pragma once

#include "../../Deep.h"

namespace Deep {
    struct Mat3 {
        Mat3& Transpose();
        Mat3 transposed() const {
            Mat3 q{
                m00, m10, m20,
                m01, m11, m21,
                m02, m12, m22
            };
            return q;
        }

        /**
         * (00, 10, 20)
         * (01, 11, 21)
         * (02, 12, 22)
         */
        union {
            float32 values[9];
            // NOTE(randomuserhi): order of values matter for specific memory access patterns
            struct {
                float32 m00;
                float32 m01;
                float32 m02;
                float32 m10;
                float32 m11;
                float32 m12;
                float32 m20;
                float32 m21;
                float32 m22;
            };
        };
    };
}