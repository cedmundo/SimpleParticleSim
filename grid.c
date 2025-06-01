#include "grid.h"
#include "shader.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>

typedef struct {
  SPS_ALIGN_MAT4 SPS_Mat4 pv;
  SPS_ALIGN_MAT4 SPS_Mat4 pv_inv;
} GridUniforms;

bool SPS_GridLoad(SPS_Grid* grid, SDL_GPUDevice* device, SDL_Window* window) {
  grid->device = device;
  SPS_ShaderOptions vert_options = (SPS_ShaderOptions){
      .filename = "grid.vert",
      .stage = SDL_GPU_SHADERSTAGE_VERTEX,
      .sampler_count = 0,
      .uniform_buffer_count = 1,
      .storage_buffer_count = 0,
      .storage_texture_count = 0,
  };
  SDL_GPUShader* vert_shader = SPS_ShaderLoad(device, vert_options);
  if (vert_shader == NULL) {
    return false;
  }

  SPS_ShaderOptions frag_options = (SPS_ShaderOptions){
      .filename = "grid.frag",
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
  grid->pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);

  SDL_ReleaseGPUShader(device, vert_shader);
  SDL_ReleaseGPUShader(device, frag_shader);

  if (grid->pipeline == NULL) {
    SDL_Log("Couldn't create graphics pipeline for debug grid");
    return false;
  }

  return true;
}

void SPS_GridDraw(SPS_Grid* grid,
                  const SPS_Mat4 proj,
                  const SPS_Mat4 view,
                  SDL_GPUCommandBuffer* cmd_buf,
                  SDL_GPURenderPass* render_pass) {
  GridUniforms uniforms = {0};
  SPS_Mat4Mul(proj, view, uniforms.pv);
  SPS_Mat4Invert(uniforms.pv, uniforms.pv_inv);

  SDL_BindGPUGraphicsPipeline(render_pass, grid->pipeline);
  SDL_PushGPUVertexUniformData(cmd_buf, 0, &uniforms, sizeof(GridUniforms));
  SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
}

void SPS_GridDestroy(SPS_Grid* grid) {
  SDL_ReleaseGPUGraphicsPipeline(grid->device, grid->pipeline);
}
