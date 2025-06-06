#ifndef SPS_SIMULATION_H
#define SPS_SIMULATION_H
// clang-format off
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_gpu.h>
// clang-format on

#include "camera.h"
#include "grid.h"
#include "particle_system.h"
#include "shader.h"

#define MAX_PARTICLES (10000)

// Global values for the simulation
typedef struct {
  SDL_Window* window;
  SDL_GPUDevice* device;
  SDL_GPUViewport viewport;
  SPS_ParticleSystem particle_system;
  SPS_Camera camera;
  SPS_Grid grid;
  Uint64 last_tick;
  float iter_delta_time;
  float cur_frame_time;
  float cur_update_time;
  float relative_mouse_wheel;
} SPS_Simulation;

// Load the simulation.
bool SPS_SimulationLoad(SPS_Simulation* state);

// Let simulation handle an event from SDL.
void SPS_SimulationEvent(SPS_Simulation* state, SDL_Event* event);

// Update the simulation (fixed rate).
void SPS_SimulationUpdate(SPS_Simulation* state, float dt);

// Render the simulation (fixed rate).
bool SPS_SimulationRender(SPS_Simulation* state, float dt);

// Release the resources creates by the simulation.
void SPS_SimulationDestroy(SPS_Simulation* state);

#endif /* SPS_SIMULATION_H */
