
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			256

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
    bool bAnimationShader : packoffset(c8.y);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    matrix gmtxInverseView : packoffset(c8);
    float3 gvCameraPosition : packoffset(c12);
};

cbuffer cbBoneOffsets : register(b4)
{
    float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b5)
{
    float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};


cbuffer cbCharacterInfo : register(b6)//ĳ���ͺ� ü�°� ������Ʈ Ȱ��ȭ ���� 
{
    float  gfCharactertHP: packoffset(c0);
    bool bRimLight : packoffset(c0.y);

};
cbuffer cbMultiSpriteInfo : register(b7)//��Ƽ��������Ʈ����
{
    matrix gmtxTextureview : packoffset(c0);
    bool bMultiSprite : packoffset(c4);

};


struct INSTANCEDGAMEOBJECTINFO//�ν��Ͻ� �����͸� ���� ����ü�̴�
{
    matrix m_mtxGameObject;
};


Texture2D shaderTexture : register(t0);
TextureCube SkyCubeTexture : register(t2);
Texture2D ShadowMap : register(t1);
Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);
StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t13);

SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

#include"Light.hlsl"

struct CB_TOOBJECTSPACE
{
    matrix mtxToTexture;
    float4 f4Position;
};

cbuffer cbToLightSpace : register(b3)
{
    CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};
//#define _WITH_VERTEX_LIGHTING
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

VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.positionH = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.color = Lighting(output.positionW, normalize(normalW));
    output.color = float4(0.5f * normalize(gvCameraPosition - output.positionW) + 0.5f, 1.0f);
#else
    output.normalW = normalW;
    output.uv = input.uv;
#endif

    return (output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING


        return (cIllumination + float4(input.positionW, 1.0f));
#else
    float3 normalW = normalize(input.normalW);
     float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    if (input.uv.y > 0.95)
    {
        cColor = float4(0, 0, 0, 1);

    }
    if (input.uv.y < 0.05)
    {
        cColor = float4(0, 0, 0, 1);
    }
    if (input.uv.x > 0.95)
    {
        cColor = float4(0, 0, 0, 1);

    }
    if (input.uv.x < 0.01)
    {
        cColor = float4(0, 0, 0, 1);

    }
    if (input.uv.x > gfCharactertHP)
    {
        cColor = float4(0, 0, 0, 1);
    }
     return cColor;

 #endif

}

struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
VS_TEXTURED_OUTPUT VSSpriteAnimation(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = mul(float3(input.uv, 1.0f), (float3x3) (gmtxTextureview)).xy;
    return (output);
}
VS_TEXTURED_OUTPUT VSUITextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(float4(input.position, 1.0f), gmtxGameObject),gmtxProjection);
    output.uv = input.uv;
    return (output);
}

float4 PSUITextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{   
       // Sample the texture
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    //  float4 outlineColor = float4(1, 0, 0, 1);

    //// Find pixels with different colors in the neighborhood
    //float4 left = shaderTexture.Sample(gWrapSamplerState, input.uv - float2(0.99, 0));
    //float4 right = shaderTexture.Sample(gWrapSamplerState, input.uv + float2(0.99, 0));
    //float4 top = shaderTexture.Sample(gWrapSamplerState, input.uv - float2(0,0.99));
    //float4 bottom = shaderTexture.Sample(gWrapSamplerState, input.uv + float2(0,0.99));

    //// Set the outline color for the pixels that have different colors
    //if (cColor.x != left.x || cColor.y != left.y || cColor.z != left.z )
    //{
    //    return outlineColor;
    //}
    if (input.uv.x > gfCharactertHP)
    {
        cColor = float4(0, 0, 0, 1);
    }
    return (cColor);
}

struct VS_STANDARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float2 uv : TEXCOORD;
	
    float4 uvs[MAX_LIGHTS] : TEXCOORD1;
};
VS_STANDARD_OUTPUT VSInstancing(VS_STANDARD_INPUT input, uint nInstanceID : SV_InstanceID)
{
  
    VS_STANDARD_OUTPUT output;

    float4 positionW = mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
		//0�� �����, ���� ��ǥ��� �ٲٰ� �ؽ��� ��ǥ��� �ٲ�
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }
    return (output);
}
VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
		//0�� �����, ���� ��ǥ��� �ٲٰ� �ؽ��� ��ǥ��� �ٲ�
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }
    return (output);
}


float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor = gtxtAlbedoTexture.Sample(gWrapSamplerState, input.uv);
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gWrapSamplerState, input.uv);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gWrapSamplerState, input.uv);
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gWrapSamplerState, input.uv);
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
        cEmissionColor = gtxtEmissionTexture.Sample(gWrapSamplerState, input.uv);

    float3 normalW;
    float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;

    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
    {
        float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] �� [-1, 1]
        normalW = normalize(mul(vNormal, TBN));
    }
    else
    {
        normalW = normalize(input.normalW);
    }
    float4 uvs[MAX_LIGHTS];
    float4 cIllumination = Lighting(input.positionW, normalW, false, uvs);
    
    //return (cIllumination);
     if (cColor.w < 0.1f)
         return cColor;
     else
         return(lerp(cColor, cIllumination, 0.5f));;
}


struct VS_SKINNED_STANDARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    int4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    float4x4 mtxVertexToBoneWorld = (float4x4) 0.0f;
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
    {
        mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
    }
    float4 positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
    output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
    output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    for (int j = 0; j < MAX_LIGHTS; j++)
    {
		//0�� �����, ���� ��ǥ��� �ٲٰ� �ؽ��� ��ǥ��� �ٲ�
        if (gcbToLightSpaces[j].f4Position.w != 0.0f)
            output.uvs[j] = mul(positionW, gcbToLightSpaces[j].mtxToTexture);
    }
	
    return (output);
}

