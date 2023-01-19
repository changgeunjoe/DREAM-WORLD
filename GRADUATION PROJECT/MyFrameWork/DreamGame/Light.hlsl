//--------------------------------------------------------------------------------------
#define max_lights			8 
#define max_materials		64 

#define point_light			1
#define spot_light			2
#define directional_light	3

#define _with_local_viewer_highlighting
#define _with_theta_phi_cones
//#define _with_reflect

struct material
{
    float4 m_cambient;
    float4 m_cdiffuse;
    float4 m_cspecular; //a = power
    float4 m_cemissive;
};

struct light
{
    float4 m_cambient;
    float4 m_cdiffuse;
    float4 m_cspecular;
    float3 m_vposition;
    float m_ffalloff;
    float3 m_vdirection;
    float m_ftheta; //cos(m_ftheta)
    float3 m_vattenuation;
    float m_fphi; //cos(m_fphi)
    bool m_benable;
    int m_ntype;
    float m_frange;
    float padding;
};

cbuffer cbmaterial : register(b3)
{
    material gmaterials[max_materials];
};

cbuffer cblights : register(b2)
{
    light glights[max_lights];
    float4 gcglobalambientlight;
    int gnlights;
};

float4 directionallight(int nindex, float3 vnormal, float3 vtocamera, float vmaterial, float vpower, float shadowfactor)
{
    float3 vtolight = -glights[nindex].m_vdirection;
    float fdiffusefactor = dot(vtolight, vnormal);
    float fspecularfactor = 0.0f;
    if (fdiffusefactor > 0.0f)
    {
        if (vpower != 0.0f)
        {
#ifdef _with_reflect
			float3 vreflect = reflect(-vtolight, vnormal);
			fspecularfactor = pow(max(dot(vreflect, vtocamera), 0.0f), vpower);
#else
#ifdef _with_local_viewer_highlighting
            float3 vhalf = normalize(vtocamera + vtolight);
#else
			float3 vhalf = float3(0.0f, 1.0f, 0.0f);
#endif
            fspecularfactor = pow(max(dot(vhalf, vnormal), 0.0f), vpower);
#endif
        }
    }

    return ((glights[nindex].m_cambient * gmaterials[vmaterial].m_cambient) + (glights[nindex].m_cdiffuse * fdiffusefactor * gmaterials[vmaterial].m_cdiffuse) * shadowfactor + (glights[nindex].m_cspecular * fspecularfactor * gmaterials[vmaterial].m_cspecular) * shadowfactor);
}

float4 pointlight(int nindex, float3 vposition, float3 vnormal, float3 vtocamera, float vmaterial, float vpower)
{
    float3 vtolight = glights[nindex].m_vposition - vposition;
    float fdistance = length(vtolight);
    if (fdistance <= glights[nindex].m_frange)
    {
        float fspecularfactor = 0.0f;
        vtolight /= fdistance;
        float fdiffusefactor = dot(vtolight, vnormal);
        if (fdiffusefactor > 0.0f)
        {
            if (vpower != 0.0f)
            {
#ifdef _with_reflect
				float3 vreflect = reflect(-vtolight, vnormal);
				fspecularfactor = pow(max(dot(vreflect, vtocamera), 0.0f), vpower);
#else
#ifdef _with_local_viewer_highlighting
                float3 vhalf = normalize(vtocamera + vtolight);
#else
				float3 vhalf = float3(0.0f, 1.0f, 0.0f);
#endif
                fspecularfactor = pow(max(dot(vhalf, vnormal), 0.0f), vpower);
#endif
            }
        }
        float fattenuationfactor = 1.0f / dot(glights[nindex].m_vattenuation, float3(1.0f, fdistance, fdistance * fdistance));

        return (((glights[nindex].m_cambient * gmaterials[vmaterial].m_cambient) + (glights[nindex].m_cdiffuse * fdiffusefactor * gmaterials[vmaterial].m_cdiffuse) + (glights[nindex].m_cspecular * fspecularfactor * gmaterials[vmaterial].m_cspecular)) * fattenuationfactor);
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 spotlight(int nindex, float3 vposition, float3 vnormal, float3 vtocamera, float vmaterial, float vpower)
{
    float3 vtolight = glights[nindex].m_vposition - vposition;
    float fdistance = length(vtolight);
    if (fdistance <= glights[nindex].m_frange)
    {
        float fspecularfactor = 0.0f;
        vtolight /= fdistance;
        float fdiffusefactor = dot(vtolight, vnormal);
        if (fdiffusefactor > 0.0f)
        {
            if (vpower != 0.0f)
            {
#ifdef _with_reflect
				float3 vreflect = reflect(-vtolight, vnormal);
				fspecularfactor = pow(max(dot(vreflect, vtocamera), 0.0f), vpower);
#else
#ifdef _with_local_viewer_highlighting
                float3 vhalf = normalize(vtocamera + vtolight);
#else
				float3 vhalf = float3(0.0f, 1.0f, 0.0f);
#endif
                fspecularfactor = pow(max(dot(vhalf, vnormal), 0.0f), vpower);
#endif
            }
        }
#ifdef _with_theta_phi_cones
        float falpha = max(dot(-vtolight, glights[nindex].m_vdirection), 0.0f);
        float fspotfactor = pow(max(((falpha - glights[nindex].m_fphi) / (glights[nindex].m_ftheta - glights[nindex].m_fphi)), 0.0f), glights[nindex].m_ffalloff);
#else
		float fspotfactor = pow(max(dot(-vtolight, glights[i].m_vdirection), 0.0f), glights[i].m_ffalloff);
#endif
        float fattenuationfactor = 1.0f / dot(glights[nindex].m_vattenuation, float3(1.0f, fdistance, fdistance * fdistance));

        return (((glights[nindex].m_cambient * gmaterials[vmaterial].m_cambient) + (glights[nindex].m_cdiffuse * fdiffusefactor * gmaterials[vmaterial].m_cdiffuse) + (glights[nindex].m_cspecular * fspecularfactor * gmaterials[vmaterial].m_cspecular)) * fattenuationfactor * fspotfactor);
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}
//lighting(position, normal, diffuse, specular.x, specular.w*255.0f);
float4 lighting(float3 vposition, float3 vnormal, float3 vdiffuse, float vmaterial, float vpower, float shadowfactor)
{
    float3 vcameraposition = float3(gvcameraposition.x, gvcameraposition.y, gvcameraposition.z);
    float3 vtocamera = normalize(vcameraposition - vposition);

    float4 ccolor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < max_lights; i++)
    {
        if (glights[i].m_benable)
        {
            if (glights[i].m_ntype == directional_light)
            {
                ccolor += directionallight(i, vnormal, vtocamera, vmaterial, vpower, shadowfactor);
            }
            else if (glights[i].m_ntype == point_light)
            {
                ccolor += pointlight(i, vposition, vnormal, vtocamera, vmaterial, vpower);
            }
            else if (glights[i].m_ntype == spot_light)
            {
                ccolor += spotlight(i, vposition, vnormal, vtocamera, vmaterial, vpower);
            }
        }
    }
    ccolor += (gcglobalambientlight * gmaterials[vmaterial].m_cambient) + gmaterials[vmaterial].m_cemissive;
    ccolor.a = gmaterials[vmaterial].m_cdiffuse.a;

    return (lerp(float4(vdiffuse, 1.0f), ccolor, 0.65f));
}