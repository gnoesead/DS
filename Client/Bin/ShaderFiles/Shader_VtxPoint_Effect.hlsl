#include "Shader_Defines.hpp"

/* 전역변수 == 상수(컨스턴트)테이블 */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_RampTexture;
texture2D		g_NoiseTexture1;
texture2D		g_NoiseTexture2;
texture2D		g_DistortionTexture;

vector			g_vCamPosition;

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

float			g_fTextureOrder = { 0.f };
float2			g_fCameraRightLookPos = { 0.f, 0.f };

float2			g_vPaddingStart = { 0.f, 0.f };
float2			g_vPaddingEnd = { 0.f, 0.f };

float2			g_vTileSize = { 1.f, 1.f };			// Width, Height
float2			g_vCurTile = { 0.f, 0.f };			// 열, 행

float3			g_vAxis = { 0.f, 1.f, 0.f };

//--------------------------------------------------

float3			g_vSize;

float           g_Black_Cull_Amount = { 0.3f };

/* 셰이더란 무엇인가?! */

/* 셰이더 : 사용자정의 렌더링 파이프 라인. */
/* DX9 : 고정기능 렌더링 파이프 라인. */

/* ID3DX11Effect pEffect;*/
/* 이제부터 셰이더로 그릴께요. (Apply) */
/* m_pContext->DrawIndexed();*/

struct VS_IN
{
	float3	vPosition : POSITION;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	//Out.vPSize = In.vPSize;

	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	//float2 vPSize : PSIZE;
};

struct GS_OUT
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_DEFAULT(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT	Out[4];

	float3		vLook = g_vCamPosition.xyz - In[0].vPosition.xyz;
	vLook.y = 0;
	float3		vNormalizedLook = normalize(vLook);

	float3 vRight = normalize(cross(g_vAxis, vLook)) * g_vSize.x * 0.5f;
	float3 vUp = normalize(cross(vLook, vRight)) * g_vSize.y * 0.5f;

	float3 vOriginalRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_vSize.x * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);


	Out[0].vPosition = float4(In[0].vPosition.xyz + vNormalizedLook * -0.01 * g_fTextureOrder + vRight + vUp
		+ vOriginalRight * g_fCameraRightLookPos.x + vNormalizedLook * g_fCameraRightLookPos.y, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.f, 0.f);

	Out[1].vPosition = float4(In[0].vPosition.xyz + vNormalizedLook * -0.01 * g_fTextureOrder - vRight + vUp
		+ vOriginalRight * g_fCameraRightLookPos.x + vNormalizedLook * g_fCameraRightLookPos.y, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = float4(In[0].vPosition.xyz + vNormalizedLook * -0.01 * g_fTextureOrder - vRight - vUp
		+ vOriginalRight * g_fCameraRightLookPos.x + vNormalizedLook * g_fCameraRightLookPos.y, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = float4(In[0].vPosition.xyz + vNormalizedLook * -0.01 * g_fTextureOrder + vRight - vUp
		+ vOriginalRight * g_fCameraRightLookPos.x + vNormalizedLook * g_fCameraRightLookPos.y, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.f, 1.f);


	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_vColor;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_DIFFUSE(PS_IN In)
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

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));


	Out.vColor = vDiffuse;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_DIFFDISSOLVE(PS_IN In)
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

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));
	vector vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	// Softness - Transparency
	//float cutoff = saturate(g_fDissolveAmount + (1 - vDiffuse.a));
	//float alpha = smoothstep(cutoff, cutoff + g_fDissolveSoftness, vDiffuse.a);
	// Softness - Color
	//fixed4 rampCol = tex2D(_GradientMap, float2(col, 0)) + _BurnCol * smoothstep(orCol - cutoff, orCol - cutoff + _CutoffSoftness, _BurnSize) * smoothstep(0.001, 0.5, cutoff);
	//float4 rampCol = vDiffuse + smoothstep(0.001, 0.5, cutoff);
	//float4 finalCol = float4(rampCol.rgb * _Color.rgb * rampCol.a, 1);

	//Out.vColor = finalCol;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASKRAMP(PS_IN In)
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

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));
	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vColor = vRamp;
	Out.vColor.a = vMask.r * g_fAlpha;
	
	if (Out.vColor.a < 0.05f)
		discard;

	return Out;
}

PS_OUT PS_DIFFDISTORTION(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

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
	fWeight.x = cos(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * g_fDistortionStrength;

	vector vMtrlEffect = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + fWeight);
	Out.vColor = vMtrlEffect;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASKDISTORTION_NONEZ(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

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

	Out.vColor = float4(0.f, 0.f, 0.f, 0.f);

	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;

	vector vMtrlEffect = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + fWeight);
	Out.vColor = vMtrlEffect;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASKCOLOR(PS_IN In)
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

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));

	Out.vColor = g_vColor;
	Out.vColor.a = vMask.r;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.05f)
		discard;

	return Out;
}

