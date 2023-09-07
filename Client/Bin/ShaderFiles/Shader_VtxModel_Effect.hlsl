
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
float2			g_vOffset = { 0.f, 0.f };
float2			g_vTilling = { 1.f, 1.f };

float2			g_vGradientOffset = { 0.f, 0.f };
float2			g_vGradientTilling = { 1.f, 1.f };

float			g_fDissolveAmount;
//float			g_fDissolveSoftness;
float			g_fDistortionStrength = { 0.f };
float			g_fDistortionSpeed = { 0.f };
vector			g_vColor;
float2			g_vDiscardedPixelMin = { 0.f, 0.f };
float2			g_vDiscardedPixelMax = { 1.f, 1.f };
float2			g_vPixelDiscardSpeedMin = { 0.f, 0.f };
float2			g_vPixelDiscardSpeedMax = { 0.f, 0.f };
float			g_fDiscardTimeAcc = { 0.f };

float2			g_vPaddingStart = { 0.f, 0.f };
float2			g_vPaddingEnd = { 0.f, 0.f };

float2			g_vTileSize = { 1.f, 1.f };			// Width, Height
float2			g_vCurTile = { 0.f, 0.f };			// 열, 행

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

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_DIFFUSE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		// 밖으로 빼지 말것
		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		// 밖으로 빼지 말것
		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a *= g_fAlpha;
	 
	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_DIFFUSE_CALC_RED(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));

	float amount = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;

	vMtrlDiffuse.a *= amount;

	vMtrlDiffuse.a += amount * 0.5f;

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_DIFFDISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));

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

	Out.vDiffuse.a = g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMP(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));
	vector vRamp;

	if(g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vDiffuse = vRamp;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_NORMALDISTORTION(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}
	
	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));

	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;

	Out.vDiffuse = g_MaskTexture.Sample(LinearSampler, In.vTexUV + fWeight);
	Out.vDiffuse = Out.vDiffuse.r * 1.4f;
	
	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_MASKCOLOR(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));

	Out.vDiffuse = g_vColor;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	//vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	//float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	//vNormal = mul(vNormal, WorldMatrix);

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 1.f);
	//Out.vAdditional = vector(1.f, 0.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_MASKCOLORDISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));

	Out.vDiffuse = g_vColor;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	//vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	//float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	//vNormal = mul(vNormal, WorldMatrix);

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

PS_OUT  PS_MASKRAMPDISTORTION(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * g_fDistortionStrength;

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX + fWeight.x, UVY + fWeight.y));
	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vDiffuse = vRamp;
	Out.vDiffuse.a = Out.vDiffuse.a * g_fAlpha;

	Out.vDiffuse.a = Out.vDiffuse.r * g_fAlpha;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMPDISTORTIONDISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));

	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;

	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));


	Out.vDiffuse = g_MaskTexture.Sample(LinearSampler, In.vTexUV + fWeight) * vRamp ;

	Out.vDiffuse.a = Out.vDiffuse.r * g_fAlpha;

	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	return Out;
}

PS_OUT  PS_MASKBLACKNODISCARD(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));

	Out.vDiffuse = g_vColor * vMask.r + float4(0.f, 0.f, 0.f, 1.f) * (1 - vMask.r);
	Out.vDiffuse.a = g_fAlpha;

	//vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	//float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	//vNormal = mul(vNormal, WorldMatrix);

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 1.f);
	//Out.vAdditional = vector(1.f, 0.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMPDISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));
	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vDiffuse = vRamp;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	
	if (Out.vDiffuse.a < 0.01f)
		discard;

	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	return Out;
}

PS_OUT  PS_MASKCOLORDISTORTION(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc * g_fDistortionSpeed) * g_fDistortionStrength;

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX + fWeight.x, UVY + fWeight.y));

	Out.vDiffuse = g_vColor;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT  PS_DIFFUSE_CALC_RED_DISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));

	float amount = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;

	vMtrlDiffuse.a *= amount;

	vMtrlDiffuse.a += amount * 0.5f;

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	return Out;
}

PS_OUT PS_DIFFUSESPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	float2 spriteUV = float2(g_vCurTile.x + UVX * g_vTileSize.x,
		g_vCurTile.y + UVY * g_vTileSize.y);

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, spriteUV);

	Out.vDiffuse = vDiffuse;

	Out.vDiffuse.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_MASKCOLORSPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	float2 spriteUV = float2(g_vCurTile.x + UVX * g_vTileSize.x,
		g_vCurTile.y + UVY * g_vTileSize.y);

	vector vMask = g_MaskTexture.Sample(LinearSampler, spriteUV);

	Out.vDiffuse = g_vColor;

	Out.vDiffuse.a = vMask.r * g_fAlpha;

	return Out;
}

