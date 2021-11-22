#ifndef h_Deep_Vector
#define h_Deep_Vector

#include <math.h>
#include "Deep.h"

struct Deep_Vector3
{
	union
	{
		float xyz[3];
		struct
		{
			float x;
			float y;
			float z;
		};
	};
};

#define Deep_Vector3_Create(...) _Deep_Vector3_ctor(__VA_ARGS__)
Deep_Inline struct Deep_Vector3 _Deep_Vector3_ctor(float x, float y, float z)
{
	struct Deep_Vector3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

Deep_Inline int Deep_Vector3_Equal(struct Deep_Vector3 v0, struct Deep_Vector3 v1)
{
	return (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z);
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Add(struct Deep_Vector3 v0, struct Deep_Vector3 v1)
{
	return _Deep_Vector3_ctor(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Sub(struct Deep_Vector3 v0, struct Deep_Vector3 v1)
{
	return _Deep_Vector3_ctor(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Mul(struct Deep_Vector3 v0, struct Deep_Vector3 v1)
{
	return _Deep_Vector3_ctor(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z);
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Div(struct Deep_Vector3 v0, struct Deep_Vector3 v1)
{
	return _Deep_Vector3_ctor(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z);
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Scale(struct Deep_Vector3 v, float scale)
{
	return _Deep_Vector3_ctor(v.x * scale, v.y * scale, v.z * scale);
}

Deep_Inline void Deep_Vector3_Scale_InPlace(struct Deep_Vector3* v, float scale)
{
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Neg(struct Deep_Vector3 v)
{
	return Deep_Vector3_Scale(v, -1.f);
}

Deep_Inline float Deep_Vector3_Dot(struct Deep_Vector3 v0, struct Deep_Vector3 v1)
{
	float dot = (float)((v0.x * v1.x) + (v0.y * v1.y) + v0.z * v1.z);
	return dot;
}

Deep_Inline float Deep_Vector3_Dist(struct Deep_Vector3 a, struct Deep_Vector3 b)
{
	float dx = (a.x - b.x);
	float dy = (a.y - b.y);
	float dz = (a.z - b.z);
	return (float)(sqrt(dx * dx + dy * dy + dz * dz));
}

Deep_Inline float Deep_Vector3_Mag(struct Deep_Vector3 v)
{
	return (float)sqrt(Deep_Vector3_Dot(v, v));
}

Deep_Inline float Deep_Vector3_MagSqrt(struct Deep_Vector3 v)
{
	return (float)(Deep_Vector3_Dot(v, v));
}

Deep_Inline struct Deep_Vector3 Deep_Vector3_Norm(struct Deep_Vector3 v)
{
	float len = Deep_Vector3_Mag(v);
	return len == 0.f ? v : Deep_Vector3_Scale(v, 1.f / len);
}

#endif

