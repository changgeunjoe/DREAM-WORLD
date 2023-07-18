
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			256

#define DEFAULT_MODE			0
#define CARTOON_MODE			1
#define OUTLINE_MODE			2
#define CELLSHADING_MODE		3
#define DISSOLVE_MODE		    4

struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular;
    float4 m_cEmissive;
};


cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxGameObject: packoffset(c0);
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


cbuffer cbCharacterInfo : register(b6)//캐릭터별 체력과 림라이트 활성화 여부 
{
    float  gfCharactertHP: packoffset(c0);
    bool bRimLight : packoffset(c0.y);
};
cbuffer cbMultiSpriteInfo : register(b7)//멀티스프라이트인포
{
    matrix gmtxTextureview : packoffset(c0);
    bool bMultiSprite : packoffset(c4);

};
cbuffer cbFrameWorkInfo : register(b8) //게임프레임워크인포
{
    float gfTime : packoffset(c0);
    float gfMode : packoffset(c0.y);
};

cbuffer cbUIInfo : register(b9) //캐릭터별 체력과 림라이트 활성화 여부 
{
    bool bUIActive : packoffset(c0);
};

cbuffer cbGameObjectWorld : register(b10) //게임오브젝트 월드변환
{
    matrix gmtxGameObjectWorld : packoffset(c0);
};

cbuffer cbGameObjectColor : register(b11) //캐릭터별 체력과 림라이트 활성화 여부 
{
    float4 gmtxGameObjectColor : packoffset(c0);
    float gmtxSkillTime : packoffset(c1.x);
};
struct INSTANCEDGAMEOBJECTINFO//인스턴싱 데이터를 위한 구조체이다
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

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObjectWorld).xyz;
    output.positionH = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObjectWorld);
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

    //if (input.uv.x < 0.01)
    //{
    //    cColor = float4(0, 0, 0, 1);

    //}
    //if (input.uv.x > gfCharactertHP)
    //{
    //    cColor = float4(0, 0, 0, 1);
    //}
    if (cColor.x < 0.2 || cColor.y < 0.2 || cColor.z < 0.2)
    {
        cColor.w = 0;
    }
    //else{
    //  //  float4 color = float4(0.6,0, 0, 0);
    //   // cColor.xyz = cColor.xyz ;
    //    //lerp(cColor, color,0.5);
    //    float4 color = float4(10, 10, 10, 0);
    //    cColor.xyz = 1 - cColor.xyz;
    //    cColor += gmtxGameObjectColor;
    //}
       
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

VS_TEXTURED_OUTPUT VSUITextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(float4(input.position, 1.0f), gmtxGameObjectWorld),gmtxProjection);
    output.uv = input.uv;
    return (output);
}

VS_TEXTURED_OUTPUT VSTrailTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;
    return (output);
}

float4 PSTexturedTrail(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
       // Sample the texture
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    cColor += gmtxGameObjectColor;
    cColor.w = 0.4;
    return (cColor);
}
VS_TEXTURED_OUTPUT VSBlendTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;
    return (output);
}

float4 PSBlendTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
       // Sample the texture
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    //cColor += gmtxGameObjectColor;
    //cColor.w = 0.4;
    return (cColor);
}
VS_TEXTURED_OUTPUT VSEffect(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObjectWorld), gmtxView), gmtxProjection);
    output.uv = input.uv;
    return (output);
}
float4 PSEffectTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    // Sample the texture
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    if (cColor.x < 0.2 || cColor.y < 0.2 || cColor.z < 0.2)
    {
        cColor.w = 0;
    }
    else
    {
      //  float4 color = float4(0.6,0, 0, 0);
       // cColor.xyz = cColor.xyz ;
        //lerp(cColor, color,0.5);
        float4 color = float4(10, 10, 10, 0);
        cColor.xyz = 1 - cColor.xyz;
        cColor += gmtxGameObjectColor;
    }
       
    return cColor;
}

VS_TEXTURED_OUTPUT VSSpriteAnimation(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObjectWorld), gmtxView), gmtxProjection);
    output.uv = mul(float3(input.uv, 1.0f), (float3x3) (gmtxTextureview)).xy;
    return (output);
}
float4 PSSpriteTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    // Sample the texture
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    //if (cColor.x == 0.0 || cColor.y == 0.0 || cColor.z == 0.0)
    //{
    //    cColor.w = 0;
    //}
  
    return (cColor);
}

