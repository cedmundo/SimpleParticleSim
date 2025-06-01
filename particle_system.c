#include "particle_system.h"
#include "shader.h"
#include "xmath.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

typedef struct {
  SPS_ALIGN_MAT4 SPS_Mat4 pv;
  SPS_ALIGN_VEC3 SPS_Vec3 view_pos;
} ParticleSystemUniforms;

void particle_compute_force(const SPS_Particle* particle, SPS_Vec3 dest);
float remap_value(float value,
                  float start1,
                  float stop1,
                  float start2,
                  float stop2);

bool SPS_ParticleSystemLoad(SPS_ParticleSystem* ps,
                            Uint64 count,
                            SDL_GPUDevice* device,
                            SDL_Window* window) {
  size_t instances_buffer_size = sizeof(SPS_Particle) * count;
  ps->device = device;
  ps->instances_count = count;
  ps->instances = SDL_aligned_alloc(16, instances_buffer_size);
  if (ps->instances == NULL) {
    return false;
  }

  // Initialize particle positions to random places
  SDL_memset(ps->instances, 0, instances_buffer_size);
  for (Uint64 i = 0; i < count; i++) {
    float rx = remap_value(SDL_randf(), 0.0f, 1.0f, -10.0f, 10.0f);
    float ry = remap_value(SDL_randf(), 0.0f, 1.0f, 0.0f, 40.0f);
    float rz = remap_value(SDL_randf(), 0.0f, 1.0f, -10.0f, 10.0f);
    ps->instances[i].position[0] = rx;
    ps->instances[i].position[1] = ry;
    ps->instances[i].position[2] = rz;
    ps->instances[i].scale = 0.1f;
    ps->instances[i].velocity[0] = 0.0f;
    ps->instances[i].velocity[0] = 0.0f;
    ps->instances[i].velocity[0] = 0.0f;
    ps->instances[i].mass = 1.0f;
  }

  SPS_ShaderOptions vert_options = (SPS_ShaderOptions){
      .filename = "particle_system.vert",
      .stage = SDL_GPU_SHADERSTAGE_VERTEX,
      .sampler_count = 0,
      .uniform_buffer_count = 1,
      .storage_buffer_count = 1,
      .storage_texture_count = 0,
  };
  SDL_GPUShader* vert_shader = SPS_ShaderLoad(device, vert_options);
  if (vert_shader == NULL) {
    return false;
  }

  SPS_ShaderOptions frag_options = (SPS_ShaderOptions){
      .filename = "particle_system.frag",
      .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
      .sampler_count = 0,
      .uniform_buffer_count = 0,
      .storage_buffer_count = 0,
      .storage_texture_count = 0,
  };
  SDL_GPUShader* frag_shader = SPS_ShaderLoad(device, frag_options);
  if (frag_shader == NULL) {
    return false;
  }

  SDL_GPUGraphicsPipelineTargetInfo color_target_info = {
      .num_color_targets = 1,
      .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
          .format = SDL_GetGPUSwapchainTextureFormat(device, window),
          .blend_state =
              (SDL_GPUColorTargetBlendState){
                  .enable_blend = true,
                  .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                  .dst_color_blendfactor =
                      SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                  .color_blend_op = SDL_GPU_BLENDOP_ADD,
                  .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                  .dst_alpha_blendfactor =
                      SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                  .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
              },
      }},
  };

  SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
      .target_info = color_target_info,
      .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .vertex_shader = vert_shader,
      .fragment_shader = frag_shader,
  };
  ps->pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);

  SDL_ReleaseGPUShader(device, vert_shader);
  SDL_ReleaseGPUShader(device, frag_shader);

  if (ps->pipeline == NULL) {
    SDL_Log("Couldn't create graphics pipeline for billboard");
    return false;
  }

  // Create buffer location for transform
  SDL_GPUBufferCreateInfo buffer_create_info = {
      .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
      .size = instances_buffer_size,
  };
  ps->buffer = SDL_CreateGPUBuffer(device, &buffer_create_info);
  if (ps->buffer == NULL) {
    SDL_Log("Couldn't create buffer to store the params of debug grid");
    return false;
  }

  // Create transfer buffer handle
  SDL_GPUTransferBufferCreateInfo upload_transfer_buffer_create_info = {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = instances_buffer_size,
  };
  ps->upload_transfer_buffer =
      SDL_CreateGPUTransferBuffer(device, &upload_transfer_buffer_create_info);
  if (ps->upload_transfer_buffer == NULL) {
    SDL_Log("Couldn't create transfer buffer of debug grid");
    return false;
  }

  return true;
}

