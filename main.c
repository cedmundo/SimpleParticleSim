// clang-format off
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_log.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
// clang-format on

#include "camera.h"
#include "grid.h"
#include "shader.h"

#define GAME_CALLBACK __attribute__((unused))
#define WINDOW_TITLE ("SimpleOrbitCamera")
#define WINDOW_WIDTH (1280)
#define WINDOW_HEIGHT (720)

typedef struct {
  SDL_Window* window;
  SDL_GPUDevice* device;
  SDL_GPUViewport viewport;
  struct {
    SOB_Camera camera;
    SOB_Grid grid;
  } scene;
  Uint64 last_tick;
  float delta_time;
  float relative_mouse_wheel;
} SOB_GameState;

bool SOB_GameStateLoad(SOB_GameState* state);
void SOB_GameStateUpdate(SOB_GameState* state);
bool SOB_GameStateRender(SOB_GameState* state);
void SOB_GameStateDestroy(SOB_GameState* state);

GAME_CALLBACK SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  (void)argc;
  (void)argv;

  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  // Allocate game state
  SOB_GameState* state = SDL_malloc(sizeof(SOB_GameState));
  if (state == NULL) {
    SDL_Log("Couldn't allocate memory for game state");
    return SDL_APP_FAILURE;
  }
  SDL_zero(*state);

  // Initialize SDL-specific attributes of game state
  state->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
  if (state->device == NULL) {
    SDL_Log("Couldn't create GPU device: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
  if (state->window == NULL) {
    SDL_Log("Couldn't create window: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowPosition(state->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  if (!SDL_ClaimWindowForGPUDevice(state->device, state->window)) {
    SDL_Log("Couldn't claim window for GPU device: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->viewport = (SDL_GPUViewport){
      .x = 0,
      .y = 0,
      .w = WINDOW_WIDTH,
      .h = WINDOW_HEIGHT,
      .min_depth = 0.0f,
      .max_depth = 1.0f,
  };

  if (!SOB_GameStateLoad(state)) {
    SDL_Log("Couldn't load game state");
    return SDL_APP_FAILURE;
  }

  *appstate = state;
  return SDL_APP_CONTINUE;
}

GAME_CALLBACK SDL_AppResult SDL_AppIterate(void* appstate) {
  SOB_GameState* state = (SOB_GameState*)appstate;
  Uint64 current_tick = SDL_GetPerformanceCounter();
  state->delta_time =
      (float)(current_tick - state->last_tick) / (float)SDL_GetPerformanceFrequency();
  state->last_tick = current_tick;
  {
    SOB_GameStateUpdate(state);

    // TODO(cedmundo): Should we fix the framerate?
    if (!SOB_GameStateRender(state)) {
      return SDL_APP_FAILURE;
    }
  }

  return SDL_APP_CONTINUE;
}

GAME_CALLBACK SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  SOB_GameState* state = (SOB_GameState*)appstate;
  switch (event->type) {
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_RESIZED:
      state->viewport.w = (float)event->window.data1;
      state->viewport.h = (float)event->window.data2;
      SOB_CameraViewportResize(&state->scene.camera, state->viewport.w / state->viewport.h);
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      state->relative_mouse_wheel = -event->wheel.y;
    default:
      break;
  }

  return SDL_APP_CONTINUE;
}

GAME_CALLBACK void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  SOB_GameState* state = (SOB_GameState*)appstate;
  if (result != SDL_APP_SUCCESS) {
    SDL_Log("Application quit with error: %d", result);
  }

  SOB_GameStateDestroy(state);
  if (state->window != NULL) {
    SDL_ReleaseWindowFromGPUDevice(state->device, state->window);
    SDL_DestroyWindow(state->window);
    SDL_DestroyGPUDevice(state->device);
    state->window = NULL;
    state->device = NULL;
  }

  SDL_free(state);
}

bool SOB_GameStateLoad(SOB_GameState* state) {
  SOB_CameraLoad(&state->scene.camera, state->viewport.w / state->viewport.h);
  if (!SOB_GridLoad(&state->scene.grid, state->device, state->window)) {
    return false;
  }

  return true;
}

void SOB_GameStateUpdate(SOB_GameState* state) {
  SOB_CameraUpdate(&state->scene.camera, state->window, state->relative_mouse_wheel,
                   state->delta_time);
  state->relative_mouse_wheel = 0.0f;
}

bool SOB_GameStateRender(SOB_GameState* state) {
  SDL_GPUCommandBuffer* cmd_buf = SDL_AcquireGPUCommandBuffer(state->device);
  if (cmd_buf == NULL) {
    SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
    return false;
  }

  // Get window swap chain texture
  SDL_GPUTexture* swap_chain_texture = NULL;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, state->window, &swap_chain_texture, NULL,
                                             NULL)) {
    SDL_Log("Couldn't acquire swap chain texture: %s", SDL_GetError());
  }

  // Render when we have a texture
  if (swap_chain_texture != NULL) {
    SDL_GPUColorTargetInfo color_target_info = {
        .texture = swap_chain_texture,
        .clear_color = (SDL_FColor){0.2f, 0.2f, 0.2f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);
    {
      SDL_SetGPUViewport(render_pass, &state->viewport);

      // Get the camera where we are going to be drawing everything
      SOB_Camera* camera = &state->scene.camera;

      // Draw the grid
      SOB_GridDraw(&state->scene.grid, camera->proj, camera->view, render_pass);
    }
    SDL_EndGPURenderPass(render_pass);
  }

  SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd_buf);
  SDL_WaitForGPUFences(state->device, true, &fence, 1);
  SDL_ReleaseGPUFence(state->device, fence);
  return true;
}

void SOB_GameStateDestroy(SOB_GameState* state) {
  SOB_GridUnload(&state->scene.grid);
}
