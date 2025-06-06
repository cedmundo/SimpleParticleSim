#ifndef SPS_SHADER_H
#define SPS_SHADER_H

#include <SDL3/SDL_gpu.h>

// General shader options such name and object count.
typedef struct {
  const char* filename;
  Uint32 sampler_count;
  Uint32 uniform_buffer_count;
  Uint32 storage_buffer_count;
  Uint32 storage_texture_count;
  SDL_GPUShaderStage stage;
} SPS_ShaderOptions;

// Load a shader from a SPV file.
SDL_GPUShader* SPS_ShaderLoad(SDL_GPUDevice* device, SPS_ShaderOptions options);

#endif /* SPS_SHADER_H */
