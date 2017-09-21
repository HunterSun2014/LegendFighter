// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://create.msdn.com/en-US/education/catalog/sample/stock_effects

Texture2D<float4> Texture : register(t0);
//Texture2D<float2> ShadowMapTex : register(t1);
sampler Sampler : register(s0);
//sampler SamPointClamp : register(s1);

cbuffer Parameters : register(b0)
{
    float4 DiffuseColor             : packoffset(c0);
    float3 EmissiveColor            : packoffset(c1);
    float3 SpecularColor            : packoffset(c2);
    float  SpecularPower            : packoffset(c2.w);

    float3 LightDirection[3]        : packoffset(c3);
    float3 LightDiffuseColor[3]     : packoffset(c6);
    float3 LightSpecularColor[3]    : packoffset(c9);

    float3 EyePosition              : packoffset(c12);

    float3 FogColor                 : packoffset(c13);
    float4 FogVector                : packoffset(c14);

    float4x4 World                  : packoffset(c15);
    float3x3 WorldInverseTranspose  : packoffset(c19);
    float4x4 WorldViewProj			: packoffset(c22);
	float4x4 ShadowMapTransform		: packoffset(c26);

    float4x3 Bones[72]              : packoffset(c30);
};


#include "Structures.fxh"
#include "Common.fxh"
#include "Lighting.fxh"
#include "ShadowMap.fx"

void Skin(inout VSInputNmTxWeights vin, uniform int boneCount)
{
    float4x3 skinning = 0;

    [unroll]
    for (int i = 0; i < boneCount; i++)
    {
        skinning += Bones[vin.Indices[i]] * vin.Weights[i];
    }

    vin.Position.xyz = mul(vin.Position, skinning);
    vin.Normal = mul(vin.Normal, (float3x3)skinning);
}


// Vertex shader: vertex lighting, one bone.
VSOutputTx VSSkinnedVertexLightingOneBone(VSInputNmTxWeights vin)
{
    VSOutputTx vout;

    Skin(vin, 1);

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: vertex lighting, two bones.
VSOutputTx VSSkinnedVertexLightingTwoBones(VSInputNmTxWeights vin)
{
    VSOutputTx vout;

    Skin(vin, 2);

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: vertex lighting, four bones.
VSOutputTx VSSkinnedVertexLightingFourBones(VSInputNmTxWeights vin)
{
    VSOutputTx vout;

    Skin(vin, 4);

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: one light, one bone.
VSOutputTx VSSkinnedOneLightOneBone(VSInputNmTxWeights vin)
{
    VSOutputTx vout;

    Skin(vin, 1);

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 1);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: one light, two bones.
VSOutputTx VSSkinnedOneLightTwoBones(VSInputNmTxWeights vin)
{
    VSOutputTx vout;

    Skin(vin, 2);

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 1);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: one light, four bones.
VSOutputTx VSSkinnedOneLightFourBones(VSInputNmTxWeights vin)
{
    VSOutputTx vout;

    Skin(vin, 4);

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 1);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: pixel lighting, one bone.
VSOutputPixelLightingTx VSSkinnedPixelLightingOneBone(VSInputNmTxWeights vin)
{
    VSOutputPixelLightingTx vout;

    Skin(vin, 1);

    CommonVSOutputPixelLighting cout = ComputeCommonVSOutputPixelLighting(vin.Position, vin.Normal);
    SetCommonVSOutputParamsPixelLighting;

    vout.Diffuse = float4(1, 1, 1, DiffuseColor.a);
    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: pixel lighting, two bones.
VSOutputPixelLightingTx VSSkinnedPixelLightingTwoBones(VSInputNmTxWeights vin)
{
    VSOutputPixelLightingTx vout;

    Skin(vin, 2);

    CommonVSOutputPixelLighting cout = ComputeCommonVSOutputPixelLighting(vin.Position, vin.Normal);
    SetCommonVSOutputParamsPixelLighting;

    vout.Diffuse = float4(1, 1, 1, DiffuseColor.a);
    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: pixel lighting, four bones.
VSOutputPixelLightingTx VSSkinnedPixelLightingFourBones(VSInputNmTxWeights vin)
{
    VSOutputPixelLightingTx vout;

    Skin(vin, 4);

    CommonVSOutputPixelLighting cout = ComputeCommonVSOutputPixelLighting(vin.Position, vin.Normal);
    SetCommonVSOutputParamsPixelLighting;

    vout.Diffuse = float4(1, 1, 1, DiffuseColor.a);
    vout.TexCoord = vin.TexCoord;

    return vout;
}

// Pixel shader: vertex lighting.
float4 PSSkinnedVertexLighting(PSInputTx pin) : SV_Target0
{
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * pin.Diffuse;

    AddSpecular(color, pin.Specular.rgb);
    ApplyFog(color, pin.Specular.w);

    return color;
}


// Pixel shader: vertex lighting, no fog.
float4 PSSkinnedVertexLightingNoFog(PSInputTx pin) : SV_Target0
{
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * pin.Diffuse;

    AddSpecular(color, pin.Specular.rgb);

    return color;
}


// Pixel shader: pixel lighting.
float4 PSSkinnedPixelLighting(VSOutputPixelLightingTx pin) : SV_Target0
{
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * pin.Diffuse;

    float3 eyeVector = normalize(EyePosition - pin.PositionWS.xyz);
    float3 worldNormal = normalize(pin.NormalWS);

	float litPercent = CalcShadowFactor(SamPointClamp, ShadowMapTex, pin.ShadowPos );

    ColorPair lightResult = ComputeLightsWithShadow(eyeVector, worldNormal, litPercent,3);
	
    color.rgb *= (lightResult.Diffuse);	//ambient 0.2f

    AddSpecular(color, lightResult.Specular);
    ApplyFog(color, pin.PositionWS.w);

    return color;
}


//--------------------------------------------------------------------------------------
//  Vertex Shader : Skinned shadow map
//--------------------------------------------------------------------------------------
V2P_SHADOW_MAP  VSSkinnedShadowMap(VSInputNmTxWeights Input)
{
	Skin(Input, 4);

	float4 pos = mul(Input.Position, WorldViewProj);
	
	return VSShadowMap(pos);
}

//--------------------------------------------------------------------------------------
//  Vertex Shader : Skinned with Normal and Sepcular mapping
//--------------------------------------------------------------------------------------
PSNormalMapping VSSkinnedNormalMapping(VSNormalMapping input)
{
	PSNormalMapping output = (PSNormalMapping)0;

	return output;
}

//--------------------------------------------------------------------------------------
//  Pixel Shader : Normal mapping
//--------------------------------------------------------------------------------------
float4 PSNormaMapping(PSNormalMapping input) : SV_Target0
{
	float4 output = float4(0, 0, 0, 0);

	return output;
}

//--------------------------------------------------------------------------------------
//  Pixel Shader : Normal and Specular mapping
//--------------------------------------------------------------------------------------
float4 PSNormalSpecularMapping(PSNormalMapping input) : SV_Target0
{
	float4 output = float4(0, 0, 0, 0);

	return output;
}