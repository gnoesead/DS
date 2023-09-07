
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;

texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_RampTexture;
texture2D		g_NoiseTexture1;
texture2D		g_NoiseTexture2;
texture2D		g_DistortionTexture;

float			g_fAlpha;

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
float2			g_vTiles = { 1.f, 1.f };			// 열, 행

bool			g_bAlignToDir = { 0 };

struct VS_IN
{
	float3		vPosition : POSITION;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;

	float4		vColor : COLOR0;
	float2		vPSize : PSIZE;
	float4		vAdditional : TEXCOORD5;
	float4		vAdditional2 : TEXCOORD6;
	float4		vLinearSpeed : TEXCOORD7;
	float4		vFrame : COLOR1;
	float4		vAdditional3 : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;

	float4		vColor : COLOR0;	
	float2		vPSize : PSIZE;
	float4		vLook : TEXCOORD3;
	float4		vAdditional : TEXCOORD5;
	float4		vAdditional2 : TEXCOORD6;
	float2		vCurTile : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT		Out = (VS_OUT)0;	

	/* 각 인스턴스당 변환해야할 정보를 가지고 있는 행렬.  */
	/* 여섯개의 정점으로 그릴때 하나의 인스턴이다라는 정의를 렌더링할 설정을 했어. 
	정점의 구성정보를 만들어낼때 인스턴스당 한번씩만 인스턴스버퍼에 들어가있는 정점(r, u, l, t)의 정보를 던져라 .*/
	/* 인스턴스버퍼에 들어가있는 정점(r, u, l, t)의 정보를 이용하여 만든 행렬. */	
	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	float4		vLocalPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vLocalPosition, g_WorldMatrix);
	Out.vColor = In.vColor;
	Out.vPSize = In.vPSize;
	Out.vAdditional = In.vAdditional;
	Out.vAdditional2 = In.vAdditional2;
	Out.vLook = mul(In.vLook, g_WorldMatrix);

	float2 vCurTile = float2(g_vTileSize.x * ((int)In.vAdditional3.x % (int)g_vTiles.x), g_vTileSize.y * ((int)In.vAdditional3.x / (int)g_vTiles.y));

	Out.vCurTile = vCurTile;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;

	float4		vColor : COLOR0;
	float2		vPSize : PSIZE;
	float4		vLook : TEXCOORD3;
	float4		vAdditional : TEXCOORD5;
	float4		vAdditional2 : TEXCOORD6;
	float2		vCurTile : TEXCOORD0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;

	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD1;
	float4		vAdditional : TEXCOORD5;
	float2		vCurTile : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream )
{
	GS_OUT	Out[4];	

	float3		vLook = g_vCamPosition.xyz - In[0].vPosition.xyz;
	float3		vRight;
	float3		vUp;

	if (!g_bAlignToDir)
	{ 
		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f * In[0].vAdditional2.x;
	}
	else
	{
		vRight = normalize(cross(In[0].vLook, vLook)) * In[0].vPSize.x * 0.5f;
		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f * In[0].vAdditional2.x;
	}

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
	Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;
	Out[0].vAdditional = In[0].vAdditional;
	Out[0].vCurTile = In[0].vCurTile;

	Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.f, 0.f);	
	Out[1].vColor = In[0].vColor;
	Out[1].vAdditional = In[0].vAdditional;
	Out[1].vCurTile = In[0].vCurTile;

	Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;
	Out[2].vAdditional = In[0].vAdditional;
	Out[2].vCurTile = In[0].vCurTile;

	Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;
	Out[3].vAdditional = In[0].vAdditional;
	Out[3].vCurTile = In[0].vCurTile;


	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

/* 자동. */
/* w나누기 연산이 수행된다. */
/* 뷰포트로의 변환. */

/* 래스터라이즈 : 정점의 정보를 기반으로하여 픽셀을 생성해나가는 과정이다. */
struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD1;
	float4		vAdditional : TEXCOORD5;
	float2		vCurTile : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;	
};

/* 픽셀의 최종적인 색을 결정한다.  */
// vector PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT  PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_vColor;

	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;		
}

PS_OUT  PS_DIFFUSE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	vector	vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vColor;

	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_DIFFUSESPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	float2 spriteUV = float2(In.vCurTile.x + In.vTexUV.x * g_vTileSize.x,
		In.vCurTile.y + In.vTexUV.y * g_vTileSize.y);

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, spriteUV);

	Out.vColor = vDiffuse;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKCOLOR(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	vector	vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = g_vColor;

	Out.vColor.a = vMask.r;
	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKCOLORSPRITE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	float2 spriteUV = float2(In.vCurTile.x + In.vTexUV.x * g_vTileSize.x,
		In.vCurTile.y + In.vTexUV.y * g_vTileSize.y);

	vector	vMask = g_MaskTexture.Sample(LinearSampler, spriteUV);

	Out.vColor = g_vColor;

	Out.vColor.a = vMask.r;
	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMP(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	vector	vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vColor = vRamp;

	Out.vColor.a = vMask.r;
	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMPSPRITE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	float2 spriteUV = float2(In.vCurTile.x + In.vTexUV.x * g_vTileSize.x,
		In.vCurTile.y + In.vTexUV.y * g_vTileSize.y);

	vector	vMask = g_MaskTexture.Sample(LinearSampler, spriteUV);

	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vColor = vRamp;

	Out.vColor.a = vMask.r;
	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKCOLORDISSOLVE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	vector	vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = g_vColor;

	Out.vColor.a = vMask.r;
	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_MASKRAMPALPHA(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	In.vTexUV.x *= g_vFlip.x;
	In.vTexUV.y *= g_vFlip.y;

	vector	vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	vector vRamp;

	if (g_vGradientOffset.x <= vMask.r && g_vGradientTilling.x >= vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	else if (g_vGradientTilling.x < vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientTilling.x, 0.f));
	else if (g_vGradientOffset.x > vMask.r)
		vRamp = g_RampTexture.Sample(LinearSampler, float2(g_vGradientOffset.x, 0.f));

	Out.vColor = vRamp;

	Out.vColor.a = vMask.r;
	Out.vColor.a *= In.vAdditional.y;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}


technique11 DefaultTechnique
{		
	pass General		// 0
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Diffuse		// 1
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE();
	}

	pass DiffuseSprite		// 2
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSESPRITE();
	}

	pass MaskColor		// 3
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLOR();
	}

	pass MaskColorSprite		// 4
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORSPRITE();
	}

	pass MaskRamp		// 5
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMP();
	}

	pass MaskRampSprite		// 6
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPSPRITE();
	}

	pass MaskColorDissolve		// 7
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKCOLORDISSOLVE();
	}

	pass MaskRampNoZWriteAlpha		// 8
	{
		SetRasterizerState(RS_CULL_NONE);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMPALPHA();
	}
}
