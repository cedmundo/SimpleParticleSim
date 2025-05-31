#ifndef SPS_CAMERA_H
#define SPS_CAMERA_H

#include <SDL3/SDL_video.h>
#include "xmath.h"

// An orbiting camera implementation
typedef struct {
  SPS_ALIGN_MAT4 SPS_Mat4 proj;
  SPS_ALIGN_MAT4 SPS_Mat4 view;
  SPS_ALIGN_XFORM SPS_XForm xform;
  SPS_ALIGN_VEC3 SPS_Vec3 orbit_point;

  float azimuth;
  float polar;
  float radius;
  float target_radius;

  float orbit_speed;
  float move_speed;
  float zoom_speed;
  float zoom_step;
  float zoom_in_limit;
  float zoom_out_limit;
} SPS_Camera;

// Load camera using perspective projection and default parameters
void SPS_CameraLoad(SPS_Camera* camera, float aspect);

// Notify the camera that viewport size has changed
void SPS_CameraViewportResize(SPS_Camera* camera, float aspect);

// Update the camera position using the default controls
void SPS_CameraUpdate(SPS_Camera* camera,
                      SDL_Window* window,
                      float relative_mouse_wheel,
                      float dt);

#endif /* SPS_CAMERA_H */
