#include <Deep/Math.h>

namespace Deep {
    const Vec3 Vec3::zero = Vec3{ 0.0f, 0.0f, 0.0f };

    const Vec3 Vec3::up = Vec3{ 0.0f, 1.0f, 0.0f };
    const Vec3 Vec3::down = Vec3{ 0.0f, -1.0f, 0.0f };
    const Vec3 Vec3::left = Vec3{ -1.0f, 0.0f, 0.0f };
    const Vec3 Vec3::right = Vec3{ 1.0f, 0.0f, 0.0f };
    const Vec3 Vec3::forwards = Vec3{ 0.0f, 0.0f, 1.0f };
    const Vec3 Vec3::backwards = Vec3{ 0.0f, 0.0f, -1.0f };
} // namespace Deep