#ifndef SOB_SHADER_H
#define SOB_SHADER_H

#include <SDL3/SDL_gpu.h>

typedef struct {
  const char *filename;
  Uint32 sampler_count;
  Uint32 uniform_buffer_count;
  Uint32 storage_buffer_count;
  Uint32 storage_texture_count;
  SDL_GPUShaderStage stage;
} SOB_ShaderOptions;

// Load a shader from a SPV file
SDL_GPUShader *SOB_ShaderLoad(
      SDL_GPUDevice *device,
      SOB_ShaderOptions options);

#endif /* SOB_SHADER_H */
