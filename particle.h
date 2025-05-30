#ifndef SPS_PARTICLE_H
#define SPS_PARTICLE_H

#include "xmath.h"

typedef struct {
  SPS_ALIGN_VEC3 SPS_Vec3 position;
  SPS_ALIGN_VEC3 SPS_Vec3 velocity;
  float mass;
} SPS_Particle;

void SPS_ParticleArrayInit(SPS_Particle *particles, Uint64 count);
void SPS_ParticleArrayDebug(const SPS_Particle *particles, Uint64 count);
void SPS_ParticleArraySimulate(SPS_Particle *particles, Uint64 count, float dt);
void SPS_ParticleComputeForce(const SPS_Particle *particle, SPS_Vec3 dest);

#endif /* SPS_PARTICLE_H */
