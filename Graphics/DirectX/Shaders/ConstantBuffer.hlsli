//======================
// ConstantBuffer.hlsli
//======================

cbuffer ConstantBuffer
{
matrix ModelMatrix;
matrix ViewMatrix;
matrix ProjectionMatrix;
float4 AmbientColor;
float4 Color;
};