PS_OUT PS_MASKRAMPSPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	float2 spriteUV = float2(g_vCurTile.x + UVX * g_vTileSize.x,
		g_vCurTile.y + UVY * g_vTileSize.y);

	vector vMask = g_MaskTexture.Sample(LinearSampler, spriteUV);
	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vDiffuse = vRamp;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	return Out;
}


PS_OUT PS_MASKRAMPSPRITE_DISSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	float2 spriteUV = float2(g_vCurTile.x + UVX * g_vTileSize.x,
		g_vCurTile.y + UVY * g_vTileSize.y);

	vector vMask = g_MaskTexture.Sample(LinearSampler, spriteUV);
	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vDiffuse = vRamp;
	Out.vDiffuse.a = vMask.r * g_fAlpha;

	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	return Out;
}


PS_OUT  PS_DIFFCALCRED_DISSOLVE_SPRITE_NOZWRITE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	In.vTexUV.x += g_fTimeAcc * g_vPanningSpeed.x;
	In.vTexUV.y += g_fTimeAcc * g_vPanningSpeed.y;

	In.vTexUV.x += g_vPaddingStart.x;
	In.vTexUV.y += g_vPaddingStart.y;

	In.vTexUV.x += g_vPaddingEnd.x;
	In.vTexUV.y += g_vPaddingEnd.y;

	float UVX = In.vTexUV.x;
	float UVY = In.vTexUV.y;

	if (g_vPanningSpeed.x == 0)
	{
		if (In.vTexUV.x < g_vDiscardedPixelMin.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.x)
			discard;

		if (In.vTexUV.x > g_vDiscardedPixelMax.x + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.x)
			discard;

		if (g_vOffset.x > In.vTexUV.x)
			discard;
		else if (g_vTilling.x < In.vTexUV.x)
			discard;
		else
		{
			UVX = (In.vTexUV.x - g_vOffset.x) / (g_vTilling.x - g_vOffset.x);
			UVX *= (g_vDiscardedPixelMax.x - g_vDiscardedPixelMin.x);
			UVX += g_vDiscardedPixelMin.x;
		}
	}

	if (g_vPanningSpeed.y == 0)
	{
		if (In.vTexUV.y < g_vDiscardedPixelMin.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMin.y)
			discard;

		if (In.vTexUV.y > g_vDiscardedPixelMax.y + g_fDiscardTimeAcc * g_vPixelDiscardSpeedMax.y)
			discard;

		if (g_vOffset.y > In.vTexUV.y)
			discard;
		else if (g_vTilling.y < In.vTexUV.y)
			discard;
		else
		{
			UVY = (In.vTexUV.y - g_vOffset.y) / (g_vTilling.y - g_vOffset.y);
			UVY *= (g_vDiscardedPixelMax.y - g_vDiscardedPixelMin.y);
			UVY += g_vDiscardedPixelMin.y;
		}
	}

	float2 spriteUV = float2(g_vCurTile.x + UVX * g_vTileSize.x,
		g_vCurTile.y + UVY * g_vTileSize.y);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, spriteUV);

	float amount = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;

	vMtrlDiffuse.a *= amount;

	vMtrlDiffuse.a += amount * 0.5f;

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a *= g_fAlpha;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	vector	vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
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
		SetRasterizerState(RS_CULL_NONE);
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
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		//SetDepthStencilState(DS_Default, 0);
		
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMP();
	}

	pass NormalDistortion	// 5
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_NORMALDISTORTION();
	}

	pass MaskColor	// 6
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLOR();
	}

	pass MaskColorDissolve	// 7
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORDISSOLVE();
	}

	pass MaskRampDistortion	// 8
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPDISTORTION();
	}

	pass MaskRampDistortionDissolve	// 9
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPDISTORTIONDISSOLVE();
	}

	pass MaskBlackNoDiscard	// 10
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKBLACKNODISCARD();
	}

	pass MaskColorNoZWrite	// 11
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLOR();
	}

	pass MaskColorDistortion	// 12
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORDISTORTION();
	}

	pass MaskRampDissolve	// 13
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPDISSOLVE();
	}

	pass MaskRampNoZWrite	// 14
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMP();
	}

	pass DiffuseCalcRedDissolve	// 15
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE_CALC_RED_DISSOLVE();
	}

	pass DiffuseCalcRedNoiseNoZWrite	// 16
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE_CALC_RED_DISSOLVE();
	}

	pass DiffuseCalcRedNoZWrite		//17
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE_CALC_RED();
	}

	pass DiffuseSprite		// 18
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSESPRITE();
	}

	pass MaskColorSprite		// 19
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORSPRITE();
	}

	pass MaskRampSprite		// 20
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPSPRITE();
	}

	pass DiffuseCalcRedDissolveSpriteNoZWrite		// 21
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFCALCRED_DISSOLVE_SPRITE_NOZWRITE();
	}

	pass MaskRampSpriteDissolve		// 22
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPSPRITE_DISSOLVE();
	}

	pass MaskColorDissolveNoZWrite		// 23
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORDISSOLVE();
	}
}
