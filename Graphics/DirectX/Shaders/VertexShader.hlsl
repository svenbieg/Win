//===================
// VertexShader.hlsl
//===================

#include "ConstantBuffer.hlsli"
#include "PixelShaderInput.hlsli"


//=====================
// Vertex-Shader-Input
//=====================

struct VSI
{
float3 Position: POSITION;
float4 Color: Color;
float2 TextureCoord: TEXCOORD;
};


//===============
// Vertex-Shader
//===============

PSI main(VSI vsi)
{
float4 vpos=float4(vsi.Position, 1.f);
vpos=mul(vpos, ModelMatrix);
vpos=mul(vpos, ViewMatrix);
vpos=mul(vpos, ProjectionMatrix);
PSI psi;
psi.Position=vpos;
psi.Color=vsi.Color;
psi.TextureCoord=vsi.TextureCoord;
return psi;
}
