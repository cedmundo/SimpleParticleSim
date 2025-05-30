#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include "particle.h"
#include "simulation.h"

bool SPS_SimulationLoad(SPS_Simulation* state) {
  SPS_CameraLoad(&state->camera, state->viewport.w / state->viewport.h);
  if (!SPS_GridLoad(&state->grid, state->device, state->window)) {
    return false;
  }

  if (!SPS_ParticleSystemInit(&state->particle_system, MAX_PARTICLES,
                              state->device, state->window)) {
    SDL_Log("Could not initialize particle system for %d particles!",
            MAX_PARTICLES);
    return false;
  }

  return true;
}

void SPS_SimulationEvent(SPS_Simulation* state, SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_WINDOW_RESIZED:
      state->viewport.w = (float)event->window.data1;
      state->viewport.h = (float)event->window.data2;
      SPS_CameraViewportResize(&state->camera,
                               state->viewport.w / state->viewport.h);
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      state->relative_mouse_wheel = -event->wheel.y;
    default:
      break;
  }
}

void SPS_SimulationUpdate(SPS_Simulation* state, float dt) {
  {
    SPS_CameraUpdate(&state->camera, state->window, state->relative_mouse_wheel,
                     dt);

    SPS_ParticleSystemUpdate(&state->particle_system, dt);
    // SPS_ParticleSystemDebug(&state->particle_system);
  }
  state->relative_mouse_wheel = 0.0f;
}

bool SPS_SimulationRender(SPS_Simulation* state, float dt) {
  SDL_GPUCommandBuffer* cmd_buf = SDL_AcquireGPUCommandBuffer(state->device);
  if (cmd_buf == NULL) {
    SDL_Log("Could not acquire GPU command buffer: %s", SDL_GetError());
    return false;
  }

  // Get window swap chain texture
  SDL_GPUTexture* swapchain_texture = NULL;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, state->window,
                                             &swapchain_texture, NULL, NULL)) {
    SDL_Log("Could not acquire swap chain texture: %s", SDL_GetError());
  }

  // Render when we have a texture
  if (swapchain_texture != NULL) {
    SDL_GPUColorTargetInfo color_target_info = {
        .texture = swapchain_texture,
        .clear_color = (SDL_FColor){0.2f, 0.2f, 0.2f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    SDL_GPURenderPass* render_pass =
        SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);
    {
      SDL_SetGPUViewport(render_pass, &state->viewport);

      // Get the camera where we are going to be drawing everything
      SPS_Camera* camera = &state->camera;

      // Draw the grid
      SPS_GridDraw(&state->grid, camera->proj, camera->view, render_pass);

      // Draw the particles
      SPS_ParticleSystemDraw(&state->particle_system, camera->proj,
                             camera->view, render_pass);
    }
    SDL_EndGPURenderPass(render_pass);
  }

  SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd_buf);
  SDL_WaitForGPUFences(state->device, true, &fence, 1);
  SDL_ReleaseGPUFence(state->device, fence);
  return true;
}

void SPS_SimulationDestroy(SPS_Simulation* state) {
  SPS_GridUnload(&state->grid);
  SPS_ParticleSystemDestroy(&state->particle_system);
}
