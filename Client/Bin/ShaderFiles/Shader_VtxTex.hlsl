#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
float           g_Alpha;
float           g_UV_Cull;
float           g_Time_X;
float           g_UV_Speed_X;
float           g_Time_Y;
float           g_UV_Speed_Y;
bool            g_Is_Side_Cut_R = false;
bool            g_Is_Side_Cut_L = false;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_UV_MOVE(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vTexUV.x += g_Time_X * g_UV_Speed_X;
	Out.vTexUV.y += g_Time_Y * g_UV_Speed_Y;


	return Out;
}

VS_OUT VS_MAIN_REVERSE(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = 1 - In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_REVERSE_X(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV.y = In.vTexUV.y;
	Out.vTexUV.x = 1 - In.vTexUV.x;

	return Out;
}

VS_OUT VS_MAIN_REVERSE_Y(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV.y = 1 - In.vTexUV.y;
	Out.vTexUV.x = In.vTexUV.x;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};


PS_OUT  PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_FADE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	vColor.w = g_Alpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (g_Is_Side_Cut_R == true && In.vTexUV.x >= 0.989)
		vColor.w *= 0.f;
	if (g_Is_Side_Cut_L == true && In.vTexUV.x <= 0.01)
		vColor.w *= 0.f;

	vColor.w *= g_Alpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA_UV(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);


	if (In.vTexUV.x < g_UV_Cull)
		vColor.w = 0.f;

	vColor.w *= g_Alpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA_UV_SIDE_CUT_R(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);


	if (In.vTexUV.x >= 0.989)
		vColor.w *= 0.f;
	
	vColor.w *= g_Alpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA_UV_SIDE_CUT_L(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);


	if (In.vTexUV.x <= 0.01)
		vColor.w *= 0.f;

	vColor.w *= g_Alpha;

	Out.vColor = vColor;

	return Out;
}

technique11 DefaultTechnique
{
	// 0
	pass NonAlpha
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	// 1
	pass Alpha_Basic
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	// 2
	pass Alpha_Reverse
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_REVERSE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	// 3
	pass Fade
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FADE();
	}
	// 4
	pass Alpha_UV
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_UV();
	}
	// 5
	pass Alpha_Reverse_UV
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_REVERSE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_UV();
	}
	// 6
	pass Alpha_Reverse_X
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_REVERSE_X();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	// 7
	pass Alpha_UV_Side_Cut_R
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_UV_SIDE_CUT_R();
	}
	// 8
	pass Alpha_UV_Move
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_UV_MOVE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	// 9
	pass Alpha_Reverse_Y
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_REVERSE_Y();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	// 10
	pass Alpha_UV_Side_Cut_L
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_UV_SIDE_CUT_L();
	}
}
