////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.vs
////////////////////////////////////////////////////////////////////////////////
#include<Shaders.hlsl>

/////////////
// GLOBALS //
/////////////
cbuffer cbLightInfo : register(b2)
{
    matrix lightViewMatrix : packoffset(c0);
    matrix lightProjectionMatrix : packoffset(c4);
    float4 ambientColor : packoffset(c8);
    float4 diffuseColor : packoffset(c9);
    float3 lightPosition : packoffset(c10.x);
    float padding : packoffset(c10.w);
};
//////////////////////
// CONSTANT BUFFERS //
//////////////////////


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;
    
    
	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, gmtxWorld);
    output.position = mul(output.position, gmtxView);
    output.position = mul(output.position, gmtxProjection);
    
	// 광원에 의해 보았을 때 vertice의 위치를 ​​계산합니다.
    output.lightViewPosition = mul(input.position, gmtxWorld);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
	// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3) gmtxWorld);
	
    // 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);

    // 세계의 정점 위치를 계산합니다.
    worldPosition = mul(input.position, gmtxWorld);

    // 빛의 위치와 세계의 정점 위치를 기반으로 빛의 위치를 ​​결정합니다.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // 라이트 위치 벡터를 정규화합니다.
    output.lightPos = normalize(output.lightPos);

    return output;
}
float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
    float bias;
    float4 color;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;


	// 부동 소수점 정밀도 문제를 해결할 바이어스 값을 설정합니다.
    bias = 0.001f;

	// 모든 픽셀에 대해 기본 출력 색상을 주변 광원 값으로 설정합니다.
    color = ambientColor;

	// 투영 된 텍스처 좌표를 계산합니다.
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// 투영 된 좌표가 0에서 1 범위에 있는지 결정합니다. 그렇다면이 픽셀은 빛의 관점에 있습니다.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
		// 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 깊이 텍스처에서 섀도우 맵 깊이 값을 샘플링합니다.
        depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// 빛의 깊이를 계산합니다.
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// lightDepthValue에서 바이어스를 뺍니다.
        lightDepthValue = lightDepthValue - bias;

		// 섀도우 맵 값의 깊이와 빛의 깊이를 비교하여이 픽셀을 음영 처리할지 조명할지 결정합니다.
		// 빛이 객체 앞에 있으면 픽셀을 비추고, 그렇지 않으면 객체 (오클 루더)가 그림자를 드리 우기 때문에이 픽셀을 그림자로 그립니다.
        if (lightDepthValue < depthValue)
        {
		    // 이 픽셀의 빛의 양을 계산합니다.
            lightIntensity = saturate(dot(input.normal, input.lightPos));

            if (lightIntensity > 0.0f)
            {
				// 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
                color += (diffuseColor * lightIntensity);

				// 최종 빛의 색상을 채웁니다.
                color = saturate(color);
            }
        }
    }

	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
    textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// 빛과 텍스처 색상을 결합합니다.
    color = color * textureColor;

    return color;
}