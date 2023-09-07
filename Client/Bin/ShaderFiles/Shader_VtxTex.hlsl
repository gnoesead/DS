#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
texture2D		g_Texture_Mask;
float           g_Alpha;
float           g_UV_Cull;
float           g_Time_X;
float           g_UV_Speed_X;
float           g_Time_Y;
float           g_UV_Speed_Y;
bool            g_Is_Side_Cut_R = false;
bool            g_Is_Side_Cut_L = false;
float           g_Time_Mask_X;
float           g_Tone;
float           g_UV_Cut_MinX;
float           g_UV_Cut_MaxX;
float           g_UV_Cut_MinY;
float           g_UV_Cut_MaxY;

float			g_fUVRatioX, g_fUVRatioY;
float			g_fRatio;
float			g_fColor;


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

PS_OUT  PS_MAIN_ALPHA_2(PS_IN In)
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

PS_OUT  PS_MAIN_ALPHA_PLUS(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	vColor.rgb *= 3.f;

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA_CUT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (In.vTexUV.x <= g_UV_Cut_MinX)
		vColor.w *= 0.f;
	if (In.vTexUV.x >= g_UV_Cut_MaxX)
		vColor.w *= 0.f;

	if (In.vTexUV.y <= g_UV_Cut_MinY)
		vColor.w *= 0.f;
	if (In.vTexUV.y >= g_UV_Cut_MaxY)
		vColor.w *= 0.f;

	vColor.w *= g_Alpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA_MASK(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float2 TexMaskUV = In.vTexUV;

	
	TexMaskUV.x += g_Time_Mask_X;

	if (TexMaskUV.x > 1.f) {
		TexMaskUV.x = 1.f;
	}

	if (TexMaskUV.x < 0.f) {
		TexMaskUV.x = 0.f;
	}
	vector	vColor_Mask = g_Texture_Mask.Sample(LinearSampler, TexMaskUV);

	if (g_Is_Side_Cut_R == true && In.vTexUV.x >= 0.989)
		vColor.w *= 0.f;
	if (g_Is_Side_Cut_L == true && In.vTexUV.x <= 0.01)
		vColor.w *= 0.f;

	vColor.w *= g_Alpha;

	if (TexMaskUV.x > 0.1f && TexMaskUV.x < 0.9f)
		vColor.w *= 1 - vColor_Mask.r;
	
	if (TexMaskUV.x >= 0.9f)
		vColor.w = 0.f;



	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_INK_MASK(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	
	vColor.w *= 1.f - vColor.r;


	Out.vColor = vColor;

	return Out;
}

PS_OUT  PS_MAIN_ALPHA_MASK_GRAY(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float2 TexMaskUV = In.vTexUV;

	TexMaskUV.x += g_Time_Mask_X;

	if (TexMaskUV.x > 1.f) {
		TexMaskUV.x = 1.f;
	}

	if (TexMaskUV.x < 0.f) {
		TexMaskUV.x = 0.f;
	}

	vector	vColor_Mask = g_Texture_Mask.Sample(LinearSampler, TexMaskUV);

	if (g_Is_Side_Cut_R == true && In.vTexUV.x >= 0.989)
		vColor.w *= 0.f;
	if (g_Is_Side_Cut_L == true && In.vTexUV.x <= 0.01)
		vColor.w *= 0.f;

	vColor.w *= g_Alpha;

	if ((TexMaskUV.x > 0.f && TexMaskUV.x < 1.f) && vColor_Mask.r > 0.5f) {
		
		vColor.rgb = dot(vColor.rgb, float3(0.3f, 0.59f, 0.11f));
	}
		
	

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

PS_OUT  PS_SMOKE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float spriteWidth = 1.0 / 4.0; // 4개의 열
	float spriteHeight = 1.0 / 4.0; // 4개의 행

	float2 spriteUV = float2(g_fUVRatioX + In.vTexUV.x * spriteWidth,
		g_fUVRatioY + In.vTexUV.y * spriteHeight);

	vector vDiffuse = g_Texture.Sample(LinearSampler, spriteUV);
	vDiffuse.a = vDiffuse.r * g_Alpha;

	vDiffuse.rgb = (0.2f, 0.2f, 0.2f);

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT  PS_ROOMSMOKE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float spriteWidth = 1.0f / 6.0f; 
	float spriteHeight = 1.0f / 6.0f; 

	float2 spriteUV = float2(g_fUVRatioX + In.vTexUV.x * spriteWidth,
		g_fUVRatioY + In.vTexUV.y * spriteHeight);

	vector vDiffuse = g_Texture.Sample(LinearSampler, spriteUV);
	vDiffuse.a = vDiffuse.r * g_Alpha * 0.9f;

	vDiffuse.rgb = (0.4f, 0.4f, 0.3f);

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT  PS_ALERTCIRCLE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	
	vColor.a *= g_Alpha;


	if (vColor.a < 0.1f)
		discard;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_SWAMP(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.a = vColor.r;

	Out.vColor.rgb = (0.f, 0.f, 0.f);

	if (Out.vColor.a < 0.2f)
		discard;

	return Out;
}

PS_OUT PS_WATEREFFECT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vColor;

	Out.vColor.rgb = (0.f, 0.f, 0.f);

	if (Out.vColor.a < 0.6f)
		discard;

	return Out;
}

PS_OUT  PS_WATERPARTICLEEFFECT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float spriteWidth = 1.0f / 4.0f;
	float spriteHeight = 1.0f / 4.0f;

	float2 spriteUV = float2(g_fUVRatioX + In.vTexUV.x * spriteWidth,
		g_fUVRatioY + In.vTexUV.y * spriteHeight);

	vector vDiffuse = g_Texture.Sample(LinearSampler, spriteUV);
	vDiffuse.a = vDiffuse.r;

	vDiffuse.rgb = float3(0.0f, 0.0f, 0.0f);

	Out.vColor = vDiffuse;
	
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_REDSWAMP(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float flowSpeed = 0.2f; // 텍스처의 흐름 속도 조절 (조절 가능)
	float2 scrolledUV = In.vTexUV + float2(g_Time_X * flowSpeed, g_Time_X * flowSpeed);
	vector vMask = g_Texture_Mask.Sample(LinearSampler, scrolledUV);

	vColor.a = vColor.r;
	if (vColor.a < 0.2f)
		discard;

	vColor.rgb = float3(0.0f, 0.0f, 0.0f);

	vColor.r += vMask.r * g_fRatio;
	
	Out.vColor = vColor;
	
	return Out;
}

PS_OUT  PS_BLACKSMOKEEFECT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float spriteWidth = 1.0f / 4.0f;
	float spriteHeight = 1.0f / 4.0f;

	float2 spriteUV = float2(g_fUVRatioX + In.vTexUV.x * spriteWidth,
		g_fUVRatioY + In.vTexUV.y * spriteHeight);

	vector vDiffuse = g_Texture.Sample(LinearSampler, spriteUV);
	vDiffuse.a = vDiffuse.r;

	vDiffuse.rgb = float3(0.0f, 0.0f, 0.0f);

	Out.vColor = vDiffuse * g_Alpha;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT  PS_GROUNDSMOKE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float spriteWidth = 1.0f / 6.0f;
	float spriteHeight = 1.0f / 6.0f;

	float2 spriteUV = float2(g_fUVRatioX + In.vTexUV.x * spriteWidth,
		g_fUVRatioY + In.vTexUV.y * spriteHeight);

	vector vDiffuse = g_Texture.Sample(LinearSampler, spriteUV);
	vDiffuse.a = vDiffuse.r;

	vDiffuse.rgb = (g_fColor, g_fColor, g_fColor);

	Out.vColor = vDiffuse * g_Alpha;

	//if (Out.vColor.a < 0.01f)
		//discard;

	return Out;
}

PS_OUT  PS_STONEPARTICLE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float spriteWidth = 1.0f / 2.0f;
	float spriteHeight = 1.0f / 2.0f;

	float2 spriteUV = float2(g_fUVRatioX + In.vTexUV.x * spriteWidth,
		g_fUVRatioY + In.vTexUV.y * spriteHeight);

	vector vDiffuse = g_Texture.Sample(LinearSampler, spriteUV);

	vDiffuse.rgb = float3(vDiffuse.g, vDiffuse.g, vDiffuse.g) * 0.2f;

	Out.vColor = vDiffuse;



	if (Out.vColor.a < 0.01f)
		discard;

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
	// 11
	pass Alpha_Mask
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_MASK();
	}
	// 12
	pass Alpha_Mask_Gray
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_MASK_GRAY();
	}
	// 13
	pass Alpha_Basic_Cut
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_CUT();
	}
	// 14
	pass Alpha_Ink_Mask
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_INK_MASK();
	}
	// 15
	pass Alpha_Reverse_X_Plus
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_REVERSE_X();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_PLUS();
	}

	// 16
	pass Train_Smoke  // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SMOKE();
	}
	// 17
	pass Cloud_LD
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN_REVERSE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_2();
	}

	// 18
	pass AlertCircle // 안원
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALERTCIRCLE();
	}

	// 19
	pass Room_Smoke  // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ROOMSMOKE();
	}

	// 20
	pass Swamp // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SWAMP();
	}

	// 21
	pass WaterEffect // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WATEREFFECT();
	}

	// 22
	pass WaterParticleEffect // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WATERPARTICLEEFFECT();
	}

	// 23
	pass RedSwamp // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_REDSWAMP();
	}

	// 24
	pass BlackSmokeEffect // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKSMOKEEFECT();
	}

	// 25
	pass GroundSmoke // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Not_ZWrite, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GROUNDSMOKE();
	}

	// 26
	pass StoneParticle // (안원)
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_STONEPARTICLE();
	}
}