float4 PSUITextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{   
       // Sample the texture 
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv);
    //  float4 outlineColor = float4(1, 0, 0, 
    if (input.uv.x > gfCharactertHP)
    {
        return cColor = float4(0, 0, 0, 1);
    }
    //if (cColor.x < 0.1 || cColor.y < 0.1 || cColor.z < 0.1 )
    //{
    //    cColor.w = 0;
    //}
    if (gmtxGameObjectColor.w > 0.5)
        cColor.w= gmtxGameObjectColor.w;
  //  cColor.w = 0.5;
    if (!bUIActive && cColor.w!=0)
    {
        float4 f = float4(0.5, 0.5, 0.5, 0);
        return lerp(f, cColor, 0.4);
    }
    if (input.uv.y<gmtxSkillTime)
    {
        
        float4 f = float4(0.5, 0.5, 0.5, 1);
        return lerp(f, cColor, 0.4);
    }
    //else
    cColor.xyz += gmtxGameObjectColor.xyz;
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
		//0은 조명끔, 조명 좌표계로 바꾸고 텍스쳐 좌표계로 바꿈
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }
    return (output);
}
VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObjectWorld);
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObjectWorld);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObjectWorld);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObjectWorld);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
		//0은 조명끔, 조명 좌표계로 바꾸고 텍스쳐 좌표계로 바꿈
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
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
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
		//0은 조명끔, 조명 좌표계로 바꾸고 텍스쳐 좌표계로 바꿈
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

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObjectWorld), gmtxView), gmtxProjection);
    output.positionL = input.position;

    return (output);
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float4 cColor = SkyCubeTexture.Sample(gClampSamplerState, input.positionL);
    cColor = round(cColor * 8.0f) / 8.0f;
    return (cColor);
}

struct VS_BOUNDING_BOX_OUTPUT
{
    float4 position : SV_POSITION;
};

struct VS_HEAL_RANGE_OUTPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD;
};

VS_BOUNDING_BOX_OUTPUT VSBoundingBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
    VS_BOUNDING_BOX_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObjectWorld), gmtxView), gmtxProjection);
    return (output);
}

float4 PSBoundingBox(VS_BOUNDING_BOX_OUTPUT input) : SV_TARGET
{
    return(float4(1.0f, 1.0f, 1.0f, 1.0f));
}

float4 PSSphere(VS_BOUNDING_BOX_OUTPUT input) : SV_TARGET
{
    return(float4(0.97f, 0.99f, 0.73f, 1.0f));
}

float4 PSNaviMesh(VS_BOUNDING_BOX_OUTPUT input) : SV_TARGET
{
    return(float4(1.0f, 0.6f, 0.0f, 0.3f));
}

VS_HEAL_RANGE_OUTPUT VSHealRange(VS_TEXTURED_INPUT input)
{
    VS_HEAL_RANGE_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObjectWorld), gmtxView), gmtxProjection);
    output.worldPosition = mul(float4(input.position, 1.0f), gmtxGameObjectWorld).xyz;
    output.worldPosition -= gmtxGameObjectWorld._41_42_43;

    return output;
}

float4 PSHealRange(VS_HEAL_RANGE_OUTPUT input) : SV_TARGET
{
    float2 position = input.worldPosition.xz;
    float distance = length(position);

    float frequency = -0.5f;
    float amplitude = 0.5f;
    float timeFactor = 5.0f;
    float wave = sin((distance * frequency) + (gfTime * timeFactor)) * amplitude;

    float alpha = 0.7;
    float4 color;

    alpha = 0.5 - smoothstep(150.0f * 0.0f, 150.0f, distance);

    //if (distance < 150.0f * 0.3f)
    //{
    //    alpha = 0.7;
    //}
    //if (distance > 150.0f * 0.95f && distance < 150.0f)
    //{
    //    // alpha = 1.0 - smoothstep(150.0f * 0.3f, 150.0f, distance);
    //    color = float4(1.0f, 1.0f, 0.8f, 1.0f);
    //    return color;
    //}

    color = float4(0.4, 1.0, 0.4, alpha);
    color.rgb += wave;

    return color;

    //if (distance > 30.0f && distance < 50.0f)
    //{
    //    return float4(1.0, 1.0, 0.8, 1.0);
    //}

    //return float4(0.0f, 0.0f, 0.0f, 0.0f);
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
        output.normalW = mul(input.normal, (float3x3) gmtxGameObjectWorld);
        output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObjectWorld);
        output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
        output.uv = input.uv;
        output.tangentW = mul(input.tangent, (float3x3) gmtxGameObjectWorld);
        output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObjectWorld);
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

