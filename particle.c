#include "particle.h"
#include "xmath.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

void SPS_ParticleArrayInit(SPS_Particle* particles, Uint64 count) {
  for (Uint64 i = 0; i < count; i++) {
    SPS_Vec3Make(SDL_randf() * 10.0f, SDL_randf() * 10.0f, SDL_randf() * 10.0f,
                 particles[i].position);
    SPS_Vec3Make(0.0f, 0.0f, 0.0f, particles[i].velocity);
    particles[i].mass = 1.0f;
  }
}

void SPS_ParticleArrayDebug(const SPS_Particle* particles, Uint64 count) {
  for (Uint64 i = 0; i < count; i++) {
    SPS_Particle p = particles[i];
    SDL_Log("particle[%ld] = (%+.2f, %+.2f, %+.2f) mass = %+.2f\n", i,
            p.position[0], p.position[1], p.position[2], p.mass);
  }
}

void SPS_ParticleArraySimulate(SPS_Particle* particles,
                               Uint64 count,
                               float dt) {
  SPS_ALIGN_VEC3 SPS_Vec3 force = {0};
  SPS_ALIGN_VEC3 SPS_Vec3 acceleration = {0};
  SPS_ALIGN_VEC3 SPS_Vec3 aux = {0};

  for (Uint64 i = 0; i < count; i++) {
    SPS_Particle* p = &particles[i];
    SPS_ParticleComputeForce(p, force);
    SPS_Vec3Scale(force, 1.0f / p->mass, acceleration);

    // p.velocity = p.velocity + acceleration * dt
    SPS_Vec3Scale(acceleration, dt, aux);
    SPS_Vec3Add(p->velocity, aux, p->velocity);

    // p.position = p.position + p.velocity * dt
    SPS_Vec3Scale(p->velocity, dt, aux);
    SPS_Vec3Add(p->position, aux, p->position);
  }
}

void SPS_ParticleComputeForce(const SPS_Particle* particle, SPS_Vec3 dest) {
  // F = ma (gravity)
  dest[0] = 0.0f;
  dest[1] = particle->mass * -9.81;
  dest[2] = 0.0f;
}
