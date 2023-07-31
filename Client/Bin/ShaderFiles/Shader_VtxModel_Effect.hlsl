
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vCamPosition;

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_NoiseTexture1;
texture2D		g_NoiseTexture2;
texture2D		g_MaskTexture;
texture2D		g_DistortionTexture;
texture2D		g_RampTexture;

float			g_fAlpha;
float			g_fFar = 1000.f;

float			g_fTimeAcc = 0.f;
float4			g_vPanningSpeed = { 0.f, 0.f, 0.f, 0.f };
float2			g_vFlip = { 1.f, 1.f };
float4			g_vPanningSpeedNormal = { 0.f, 0.f, 0.f, 0.f };

float			g_fDissolveAmount;
//float			g_fDissolveSoftness;
float			g_fDistortionStrength = { 0.f };
float			g_fDistortionSpeed = { 0.f };
vector			g_vColor;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct PS_OUT
{
	//vector		vColor : SV_TARGET0;
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vAdditional : SV_TARGET3;
};

PS_OUT  PS_DEFAULT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vDiffuse = g_vColor;

	return Out;
}

PS_OUT  PS_DIFFUSE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;

	return Out;
}

PS_OUT  PS_DIFFUSE_CALC_RED(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.r < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;

	return Out;
}

PS_OUT  PS_DIFFDISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	//vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	//float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	//vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 1.f);
	//Out.vAdditional = vector(1.f, 0.f, 0.f, 0.f);

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMP(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector vRamp;

	vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	Out.vDiffuse = vRamp;
	Out.vDiffuse.a = vMask.r;

	//vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	//float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	//vNormal = mul(vNormal, WorldMatrix);

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 1.f);
	//Out.vAdditional = vector(1.f, 0.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.05f)
		discard;

	return Out;
}

PS_OUT  PS_NORMALDISTORTION(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeedNormal.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeedNormal.y;

	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 1.f);
	Out.vAdditional = vector(1.f, 0.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.05f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default	//0
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}

	pass Diffuse	//1
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE();
	}

	pass DiffuseCalcRed		//2
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE_CALC_RED();
	}

	pass DiffuseDissolve	// 3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFDISSOLVE();
	}

	pass MaskRamp	// 4
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMP();
	}

	pass NormalDistortion	// 5
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_NORMALDISTORTION();
	}
}
