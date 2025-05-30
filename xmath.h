#ifndef SOB_XMATH_H
#define SOB_XMATH_H

#include <SDL3/SDL_stdinc.h>
#include <stdalign.h>
#define SOB_ALIGN_AS(x) _Alignas(x)
#define SOB_ALIGN_MAT4 SOB_ALIGN_AS(16)
#define SOB_ALIGN_QUAT SOB_ALIGN_AS(16)
#define SOB_ALIGN_VEC4 SOB_ALIGN_AS(16)
#define SOB_ALIGN_VEC3 SOB_ALIGN_AS(8)
#define SOB_ALIGN_XFORM SOB_ALIGN_AS(16)

// Floating point vector of 2 dimensions
typedef float SOB_Vec2[2];

// Floating point vector of 3 dimensions
typedef float SOB_Vec3[3];

// Floating point vector of 4 dimensions
typedef float SOB_Vec4[4];

// Floating point quaternion: ijkr
typedef float SOB_Quat[4];

// Floating point matrix of 4x4 components, column major
typedef float SOB_Mat4[16];

// Floating point transform with rotation:quat,position:vec3,scale:vec3 components
typedef float SOB_XForm[10];
// TODO(cedmundo): Add API to get the pointers of rot,pos,sca

// Subtract two vec3 into dest
void SOB_Vec3Sub(const SOB_Vec3 a, const SOB_Vec3 b, SOB_Vec3 dest);

// Add two vec3s into dest
void SOB_Vec3Add(const SOB_Vec3 a, const SOB_Vec3 b, SOB_Vec3 dest);

// Multiplies all components by a scalar into dest
void SOB_Vec3Scale(const SOB_Vec3 a, float s, SOB_Vec3 dest);

// Dot product between two vectors
float SOB_Vec3Dot(const SOB_Vec3 a, const SOB_Vec3 b);

// Cross product between two vectors into dest
void SOB_Vec3Cross(const SOB_Vec3 a, const SOB_Vec3 b, SOB_Vec3 dest);

// Length of the given vector
float SOB_Vec3Len(const SOB_Vec3 v);

// Normalizes a vec3 into dest
void SOB_Vec3Normalize(const SOB_Vec3 src, SOB_Vec3 dest);

// Negates a vec3 into dest
void SOB_Vec3Negate(const SOB_Vec3 src, SOB_Vec3 dest);

// Creates a quaternion using axis and angle into dest
void SOB_QuatMakeAxisAngle(const SOB_Vec3 axis, float angle, SOB_Quat dest);

// Get the angle between two quaternions
float SOB_QuatAngleTo(const SOB_Quat a, const SOB_Quat b);

// Dot product between two quaternions
float SOB_QuatDot(const SOB_Quat a, const SOB_Quat b);

// Squared lenght of a quaternion (l*l), faster than lenght
float SOB_QuatLenSq(const SOB_Quat src);

// Invert a quaternion
void SOB_QuatInvert(const SOB_Quat src, SOB_Quat dest);

// Multiplies two quaternions into dest
void SOB_QuatMul(const SOB_Quat a, const SOB_Quat b, SOB_Quat dest);

// Rotate a vec3 using a quaternion
void SOB_QuatTransformVec3(const SOB_Quat q, const SOB_Vec3 v, SOB_Vec3 dest);

// Create a quaternion looking at a direction
void SOB_QuatLookRotation(const SOB_Vec3 dir, const SOB_Vec3 up, SOB_Quat dest);

// Convert a quaternion into a 4x4 matrix
void SOB_QuatToMat4(const SOB_Quat q, SOB_Mat4 dest);

// Create a prespective matrix
void SOB_Mat4Perspective(float fov, float aspect, float near, float far, SOB_Mat4 dest);

// Resize a perspective matrix using a new aspect ratio
void SOB_Mat4PerspectiveResize(const SOB_Mat4 src, float aspect, SOB_Mat4 dest);

// Invert a matrix
bool SOB_Mat4Invert(const SOB_Mat4 src, SOB_Mat4 dest);

// Multiply two matrices
void SOB_Mat4Mul(const SOB_Mat4 a, const SOB_Mat4 b, SOB_Mat4 dest);

// Convert a 4x4 matrix to a quaternion (basis matrix only)
void SOB_Mat4ToQuat(const SOB_Mat4 m, SOB_Quat dest);

// Transform a Vec4 in the space of mat4 into dest
void SOB_Mat4TransformVec4(const SOB_Mat4 m, const SOB_Vec4 v, SOB_Vec4 dest);

// Initialiaze a transform to identity
void SOB_XFormIdentity(SOB_XForm dest);

// Set the position of a transform to the given vec3
void SOB_XFormTranslate(const SOB_XForm src, const SOB_Vec3 position, SOB_XForm dest);

// Rotate the transform to look at a point in the scene
void SOB_XFormLookAtPoint(const SOB_XForm src, const SOB_Vec3 position, const SOB_Vec3 up, SOB_XForm dest);

// Create a view matrix from an transform
void SOB_XFormToView(const SOB_XForm xform, SOB_Mat4 view);

// Create a model matrix from an transform
void SOB_XFormToModel(const SOB_XForm xform, SOB_Mat4 model);

#define SOB_Rads(x) ((x)*0.01745329f)
#endif /* SOB_XMATH_H */
