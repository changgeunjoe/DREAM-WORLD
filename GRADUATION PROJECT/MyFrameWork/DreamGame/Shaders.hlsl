cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    matrix gmtxLightView : packoffset(c8);
    matrix gmtxLightProjection : packoffset(c12);
    matrix gmtxShadowTransform : packoffset(c16);
    float3 gvCameraPosition : packoffset(c20);
    float gfPad1 : packoffset(c20.w);
    float3 gvLightPosition : packoffset(c21.x);
};


Texture2D shaderTexture : register(t0);
Texture2D ShadowMap : register(t1);

SamplerState gWrapSamplerState : register(s0);
SamplerComparisonState gClampSamplerState : register(s1);

float CalcShadowFactor(float4 shadowPosH)
{
	// Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    ShadowMap.GetDimensions(0, width, height, numMips);

	// Texel size.
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    for (int i = 0; i < 9; ++i)
    {
        percentLit += ShadowMap.SampleCmpLevelZero(gClampSamplerState, shadowPosH.xy, depth).r;
    }

    return percentLit / 9.0f;
}


struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), 
    gmtxProjection);
    output.uv = input.uv;
    return(output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    return (cColor);

}