void SPS_ParticleSystemDebug(SPS_ParticleSystem* ps) {
  for (Uint64 i = 0; i < ps->instances_count; i++) {
    SPS_Particle p = ps->instances[i];
    SDL_Log("particle[%ld] = (%+.2f, %+.2f, %+.2f) mass = %+.2f\n", i,
            p.position[0], p.position[1], p.position[2], p.mass);
  }
}

bool SPS_ParticleSystemDraw(SPS_ParticleSystem* ps,
                            const SPS_Mat4 proj,
                            const SPS_Mat4 view,
                            const SPS_Vec3 view_pos,
                            SDL_GPUCommandBuffer* cmd_buf,
                            SDL_GPURenderPass* render_pass) {
  ParticleSystemUniforms uniforms = {0};
  SPS_Mat4Mul(proj, view, uniforms.pv);
  SPS_Vec3Copy(view_pos, uniforms.view_pos);

  SDL_BindGPUGraphicsPipeline(render_pass, ps->pipeline);
  {
    // Copy data to the staging of the GPU
    void* transfer_point =
        SDL_MapGPUTransferBuffer(ps->device, ps->upload_transfer_buffer, 0);
    SDL_memcpy(transfer_point, ps->instances,
               sizeof(SPS_Vec4) * ps->instances_count);
    SDL_UnmapGPUTransferBuffer(ps->device, ps->upload_transfer_buffer);

    // Create a copy pass
    SDL_GPUCommandBuffer* upload_cmd_buf =
        SDL_AcquireGPUCommandBuffer(ps->device);
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buf);
    {
      SDL_GPUTransferBufferLocation source = {
          .transfer_buffer = ps->upload_transfer_buffer,
          .offset = 0,
      };
      SDL_GPUBufferRegion destination = {
          .buffer = ps->buffer,
          .offset = 0,
          .size = sizeof(SPS_Vec4) * ps->instances_count,
      };

      SDL_UploadToGPUBuffer(copy_pass, &source, &destination, false);
      SDL_EndGPUCopyPass(copy_pass);
      SDL_SubmitGPUCommandBuffer(upload_cmd_buf);
    }
  }

  SDL_PushGPUVertexUniformData(cmd_buf, 0, &uniforms,
                               sizeof(ParticleSystemUniforms));
  SDL_BindGPUVertexStorageBuffers(render_pass, 0, &ps->buffer, 1);
  SDL_DrawGPUPrimitives(render_pass, 6, ps->instances_count, 0, 0);

  return true;
}

void SPS_ParticleSystemUpdate(SPS_ParticleSystem* ps, float dt) {
  SPS_ALIGN_VEC3 SPS_Vec3 force = {0};
  SPS_ALIGN_VEC3 SPS_Vec3 acceleration = {0};
  SPS_ALIGN_VEC3 SPS_Vec3 aux = {0};

  for (Uint64 i = 0; i < ps->instances_count; i++) {
    SPS_Particle* p = &ps->instances[i];
    particle_compute_force(p, force);
    SPS_Vec3Scale(force, 1.0f / SDL_max(p->mass, 0.00001f), acceleration);

    // p.velocity = p.velocity + acceleration * dt
    SPS_Vec3Scale(acceleration, dt, aux);
    SPS_Vec3Add(p->velocity, aux, p->velocity);

    // p.position = p.position + p.velocity * dt
    SPS_Vec3Scale(p->velocity, dt, aux);
    SPS_Vec3Add(p->position, aux, p->position);
  }
}

void SPS_ParticleSystemDestroy(SPS_ParticleSystem* ps) {
  SDL_ReleaseGPUGraphicsPipeline(ps->device, ps->pipeline);
  SDL_ReleaseGPUTransferBuffer(ps->device, ps->upload_transfer_buffer);
  SDL_ReleaseGPUBuffer(ps->device, ps->buffer);

  if (ps->instances != NULL) {
    SDL_aligned_free(ps->instances);
    ps->instances = NULL;
    ps->instances_count = 0;
  }
}

void particle_compute_force(const SPS_Particle* particle, SPS_Vec3 dest) {
  // F = ma (gravity)
  dest[0] = 0.0f;
  dest[1] = particle->mass * -9.81;
  dest[2] = 0.0f;
}

float remap_value(float value,
                  float start1,
                  float stop1,
                  float start2,
                  float stop2) {
  return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}