struct VS_SKYBOX_CUBEMAP_INPUT
{
    float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
    float3 positionL : POSITION;
    float4 position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
    VS_SKYBOX_CUBEMAP_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.positionL = input.position;

    return (output);
}



float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float4 cColor = SkyCubeTexture.Sample(gClampSamplerState, input.positionL);

    return (cColor);
}
//////////////////////////////////////////////////////////////////////////shadow



struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    int4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
    
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float2 uv : TEXCOORD;
    
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    if (!bAnimationShader)
    {
        output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
        output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
        output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
        output.uv = input.uv;
        output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
        output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    }
    else if (bAnimationShader)
    {
        float4x4 mtxVertexToBoneWorld = (float4x4) 0.0f;
        for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
        {
            mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
        }
        float4 positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);
        output.positionW = positionW.xyz;
        output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
        output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
        output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;
        output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
        output.uv = input.uv;
    }
    
    
    return (output);
}



struct PS_DEPTH_OUTPUT
{
    float fzPosition : SV_Target;
    float fDepth : SV_Depth;
};

//���̸� �����ϴ� PS
PS_DEPTH_OUTPUT PSDepthWriteShader(VS_LIGHTING_OUTPUT input)
{
    PS_DEPTH_OUTPUT output;

	//���� ������ �� ��ǥ-����
    output.fzPosition = input.position.z;
    output.fDepth = input.position.z;

    return (output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SHADOW_MAP_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;

    float2 uv : TEXCOORD;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float4 uvs[MAX_LIGHTS] : TEXCOORD1;
    
};

VS_SHADOW_MAP_OUTPUT VSShadowMapShadow(VS_LIGHTING_INPUT input)
{
    VS_SHADOW_MAP_OUTPUT output = (VS_SHADOW_MAP_OUTPUT) 0;
    float4 positionW = (float4) 0.0f;
    if (!bAnimationShader)
    {
        positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
        output.positionW = positionW.xyz;
        output.position = mul(mul(positionW, gmtxView), gmtxProjection);
        output.normalW = mul(float4(input.normal, 0.0f), gmtxGameObject).xyz;
        output.uv = input.uv;
        output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
        output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    }
    else if (bAnimationShader)
    {
        float4x4 mtxVertexToBoneWorld = (float4x4) 0.0f;
        for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
        {
            mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
        }
        positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);
        output.positionW = positionW.xyz;
        output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
        output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
        output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;
        output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
        output.uv = input.uv;
    }  
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
		//0�� �����, ���� ��ǥ��� �ٲٰ� �ؽ��� ��ǥ��� �ٲ�
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }
    return (output);
}



float4 PSShadowMapShadow(VS_SHADOW_MAP_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP) 
        cAlbedoColor = gtxtAlbedoTexture.Sample(gWrapSamplerState, input.uv);
    else
        cAlbedoColor = gMaterial.m_cDiffuse;

    cAlbedoColor = round(cAlbedoColor * 8.0f) / 8.0f; // ����� 16�ܰ�� ������ �ݿø��մϴ�.

    float4 cIllumination = Lighting(input.positionW, normalize(input.normalW), true, input.uvs);

    float4 cColor = cAlbedoColor;

    float cThreshold = 0.5f; // ����� �����ϴ� �Ӱ谪�Դϴ�. ���� �������� ����� �������ϴ�.

     
    float3 viewDir = normalize(gvCameraPosition - input.positionW);
    
    float3 RimColor = float3(-2.f, -2.f, -2.f);
    float rimPower = 5.0; // 
    float3 normal = normalize(input.normalW); // 
    
    float rim = saturate(dot(normal, viewDir));
    
    //rim *= saturate((rimWidth - distance) / rimWidth);
    float4 Rimline = float4(pow(1 - rim, rimPower) * RimColor, 0.f);
    if (bRimLight)
    {
        cColor = cColor + Rimline; // Rimline;
    }
    if (cColor.w < 0.1f)
        return cColor;
    //else if (dot(normalize(cIllumination), normalize(gLightDir.xyz)) > cThreshold) // ���� ����� ���� ������ ������ ����մϴ�.
    // /   return cColor;
    else
        return lerp(cColor, cIllumination, 0.4f); // ���� ������ ���� ���� �ε巴�� ó���մϴ�.
    
}
///////////////////////////////////////////////////////////////////////////////
//

VS_TEXTURED_OUTPUT VSTextureToViewport(uint nVertexID : SV_VertexID)
{
    VS_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT) 0;

    if (nVertexID == 0)
    {
        output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    if (nVertexID == 1)
    {
        output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 0.0f);
    }
    if (nVertexID == 2)
    {
        output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    if (nVertexID == 3)
    {
        output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    if (nVertexID == 4)
    {
        output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    if (nVertexID == 5)
    {
        output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 1.0f);
    }
    return (output);
}

//SamplerState gssBorder : register(s3);

float4 PSTextureToViewport(VS_TEXTURED_OUTPUT input) : SV_Target
{
    float fDepthFromLight0 = gtxtDepthTextures[0].SampleLevel(gWrapSamplerState /*gssBorder*/, input.uv, 0).r;

    return ((float4) (fDepthFromLight0 * 0.8f));
}



