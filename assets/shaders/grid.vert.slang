struct ViewParams {
  float4x4 pv;
  float4x4 pvInv;
};

struct VSInput {
  uint vertexID : SV_VertexID;
};

struct VSOutput {
  float4x4 pv;
  float3 nearPoint;
  float3 farPoint;
  float4 position : SV_Position;
};

static const float2[] quadXYVertices = {
  float2(-1.0f, +1.0f),  // bottom left
  float2(-1.0f, -1.0f),  // top left
  float2(+1.0f, -1.0f),  // top right
  float2(+1.0f, -1.0f),  // top right
  float2(+1.0f, +1.0f),  // bottom right
  float2(-1.0f, +1.0f),  // bottom left
};

float3 unprojectedPoint(float x, float y, float z, float4x4 pvInv) {
  float4 unprojected = mul(pvInv, float4(x, y, z, 1.0f));
  return unprojected.xyz / unprojected.w;
}

layout(set = 1, binding = 0) ConstantBuffer<ViewParams> viewParams;

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
  VSOutput output;
  float2 vertexPos = quadXYVertices[input.vertexID];
  output.pv = viewParams.pv;
  output.nearPoint =
      unprojectedPoint(vertexPos.x, vertexPos.y, 0.01f, viewParams.pvInv);
  output.farPoint =
      unprojectedPoint(vertexPos.x, vertexPos.y, 1.0f, viewParams.pvInv);
  output.position = float4(vertexPos.x, vertexPos.y, 0.0f, 1.0f);
  return output;
}
