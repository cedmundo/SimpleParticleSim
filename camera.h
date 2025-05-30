#ifndef SOB_CAMERA_H
#define SOB_CAMERA_H

#include "xmath.h"
#include <SDL3/SDL_video.h>

typedef struct {
  SOB_ALIGN_MAT4 SOB_Mat4 proj;
  SOB_ALIGN_MAT4 SOB_Mat4 view;
  SOB_ALIGN_XFORM SOB_XForm xform;
  SOB_ALIGN_VEC3 SOB_Vec3 orbit_point;

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
} SOB_Camera;

// Load camera using perspective projection and default parameters
void SOB_CameraLoad(SOB_Camera *camera, float aspect);

// Notify the camera that viewport size has changed
void SOB_CameraViewportResize(SOB_Camera *camera, float aspect);

// Update the camera position using the default controls
void SOB_CameraUpdate(SOB_Camera *camera, SDL_Window *window, float relative_mouse_wheel, float dt);

#endif /* SOB_CAMERA_H */
