#ifndef SPS_GRID_H
#define SPS_GRID_H

#include <SDL3/SDL_gpu.h>
#include "xmath.h"

// Debug grid in XZ plane.
typedef struct {
  SDL_GPUDevice* device;
  SDL_GPUGraphicsPipeline* pipeline;
} SPS_Grid;

// Load the debug grid shaders and resources
bool SPS_GridLoad(SPS_Grid* grid, SDL_GPUDevice* device, SDL_Window* window);

// Draw the debug grid on scene
void SPS_GridDraw(SPS_Grid* grid,
                  const SPS_Mat4 proj,
                  const SPS_Mat4 view,
                  SDL_GPUCommandBuffer* cmd_buf,
                  SDL_GPURenderPass* render_pass);

// Unload the debug grid resources
void SPS_GridDestroy(SPS_Grid* grid);

#endif /* SPS_GRID_H */
