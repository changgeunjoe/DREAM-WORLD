cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    matrix gmtxInverseView : packoffset(c8);
    float3 gvCameraPosition : packoffset(c12);
};


Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState gClampSamplerState : register(s0);
SamplerState gWrapSamplerState : register(s1);


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