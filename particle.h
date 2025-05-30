#ifndef SPS_PARTICLE_H
#define SPS_PARTICLE_H

#include "xmath.h"
#include <SDL3/SDL_gpu.h>

typedef struct {
  SPS_ALIGN_VEC3 SPS_Vec3 position;
  SPS_ALIGN_VEC3 SPS_Vec3 velocity;
  float mass;
} SPS_Particle;

typedef struct {
  SDL_GPUDevice *device;
  SDL_GPUGraphicsPipeline *pipeline;
  SDL_GPUBuffer *vp_buffer;
  SDL_GPUBuffer *pp_buffer;
  SDL_GPUTransferBuffer *upload_transfer_buffer;
  SPS_Particle *particles;
  Uint64 particle_count;
} SPS_ParticleSystem;

bool SPS_ParticleSystemInit(SPS_ParticleSystem *ps, Uint64 count, SDL_GPUDevice *device, SDL_Window *window);
void SPS_ParticleSystemDebug(SPS_ParticleSystem *ps);
bool SPS_ParticleSystemDraw(SPS_ParticleSystem *ps,
        const SPS_Mat4 proj,
        const SPS_Mat4 view,
        SDL_GPURenderPass* render_pass);
void SPS_ParticleSystemUpdate(SPS_ParticleSystem *ps, float dt);
void SPS_ParticleSystemDestroy(SPS_ParticleSystem *ps);

#endif /* SPS_PARTICLE_H */
