struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular; //a = power
    float4 m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
    MATERIAL gMaterial : packoffset(c4);
    uint gObjectID : packoffset(c8.x);
    float RimPower : packoffset(c8.y);
};

#include "Light.hlsl"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float3 RimLighting(float3 color, float3 pos, float3 normal, float3 rimColor, float rimPower)
{
    float rim = 1.0f - saturate(dot(normalize(gvCameraPosition.xyz - pos), normal));
    float4 cRimColor = float4(rimColor.x, rimColor.g, rimColor.b, 1.0f);
    color += cRimColor.rgb * pow(rim, rimPower);
    
    return color;
}

//#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
   float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
   output.normalW = normalize(output.normalW);
   output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
   return(input.color);
#else
    input.normalW = normalize(input.normalW);
    float4 color = Lighting(input.positionW, input.normalW);

    if (gObjectID == 0)
        color.rgb = RimLighting(color.rgb, input.positionW, input.normalW, float3(0.0, 0.0, 0.0), 0.0);
    else if (gObjectID == 1)
        color.rgb = RimLighting(color.rgb, input.positionW, input.normalW, float3(1.0, 1.0, 1.0), 4.0);
    else
    {
        if(gObjectID % 2 == 0)
            color.rgb = RimLighting(color.rgb, input.positionW, input.normalW, float3(1.0, 0.0, 0.0), RimPower);
        else        
            color.rgb = RimLighting(color.rgb, input.positionW, input.normalW, float3(0.0, 0.0, 1.0), RimPower);
    }
        
    

    return (color);
#endif
}

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�. 
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};
//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
//���� ���̴��� �����Ѵ�. 
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}
//�ȼ� ���̴��� �����Ѵ�. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}