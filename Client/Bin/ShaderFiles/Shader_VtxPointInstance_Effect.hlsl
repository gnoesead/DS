
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;

texture2D		g_DiffuseTexture;

texture2D		g_RampTexture;

float			g_fAlpha;

float			g_fTimeAcc = 0.f;
float4			g_vPanningSpeed = { 0.f, 0.f, 0.f, 0.f };
float2			g_vFlip = { 0.f, 0.f };

float4			g_vColor;

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
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float4		vColor : COLOR0;	
	float2		vPSize : PSIZE;
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

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float4		vColor : COLOR0;
	float2		vPSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream )
{
	GS_OUT	Out[4];	

	float3		vLook = g_vCamPosition.xyz - In[0].vPosition.xyz;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	

	Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vColor = In[0].vColor;
	Out[0].vTexUV = float2(0.f, 0.f);

	Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vColor = In[0].vColor;
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vColor = In[0].vColor;
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vColor = In[0].vColor;
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

/* 자동. */
/* w나누기 연산이 수행된다. */
/* 뷰포트로의 변환. */

/* 래스터라이즈 : 정점의 정보를 기반으로하여 픽셀을 생성해나가는 과정이다. */
struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD0;
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
	
	vector	vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vColor.a < 0.1f)
		discard;

	//Out.vColor = In.vColor;
	Out.vColor = vColor;

	return Out;		
}

/* 각각의 하드웨어에 맞는 셰이더버젼으로 셰이더를 구동해주기 위해. */
technique11 DefaultTechnique
{		
	/* 기능의 캡슐화. */
	pass General
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		/* 버텍스 쉐이더는 5.0버젼으로 번역하고 VS_MAIN이라는 이름을 가진 함수를 호출해라. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