//깊이를 저장하는 PS
PS_DEPTH_OUTPUT PSDepthWriteShader(VS_LIGHTING_OUTPUT input)
{
    PS_DEPTH_OUTPUT output;

	//원투 나누기 한 좌표-깊이
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
        positionW = mul(float4(input.position, 1.0f), gmtxGameObjectWorld);
        output.positionW = positionW.xyz;
        output.position = mul(mul(positionW, gmtxView), gmtxProjection);
        output.normalW = mul(float4(input.normal, 0.0f), gmtxGameObjectWorld).xyz;
        output.uv = input.uv;
        output.tangentW = mul(input.tangent, (float3x3) gmtxGameObjectWorld);
        output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObjectWorld);
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
		//0은 조명끔, 조명 좌표계로 바꾸고 텍스쳐 좌표계로 바꿈
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }
    return (output);
}

float dissolveThreshold = 0.5f; // Dissolve 텍스처의 임계값
float dissolveSpeed = 1.0f; // Dissolve 속도

float4 PSShadowMapShadow(VS_SHADOW_MAP_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP) 
        cAlbedoColor = gtxtAlbedoTexture.Sample(gWrapSamplerState, input.uv);
    else
        cAlbedoColor = gMaterial.m_cDiffuse;
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gWrapSamplerState, input.uv);//디퓨즈 컬러로 사용할 예정
    
    
    if (gfMode == DEFAULT_MODE)
    {
        return cAlbedoColor;

    }
    if (gfMode == CELLSHADING_MODE || gfMode == CARTOON_MODE)
    {
           cAlbedoColor = round(cAlbedoColor * 8.0f) / 5.0f; // 등급을 16단계로 나누어 반올림합니다.
    }
    
    float4 cIllumination = Lighting(input.positionW, normalize(input.normalW), true, input.uvs);

    float4 cColor = cAlbedoColor;

    float cThreshold = 0.5f; // 등급을 결정하는 임계값입니다. 값이 작을수록 등급이 높아집니다.

     
    float3 viewDir = normalize(gvCameraPosition - input.positionW);
    
    float3 RimColor = float3(-2.f, -2.f, -2.f);
    float rimPower = 5.0; // 
    float3 normal = normalize(input.normalW); // 
    
    float rim = saturate(dot(normal, viewDir));
    
    //rim *= saturate((rimWidth - distance) / rimWidth);
    float4 Rimline = float4(pow(1 - rim, rimPower) * RimColor, 0.f);
    if (bRimLight)
    {
        if (gfMode == CELLSHADING_MODE || gfMode == OUTLINE_MODE)
        cColor = cColor + Rimline; // Rimline;
    }
    float4 cHDRColor = lerp(cColor, cIllumination, 0.4f);
    float3 cLinearColor = pow(cHDRColor.rgb, 2.2f);
    cLinearColor = (cLinearColor * (1.0f + cLinearColor / 5.5f)) / (1.0f + cLinearColor);
    float3 cGammaColor = pow(cLinearColor, 1.0f / 2.2f);
    
    ////////////////////////////////////////////////////
        // Dissolve 효과를 위한 변수
    float dissolveAmount = 0.0f; // Dissolve 정도
    if (cColor.w < 0.1f && gmtxGameObjectColor.a != 1)
        return cColor;
    //if (cGammaColor.y < gmtxGameObjectColor.w)
    //{
    //    return float4(cGammaColor, 0);
    //}
    // Dissolve 활성화 여부를 확인
    //if (cAlbedoColor.r < gmtxGameObjectColor.w && gfMode == DISSOLVE_MODE)
    //{
    //    return float4(cGammaColor, 0);
    //    //dissolveAmount = saturate((input.positionW.x + input.positionW.y + input.positionW.z) * dissolveSpeed);
    //    //dissolveAmount = 1.0f - dissolveAmount;
    //    //dissolveAmount = step(dissolveThreshold, dissolveAmount);
    //    //float3 finalColor = lerp(cGammaColor.rgb, float3(0.0f, 0.0f, 0.0f), dissolveAmount);

    //    //return float4(finalColor, 1.0f);
    //}

    // 캐릭터가 사라지는 부분과 dissolve된 부분을 혼합


    ////////////////////////////////////////////////////
       
    return float4(cGammaColor, 1 );
    
  
    
   
    //else if (dot(normalize(cIllumination), normalize(gLightDir.xyz)) > cThreshold) // 빛의 방향과 색상 값으로 경계면을 계산합니다.
    // /   return cColor;
    //else
        //return lerp(cColor, cIllumination, 0.4f); // 경계면 이하의 색상 값은 부드럽게 처리합니다.
    //return cColor;
    
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


struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
    
    output.uv0 = float2(0, 1);
    output.uv1 = input.uv1;

    return (output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cColor = shaderTexture.Sample(gWrapSamplerState, input.uv0);
    //float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gssWrap, input.uv1);
//	float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
  //  float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

    return (cColor);
}




