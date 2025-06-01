#ifndef SPS_PARTICLE_SYSTEM_H
#define SPS_PARTICLE_SYSTEM_H

#include <SDL3/SDL_gpu.h>
#include "xmath.h"

// Single simulated particle.
typedef struct {
  SPS_ALIGN_VEC4 SPS_Vec3 position;
  float scale;
  SPS_ALIGN_VEC4 SPS_Vec3 velocity;
  float mass;
} SPS_Particle;

// Particle simulation, it can also render the partciles.
typedef struct {
  SDL_GPUDevice* device;
  SDL_GPUGraphicsPipeline* pipeline;
  SDL_GPUBuffer* buffer;
  SDL_GPUTransferBuffer* upload_transfer_buffer;
  SPS_Particle* instances;
  Uint64 instances_count;
} SPS_ParticleSystem;

// Initializes the particle system with a fixed count of partciles.
bool SPS_ParticleSystemLoad(SPS_ParticleSystem* ps,
                            Uint64 count,
                            SDL_GPUDevice* device,
                            SDL_Window* window);

// Prints to logs the particle positions and mass.
void SPS_ParticleSystemDebug(SPS_ParticleSystem* ps);

// Draws the partcile system entirely.
bool SPS_ParticleSystemDraw(SPS_ParticleSystem* ps,
                            const SPS_Mat4 proj,
                            const SPS_Mat4 view,
                            const SPS_Vec3 view_pos,
                            SDL_GPUCommandBuffer* cmd_buf,
                            SDL_GPURenderPass* render_pass);

// Updates the particle system simulation.
void SPS_ParticleSystemUpdate(SPS_ParticleSystem* ps, float dt);

// Releases the resources used by the particle system simulation.
void SPS_ParticleSystemDestroy(SPS_ParticleSystem* ps);

#endif /* SPS_PARTICLE_SYSTEM_H */