PS_OUT PS_DIFFUSESPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

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

	Out.vColor = vDiffuse;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASKCOLORSPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

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

	Out.vColor = g_vColor;
	
	Out.vColor.a = vMask.r * g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASKRAMPSPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

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

	Out.vColor = vRamp;
	Out.vColor.a = vMask.r * g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASKCOLORDISSOLVE(PS_IN In)
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

	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(UVX, UVY));
	vector vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = g_vColor;
	Out.vColor.a = vMask.r;

	Out.vColor.a *= g_fAlpha;

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	if (Out.vColor.a < 0.05f)
		discard;

	return Out;
}

PS_OUT PS_DIFF_CALCRED_DISSOLVE(PS_IN In)
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

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(UVX, UVY));


	float amount = (vDiffuse.r + vDiffuse.g + vDiffuse.b) / 3.f;

	vDiffuse.a *= amount;

	vDiffuse.a += amount * g_Black_Cull_Amount;

	Out.vColor = vDiffuse;

	vector vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	// Softness - Transparency
	//float cutoff = saturate(g_fDissolveAmount + (1 - vDiffuse.a));
	//float alpha = smoothstep(cutoff, cutoff + g_fDissolveSoftness, vDiffuse.a);
	// Softness - Color
	//fixed4 rampCol = tex2D(_GradientMap, float2(col, 0)) + _BurnCol * smoothstep(orCol - cutoff, orCol - cutoff + _CutoffSoftness, _BurnSize) * smoothstep(0.001, 0.5, cutoff);
	//float4 rampCol = vDiffuse + smoothstep(0.001, 0.5, cutoff);
	//float4 finalCol = float4(rampCol.rgb * _Color.rgb * rampCol.a, 1);

	//Out.vColor = finalCol;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_DIFF_CALCRED_DISSOLVE_SPRITE(PS_IN In)
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

	float amount = (vDiffuse.r + vDiffuse.g + vDiffuse.b) / 3.f;

	vDiffuse.a *= amount;

	vDiffuse.a += amount * g_Black_Cull_Amount;

	Out.vColor = vDiffuse;

	vector vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_DIFF_DISSOLVE_SPRITE(PS_IN In)
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

	vector vNoise = g_NoiseTexture1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	// Dissolve 효과 계산
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	// Softness - Transparency
	//float cutoff = saturate(g_fDissolveAmount + (1 - vDiffuse.a));
	//float alpha = smoothstep(cutoff, cutoff + g_fDissolveSoftness, vDiffuse.a);
	// Softness - Color
	//fixed4 rampCol = tex2D(_GradientMap, float2(col, 0)) + _BurnCol * smoothstep(orCol - cutoff, orCol - cutoff + _CutoffSoftness, _BurnSize) * smoothstep(0.001, 0.5, cutoff);
	//float4 rampCol = vDiffuse + smoothstep(0.001, 0.5, cutoff);
	//float4 finalCol = float4(rampCol.rgb * _Color.rgb * rampCol.a, 1);

	//Out.vColor = finalCol;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MASK_CALCALPHA_SPRITE_WATERSPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

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

	Out.vColor.rgb = g_vColor;

	Out.vColor.a = vMask.a * g_fAlpha;

	if (Out.vColor.a < 0.6f)
		discard;

	return Out;
}

/* 각각의 하드웨어에 맞는 셰이더버젼으로 셰이더를 구동해주기 위해. */
technique11 DefaultTechnique {
	pass Default // 0
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default , float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}

	pass Diffuse // 1
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE();
	}

	pass DiffuseDissolve	// 2
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFDISSOLVE();
	}

	pass MaskRamp	// 3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMP();
	}

	pass DiffuseDistortion		// 4
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFDISTORTION();
	}

	pass MaskDistortionNoneZ		// 5
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKDISTORTION_NONEZ();
	}

	pass MaskColor		// 6
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLOR();
	}

	pass DiffuseSprite		// 7
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSESPRITE();
	}

	pass MaskColorSprite		// 8
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORSPRITE();
	}

	pass MaskRampSprite		// 9
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPSPRITE();
	}

	pass MaskColorDissolve		// 10
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORDISSOLVE();
	}

	pass DiffuseCalcRedDissolve	// 11
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFF_CALCRED_DISSOLVE();
	}

	pass DiffuseCalcRedDissolve_Sprite	// 12
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFF_CALCRED_DISSOLVE_SPRITE();
	}

	pass DiffuseDissolve_Sprite	// 13
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFF_DISSOLVE_SPRITE();
	}

	pass MaskCalcAlphaSprite_WaterSprite	// 14
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_CALCALPHA_SPRITE_WATERSPRITE();
	}
};