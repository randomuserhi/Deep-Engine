/**
 * Math
 *
 * All Math uses float32 and no overloads / template ability is available to use float64 etc...
 * This is because this engine is optimised with float32. Higher precision is not supported.
 */

// NOTE(randomuserhi): Math uses Left-Hand coordinate system so quaternions rotate counter-clockwise -> 
//                     https://gamedev.stackexchange.com/questions/87612/quaternion-rotation-clockwise-or-counter-clockwise

#pragma once

#include "../Deep.h"
#include "./Math/DeepMath.h"

#include "./Math/Vec3.h"
#include "./Math/Vec4.h"

#include "./Math/Mat3.h"

#include "./Math/Quaternion.h"