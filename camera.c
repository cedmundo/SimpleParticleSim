#include "camera.h"
#include "xmath.h"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>

void SOB_CameraLoad(SOB_Camera* camera, float aspect) {
  SOB_Mat4Perspective(SOB_Rads(45.0f), aspect, 0.01f, 100.0f, camera->proj);
  SOB_XFormIdentity(camera->xform);

  // Temp: Move to 5,5,5 and look at the center
  SOB_XFormTranslate(camera->xform, (SOB_Vec3){5.0f, 5.0f, 5.0f}, camera->xform);
  SOB_XFormLookAtPoint(camera->xform, (SOB_Vec3){0.0f, 0.0f, 0.0f}, (SOB_Vec3){0.0f, 1.0f, 0.0f},
                       camera->xform);
  camera->radius = 10.0f;
  camera->target_radius = 10.0f;
  camera->azimuth = 45.0f;
  camera->polar = 45.0f;
  camera->move_speed = 10.0f;
  camera->orbit_speed = 720.0f;
  camera->zoom_speed = 5.0f;
  camera->zoom_step = 2.0f;
  camera->zoom_in_limit = 0.1f;
  camera->zoom_out_limit = 30.0f;
}

void SOB_CameraViewportResize(SOB_Camera* camera, float aspect) {
  SOB_Mat4PerspectiveResize(camera->proj, aspect, camera->proj);
}

void SOB_CameraUpdate(SOB_Camera* camera,
                      SDL_Window* window,
                      float relative_mouse_wheel,
                      float dt) {
  // Update camera orbiting position using keyboard
  const bool* keyboard_state = SDL_GetKeyboardState(NULL);
  SOB_ALIGN_VEC3 SOB_Vec3 world_up = {0.0, 1.0f, 0.0f};
  SOB_ALIGN_VEC3 SOB_Vec3 input_forward = {0.0f, 0.0f, 0.0f};
  SOB_ALIGN_VEC3 SOB_Vec3 input_left = {0.0f, 0.0f, 0.0f};
  SOB_ALIGN_VEC3 SOB_Vec3 cam_forward = {0};
  SOB_ALIGN_VEC3 SOB_Vec3 cam_left = {0};
  SOB_ALIGN_VEC3 SOB_Vec3 move_dir = {0};
  SOB_ALIGN_VEC3 SOB_Vec3 orbit_vec = {0};
  SOB_ALIGN_QUAT SOB_Quat yaw_rot = {0};
  SOB_Vec2 mouse_coords = {0};
  float a = SOB_Rads(camera->azimuth);
  float p = SOB_Rads(camera->polar);

  if (keyboard_state[SDL_SCANCODE_W]) {
    input_forward[2] = -1.0f;
  } else if (keyboard_state[SDL_SCANCODE_S]) {
    input_forward[2] = 1.0f;
  }

  if (keyboard_state[SDL_SCANCODE_A]) {
    input_left[0] = -1.0f;
  } else if (keyboard_state[SDL_SCANCODE_D]) {
    input_left[0] = 1.0f;
  }

  float move_speed_zoom_k = SDL_log(camera->radius * camera->radius + 1.5f);
  SOB_QuatMakeAxisAngle(world_up, SDL_PI_F * 0.5f - a, yaw_rot);
  SOB_QuatTransformVec3(yaw_rot, input_forward, cam_forward);
  SOB_QuatTransformVec3(yaw_rot, input_left, cam_left);
  SOB_Vec3Add(cam_left, cam_forward, move_dir);
  SOB_Vec3Normalize(move_dir, move_dir);
  SOB_Vec3Scale(move_dir, move_speed_zoom_k * dt, move_dir);
  SOB_Vec3Add(move_dir, camera->orbit_point, camera->orbit_point);

  // Orbit the camera around the orbit point
  const SDL_MouseButtonFlags mouse_state =
      SDL_GetRelativeMouseState(&mouse_coords[0], &mouse_coords[1]);
  if (SDL_BUTTON_MMASK & mouse_state) {
    SDL_CaptureMouse(true);
    SDL_SetWindowRelativeMouseMode(window, true);
    camera->azimuth += SOB_Rads(mouse_coords[0] * camera->orbit_speed) * dt;
    camera->polar = SDL_clamp(camera->polar + SOB_Rads(mouse_coords[1] * camera->orbit_speed) * dt,
                              -90.0f, 90.0f);
  } else {
    SDL_CaptureMouse(false);
    SDL_SetWindowRelativeMouseMode(window, false);
  }

  orbit_vec[0] = camera->orbit_point[0] + camera->radius * SDL_cos(p) * SDL_cos(a);
  orbit_vec[1] = camera->orbit_point[1] + camera->radius * SDL_sin(p);
  orbit_vec[2] = camera->orbit_point[2] + camera->radius * SDL_cos(p) * SDL_sin(a);

  SOB_XFormTranslate(camera->xform, orbit_vec, camera->xform);
  SOB_XFormLookAtPoint(camera->xform, camera->orbit_point, world_up, camera->xform);

  // Interpolate the zoom to smooth transition
  if ((camera->radius > camera->zoom_in_limit && relative_mouse_wheel < 0.0f) ||
      (camera->radius < camera->zoom_out_limit && relative_mouse_wheel > 0.0f)) {
    float step = camera->zoom_step * SDL_log(camera->radius * 0.25f + 1);
    camera->target_radius = SDL_clamp(camera->radius + step * relative_mouse_wheel,
                                      camera->zoom_in_limit, camera->zoom_out_limit);
  }

  if (SDL_fabsf(camera->target_radius - camera->radius) > SDL_FLT_EPSILON) {
    camera->radius *= SDL_powf(camera->target_radius / camera->radius, dt * camera->zoom_speed);
  }

  // Apply transform and get view matrix
  SOB_XFormToView(camera->xform, camera->view);
}
