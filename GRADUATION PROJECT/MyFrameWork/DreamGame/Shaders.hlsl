
struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular; 
    float4 m_cEmissive;
};

cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxGameObject : packoffset(c0);
    MATERIAL gMaterial : packoffset(c4);
    uint gnTexturesMask : packoffset(c8);
};

//cbuffer cbCameraInfo : register(b1)
//{
//    matrix gmtxView : packoffset(c0);
//    matrix gmtxProjection : packoffset(c4);
//    matrix gmtxLightView : packoffset(c8);
//    matrix gmtxLightProjection : packoffset(c12);
//    matrix gmtxShadowTransform : packoffset(c16);
//    float3 gvCameraPosition : packoffset(c20);
//    float gfPad1 : packoffset(c20.w);
//    float3 gvLightPosition : packoffset(c21.x);
//};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    matrix gmtxInverseView : packoffset(c8);
    float3 gvCameraPosition : packoffset(c12);
   
};

Texture2D shaderTexture : register(t0);
Texture2D ShadowMap : register(t1);

SamplerState gWrapSamplerState : register(s0);
SamplerComparisonState gClampSamplerState : register(s1);

//float CalcShadowFactor(float4 shadowPosH)
//{
//	// Complete projection by doing division by w.
//    shadowPosH.xyz /= shadowPosH.w;

//	// Depth in NDC space.
//    float depth = shadowPosH.z;

//    uint width, height, numMips;
//    ShadowMap.GetDimensions(0, width, height, numMips);

//	// Texel size.
//    float dx = 1.0f / (float) width;

//    float percentLit = 0.0f;
//    const float2 offsets[9] =
//    {
//        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
//		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
//		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
//    };

//    for (int i = 0; i < 9; ++i)
//    {
//        percentLit += ShadowMap.SampleCmpLevelZero(gClampSamplerState, shadowPosH.xy, depth).r;
//    }

//    return percentLit / 9.0f;
//}


struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    //float3 normal : NORMAL;
};
// #define _WITH_VERTEX_LIGHTING
struct VS_OUTPUT
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
   
};
#include"Light.hlsl"
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    float3 normal = { 1.f,1.f,1.f };
    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.positionH = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(normal, (float3x3) gmtxGameObject);
    output.uv = input.uv;
    output.normalW = normalW;
#ifdef _WITH_VERTEX_LIGHTING
	output.color = Lighting(output.positionW, normalize(normalW));
	output.color = float4(0.5f * normalize(gvCameraPosition - output.positionW) + 0.5f, 1.0f);
#else
#endif

    return (output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	return(float4(input.positionW, 1.0f));
//    return (output.color)
#else
    float3 normalW = normalize(input.normalW);
    float4 cIllumination = Lighting(input.positionW, normalW);
    return cIllumination;
    // return (cIllumination * gcAlbedoColor + gcEmissionColor);
#endif

}