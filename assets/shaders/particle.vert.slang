
struct ViewParams {
  float4x4 pv;
};

struct Particle {
  float3 position;
};

struct VSInput {
  uint vertexId : SV_VertexID;
  uint instanceId : SV_InstanceID;
};

struct VSOutput {
  float4 position : SV_Position;
};

StructuredBuffer<ViewParams> viewParams;
StructuredBuffer<Particle> particles;

static const float2[] quad_vertices = {
  float2(-1.0f, +1.0f),  // bottom left
  float2(-1.0f, -1.0f),  // top left
  float2(+1.0f, -1.0f),  // top right
  float2(+1.0f, -1.0f),  // top right
  float2(+1.0f, +1.0f),  // bottom right
  float2(-1.0f, +1.0f),  // bottom left
};

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
  VSOutput output;
  float2 curVertex = quad_vertices[input.vertexId];
  float3 particlePos = particles[input.instanceId].position;
  float4x4 model = {
    { 0.1, 0, 0, particlePos.x },  // mx
    { 0, 0.1, 0, particlePos.y },  // my
    { 0, 0, 0.1, particlePos.z },  // mz
    { 0, 0, 0, 1 },                // mw
  };

  float4 vertexPos = float4(curVertex.x, curVertex.y, 0.0f, 1.0f);
  output.position = mul(mul(viewParams[0].pv, model), vertexPos);
  return output;
}
