////--------------------------------------------------------------------------------------
//#define MAX_LIGHTS			8 
//#define MAX_MATERIALS		64 

//#define POINT_LIGHT			1
//#define SPOT_LIGHT			2
//#define DIRECTIONAL_LIGHT	3

//#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
//#define _WITH_THETA_PHI_CONES
////#define _WITH_REFLECT

//struct MATERIAL
//{
//    float4 m_cAmbient;
//    float4 m_cDiffuse;
//    float4 m_cSpecular; //a = power
//    float4 m_cEmissive;
//};

//struct LIGHT
//{
//    float4 m_cAmbient;
//    float4 m_cDiffuse;
//    float4 m_cSpecular;
//    float3 m_vPosition;
//    float m_fFalloff;
//    float3 m_vDirection;
//    float m_fTheta; //cos(m_fTheta)
//    float3 m_vAttenuation;
//    float m_fPhi; //cos(m_fPhi)
//    bool m_bEnable;
//    int m_nType;
//    float m_fRange;
//    float padding;
//};

//cbuffer cbMaterial : register(b7)
//{
//    MATERIAL gMaterials[MAX_MATERIALS];
//};

//cbuffer cbLights : register(b8)
//{
//    LIGHT gLights[MAX_LIGHTS];
//    float4 gcGlobalAmbientLight;
//};

//float4 DirectionalLight(int nIndex, float3 vNormal, float3 vToCamera, float vMaterial, float vPower, float shadowFactor)
//{
//    float3 vToLight = -gLights[nIndex].m_vDirection;
//    float fDiffuseFactor = dot(vToLight, vNormal);
//    float fSpecularFactor = 0.0f;
//    if (fDiffuseFactor > 0.0f)
//    {
//        if (vPower != 0.0f)
//        {
//#ifdef _WITH_REFLECT
//			float3 vReflect = reflect(-vToLight, vNormal);
//			fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), vPower);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//            float3 vHalf = normalize(vToCamera + vToLight);
//#else
//			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//            fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), vPower);
//#endif
//        }
//    }

//    return ((gLights[nIndex].m_cAmbient * gMaterials[vMaterial].m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterials[vMaterial].m_cDiffuse) * shadowFactor + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterials[vMaterial].m_cSpecular) * shadowFactor);
//}

//float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera, float vMaterial, float vPower)
//{
//    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
//    float fDistance = length(vToLight);
//    if (fDistance <= gLights[nIndex].m_fRange)
//    {
//        float fSpecularFactor = 0.0f;
//        vToLight /= fDistance;
//        float fDiffuseFactor = dot(vToLight, vNormal);
//        if (fDiffuseFactor > 0.0f)
//        {
//            if (vPower != 0.0f)
//            {
//#ifdef _WITH_REFLECT
//				float3 vReflect = reflect(-vToLight, vNormal);
//				fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), vPower);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//                float3 vHalf = normalize(vToCamera + vToLight);
//#else
//				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//                fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), vPower);
//#endif
//            }
//        }
//        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

//        return (((gLights[nIndex].m_cAmbient * gMaterials[vMaterial].m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterials[vMaterial].m_cDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterials[vMaterial].m_cSpecular)) * fAttenuationFactor);
//    }
//    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
//}

//float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera, float vMaterial, float vPower)
//{
//    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
//    float fDistance = length(vToLight);
//    if (fDistance <= gLights[nIndex].m_fRange)
//    {
//        float fSpecularFactor = 0.0f;
//        vToLight /= fDistance;
//        float fDiffuseFactor = dot(vToLight, vNormal);
//        if (fDiffuseFactor > 0.0f)
//        {
//            if (vPower != 0.0f)
//            {
//#ifdef _WITH_REFLECT
//				float3 vReflect = reflect(-vToLight, vNormal);
//				fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), vPower);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//                float3 vHalf = normalize(vToCamera + vToLight);
//#else
//				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//                fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), vPower);
//#endif
//            }
//        }
//#ifdef _WITH_THETA_PHI_CONES
//        float fAlpha = max(dot(-vToLight, gLights[nIndex].m_vDirection), 0.0f);
//        float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
//#else
//		float fSpotFactor = pow(max(dot(-vToLight, gLights[i].m_vDirection), 0.0f), gLights[i].m_fFalloff);
//#endif
//        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

//        return (((gLights[nIndex].m_cAmbient * gMaterials[vMaterial].m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterials[vMaterial].m_cDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterials[vMaterial].m_cSpecular)) * fAttenuationFactor * fSpotFactor);
//    }
//    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
//}
////Lighting(position, normal, diffuse, specular.x, specular.w*255.0f);
//float4 Lighting(float3 vPosition, float3 vNormal, float3 vDiffuse, float vMaterial, float vPower, float shadowFactor)
//{
//    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
//    float3 vToCamera = normalize(vCameraPosition - vPosition);

//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//    for (int i = 0; i < MAX_LIGHTS; i++)
//    {
//        if (gLights[i].m_bEnable)
//        {
//            if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
//            {
//                cColor += DirectionalLight(i, vNormal, vToCamera, vMaterial, vPower, shadowFactor);
//            }
//            else if (gLights[i].m_nType == POINT_LIGHT)
//            {
//                cColor += PointLight(i, vPosition, vNormal, vToCamera, vMaterial, vPower);
//            }
//            else if (gLights[i].m_nType == SPOT_LIGHT)
//            {
//                cColor += SpotLight(i, vPosition, vNormal, vToCamera, vMaterial, vPower);
//            }
//        }
//    }
//    cColor += (gcGlobalAmbientLight * gMaterials[vMaterial].m_cAmbient) + gMaterials[vMaterial].m_cEmissive;
//    cColor.a = gMaterials[vMaterial].m_cDiffuse.a;

//    return (lerp(float4(vDiffuse, 1.0f), cColor, 0.65f));
//}