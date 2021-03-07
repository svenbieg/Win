//==================
// PixelShader.hlsl
//==================

#include "ConstantBuffer.hlsli"
#include "PixelShaderInput.hlsli"


//=========
// Texture
//=========

SamplerState LinearState: register(s0);
Texture2D DiffuseTexture: register(t0);
Texture2D SpecularTexture: register(t1);


//==============
// Pixel-Shader
//==============

float4 main(PSI psi): SV_TARGET
{
return psi.Color*DiffuseTexture.Sample(LinearState, psi.TextureCoord);
}
