#ifndef SPS_XMATH_H
#define SPS_XMATH_H

#include <SDL3/SDL_stdinc.h>
#include <stdalign.h>
#define SPS_ALIGN_AS(x) _Alignas(x)
#define SPS_ALIGN_MAT4 SPS_ALIGN_AS(16)
#define SPS_ALIGN_QUAT SPS_ALIGN_AS(16)
#define SPS_ALIGN_VEC4 SPS_ALIGN_AS(16)
#define SPS_ALIGN_VEC3 SPS_ALIGN_AS(8)
#define SPS_ALIGN_XFORM SPS_ALIGN_AS(16)

// Floating point vector of 2 dimensions
typedef float SPS_Vec2[2];

// Floating point vector of 3 dimensions
typedef float SPS_Vec3[3];

// Floating point vector of 4 dimensions
typedef float SPS_Vec4[4];

// Floating point quaternion: ijkr
typedef float SPS_Quat[4];

// Floating point matrix of 4x4 components, column major
typedef float SPS_Mat4[16];

// Floating point transform with rotation:quat,position:vec3,scale:vec3 components
typedef float SPS_XForm[10];
// TODO(cedmundo): Add API to get the pointers of rot,pos,sca

// Subtract two vec3 into dest
void SPS_Vec3Sub(const SPS_Vec3 a, const SPS_Vec3 b, SPS_Vec3 dest);

// Add two vec3s into dest
void SPS_Vec3Add(const SPS_Vec3 a, const SPS_Vec3 b, SPS_Vec3 dest);

// Multiplies all components by a scalar into dest
void SPS_Vec3Scale(const SPS_Vec3 a, float s, SPS_Vec3 dest);

// Dot product between two vectors
float SPS_Vec3Dot(const SPS_Vec3 a, const SPS_Vec3 b);

// Cross product between two vectors into dest
void SPS_Vec3Cross(const SPS_Vec3 a, const SPS_Vec3 b, SPS_Vec3 dest);

// Length of the given vector
float SPS_Vec3Len(const SPS_Vec3 v);

// Normalizes a vec3 into dest
void SPS_Vec3Normalize(const SPS_Vec3 src, SPS_Vec3 dest);

// Negates a vec3 into dest
void SPS_Vec3Negate(const SPS_Vec3 src, SPS_Vec3 dest);

// Creates a quaternion using axis and angle into dest
void SPS_QuatMakeAxisAngle(const SPS_Vec3 axis, float angle, SPS_Quat dest);

// Get the angle between two quaternions
float SPS_QuatAngleTo(const SPS_Quat a, const SPS_Quat b);

// Dot product between two quaternions
float SPS_QuatDot(const SPS_Quat a, const SPS_Quat b);

// Squared lenght of a quaternion (l*l), faster than lenght
float SPS_QuatLenSq(const SPS_Quat src);

// Invert a quaternion
void SPS_QuatInvert(const SPS_Quat src, SPS_Quat dest);

// Multiplies two quaternions into dest
void SPS_QuatMul(const SPS_Quat a, const SPS_Quat b, SPS_Quat dest);

// Rotate a vec3 using a quaternion
void SPS_QuatTransformVec3(const SPS_Quat q, const SPS_Vec3 v, SPS_Vec3 dest);

// Create a quaternion looking at a direction
void SPS_QuatLookRotation(const SPS_Vec3 dir, const SPS_Vec3 up, SPS_Quat dest);

// Convert a quaternion into a 4x4 matrix
void SPS_QuatToMat4(const SPS_Quat q, SPS_Mat4 dest);

// Create a prespective matrix
void SPS_Mat4Perspective(float fov, float aspect, float near, float far, SPS_Mat4 dest);

// Resize a perspective matrix using a new aspect ratio
void SPS_Mat4PerspectiveResize(const SPS_Mat4 src, float aspect, SPS_Mat4 dest);

// Invert a matrix
bool SPS_Mat4Invert(const SPS_Mat4 src, SPS_Mat4 dest);

// Multiply two matrices
void SPS_Mat4Mul(const SPS_Mat4 a, const SPS_Mat4 b, SPS_Mat4 dest);

// Convert a 4x4 matrix to a quaternion (basis matrix only)
void SPS_Mat4ToQuat(const SPS_Mat4 m, SPS_Quat dest);

// Transform a Vec4 in the space of mat4 into dest
void SPS_Mat4TransformVec4(const SPS_Mat4 m, const SPS_Vec4 v, SPS_Vec4 dest);

// Initialiaze a transform to identity
void SPS_XFormIdentity(SPS_XForm dest);

// Set the position of a transform to the given vec3
void SPS_XFormTranslate(const SPS_XForm src, const SPS_Vec3 position, SPS_XForm dest);

// Rotate the transform to look at a point in the scene
void SPS_XFormLookAtPoint(const SPS_XForm src, const SPS_Vec3 position, const SPS_Vec3 up, SPS_XForm dest);

// Create a view matrix from an transform
void SPS_XFormToView(const SPS_XForm xform, SPS_Mat4 view);

// Create a model matrix from an transform
void SPS_XFormToModel(const SPS_XForm xform, SPS_Mat4 model);

#define SPS_Rads(x) ((x)*0.01745329f)
#endif /* SPS_XMATH_H */
