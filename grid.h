#ifndef SOB_GRID_H
#define SOB_GRID_H

#include <SDL3/SDL_gpu.h>
#include "xmath.h"

typedef struct {
  SDL_GPUDevice *device;
  SDL_GPUGraphicsPipeline *pipeline;
  SDL_GPUBuffer *buffer;
  SDL_GPUTransferBuffer *upload_transfer_buffer;
} SOB_Grid;

// Load the debug grid shaders and resources
bool SOB_GridLoad(SOB_Grid *grid, SDL_GPUDevice *device, SDL_Window *window);

// Draw the debug grid on scene
void SOB_GridDraw(SOB_Grid *grid, const SOB_Mat4 proj, const SOB_Mat4 view, SDL_GPURenderPass *render_pass);

// Unload the debug grid resources
void SOB_GridUnload(SOB_Grid *grid);

#endif /* SOB_GRID_H */
