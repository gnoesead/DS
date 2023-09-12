#include "..\Public\Renderer.h"

#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "PipeLine.h"
#include "GameInstance.h"
#include "UI.h"
#include "MasterEffect.h"


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pTarget_Manager->Initialize(m_pDevice, m_pContext);

	/*_uint   iNumViewports = { 1 };
	D3D11_VIEWPORT   Viewport;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);*/

	_uint      iNumViewport = 1;
	m_pContext->RSGetViewports(&iNumViewport, &m_VP);

	/* For.Target_Diffuse */
	_float4 vColor_Diffuse = { 1.f, 0.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_B8G8R8A8_UNORM, vColor_Diffuse)))
		return E_FAIL;

	_float4 vColor_Diffuse_Cha = { 1.f, 0.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse_Cha")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_B8G8R8A8_UNORM, vColor_Diffuse_Cha)))
		return E_FAIL;

	/* For.Target_Normal */
	_float4 vColor_Normal = { 1.f, 1.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R16G16B16A16_UNORM, vColor_Normal)))
		return E_FAIL;
	/* For.Target_Depth */
	_float4 vColor_Depth = { 0.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_Depth)))
		return E_FAIL;
	/* For.Target_Shade */
	_float4 vColor_Shade = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R16G16B16A16_UNORM, vColor_Shade)))
		return E_FAIL;
	/* For.Target_Specular */
	_float4 vColor_Specular = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R16G16B16A16_UNORM, vColor_Specular)))
		return E_FAIL;

	/* For.Target_ShadowDepth */
	_uint      iShadowMapCX = 12800;
	_uint      iShadowMapCY = 7200;
	_float4 vColor_ShadowDepth = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"), iShadowMapCX, iShadowMapCY, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_ShadowDepth)))
		return E_FAIL;
	

	///* For.Target_SSAO */
	_float4 vColor_SSAO = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAO")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, vColor_SSAO)))
		return E_FAIL;

	///* For.Target_BlurX */
	_float4 vColor_BlurX = { 1.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_BlurX)))
		return E_FAIL;

	///* For.Target_BlurY */
	_float4 vColor_BlurY = { 1.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_BlurY)))
		return E_FAIL;

	///* For.Target_CombineBlur */
	_float4 vColor_Blur = { 1.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_CombineBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_Blur)))
		return E_FAIL;

	///* For.Target_SSAOBlurX */
	//_float4 vColor_BlurX = { 1.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAOBlurX")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_BlurX)))
		return E_FAIL;

	///* For.Target_SSAOBlurY */
	//_float4 vColor_BlurY = { 1.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAOBlurY")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_BlurY)))
		return E_FAIL;

	///* For.Target_SSAOBlur */
	_float4 vColor_SSAOBlur = { 1.f, 1.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAOBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_SSAOBlur)))
		return E_FAIL;
	
	/* For.Target_ShadowBlurX */
	_float4 vColor_ShadowBlurX = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowBlurX")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_ShadowBlurX)))
		return E_FAIL;

	///* For.Target_ShadowBlurY */
	_float4 vColor_ShadowBlurY = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowBlurY")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_ShadowBlurY)))
		return E_FAIL;

	///* For.Target_ShadowBlur */
	_float4 vColor_ShadowBlur = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_ShadowBlur)))
		return E_FAIL;

	/* For.Target_ExportFinal */
	_float4 vColor_ExportTexture = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ExportFinal")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_ExportTexture)))
		return E_FAIL;

	/* For.Target_HDR */
	_float4 vColor_HDRTexture = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_HDRTexture)))
		return E_FAIL;

	/* For.Target_EffectDiffuse */
	_float4 vColor_EffectDiffuse = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectDiffuse")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectDiffuse)))
		return E_FAIL;

	/* For.Target_EffectDiffuse */
	_float4 vColor_EffectDistortion = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectDistortion")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectDistortion)))
		return E_FAIL;

	/* For.Target_EffectBlurX */
	_float4 vColor_DiffuseBlurX = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_DiffuseBlurX")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_DiffuseBlurX)))
		return E_FAIL;
	/* For.Target_EffectBlurY */
	_float4 vColor_DiffuseBlurY = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_DiffuseBlurY")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_DiffuseBlurY)))
		return E_FAIL;
	/* For.Target_EffectCombineBlur */
	_float4 vColor_EffectDiffuseCombineBlur = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectDiffuseCombineBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectDiffuseCombineBlur)))
		return E_FAIL;

	/* For.Target_EffectBloom */
	_float4 vColor_EffectBloom = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectBloom")
		, (_uint)(m_VP.Width), (_uint)(m_VP.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectBloom)))
		return E_FAIL;
	/* For.Target_EffectBlurX */
	_float4 vColor_EffectBlurX = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectBlurX")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectBlurX)))
		return E_FAIL;
	/* For.Target_EffectBlurY */
	_float4 vColor_EffectBlurY = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectBlurY")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectBlurY)))
		return E_FAIL;
	/* For.Target_EffectCombineBlur */
	_float4 vColor_EffectCombineBlur = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectCombineBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectCombineBlur)))
		return E_FAIL;
	_float4 vColor_EffectColor = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectColor")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectColor)))
		return E_FAIL;

	_float4 vColor_RadialColor = { 0.f, 0.f, 0.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_RadialBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_RadialColor)))
		return E_FAIL;

	_float4 vColor_EffectRadialColor = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_EffectRadialBlur")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_EffectRadialColor)))
		return E_FAIL;

	_float4 vColor_Emissive = { 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Emissive")
		, (_uint)m_VP.Width, (_uint)m_VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, vColor_Emissive)))
		return E_FAIL;


	/* For.MRT_GameObject */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse_Cha"))))
		return E_FAIL;


	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	// For.MRT_ShadowDepth(Shadow)
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;

	// For.MRT_SSAO
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;

	// For.MRT_Blur
	// 
	//최종블러
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_BlurX"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_BlurY"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_CombineBlur"), TEXT("Target_CombineBlur"))))
		return E_FAIL;

	//SSAO블러
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAOBlurX"), TEXT("Target_BlurX"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAOBlurY"), TEXT("Target_BlurY"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAOBlur"), TEXT("Target_SSAOBlur"))))
		return E_FAIL;
	//레디얼블러
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("Target_RadialBlur"))))
		return E_FAIL;

	//그림자블러
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ShadowBlurX"), TEXT("Target_ShadowBlurX"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ShadowBlurY"), TEXT("Target_ShadowBlurY"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ShadowBlur"), TEXT("Target_ShadowBlur"))))
		return E_FAIL;

	//HDR
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ExportHDR"), TEXT("Target_ExportFinal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;

	////////////////////////////Effect///////////////////////////////////
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_EffectDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_EffectDistortion"))))
		return E_FAIL;
	//디퓨즈
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectColor"), TEXT("Target_EffectColor"))))
		return E_FAIL;
	//블룸
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectBloom"), TEXT("Target_EffectBloom"))))
		return E_FAIL;
	//최종블러
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectBlurX"), TEXT("Target_EffectBlurX"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectBlurY"), TEXT("Target_EffectBlurY"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectCombineBlur"), TEXT("Target_EffectCombineBlur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DiffuseBlurX"), TEXT("Target_DiffuseBlurX"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DiffuseBlurY"), TEXT("Target_DiffuseBlurY"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectDiffuseCombineBlur"), TEXT("Target_EffectDiffuseCombineBlur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_EffectRadialBlur"), TEXT("Target_EffectRadialBlur"))))
		return E_FAIL;


	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_VP.Width, m_VP.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_VP.Width, m_VP.Height, 0.f, 1.f));


	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECL::Elements, VTXTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect.hlsl"), VTXTEX_DECL::Elements, VTXTEX_DECL::iNumElements);
	if (nullptr == m_pEffectShader)
		return E_FAIL;

	m_pSSAOShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SSAO.hlsl"), VTXTEX_DECL::Elements, VTXTEX_DECL::iNumElements);
	if (nullptr == m_pSSAOShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
	m_pVIBufferEffect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBufferEffect)
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 50.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 50.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 50.0f, 300.0f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 150.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 150.0f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), 150.0f, 300.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_SSAO"), 50.0f, 400.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_BlurX"), 150.0f, 400.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_BlurY"), 50.0f, 500.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_CombineBlur"), 150.0f, 500.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_SSAOBlur"), 50.0f, 600.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_HDR"), 250.0f, 100.f, 100.f, 100.f)))
		return E_FAIL;
#endif // _DEBUG

	

	return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject ||
		RENDER_END <= eRenderGroup)
	{
		MSG_BOX("Failed to Add_RenderGroup");
		return E_FAIL;
	}

	m_RenderObjects[eRenderGroup].emplace_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_RenderTarget(const _tchar* pMRT_Tag, const _tchar* pRenderTargetTag, DXGI_FORMAT eFormat, _float4 vColor)
{
	_uint   iNumViewports = { 1 };
	D3D11_VIEWPORT   Viewport;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	_float4 vColor_Diffuse = { 1.f, 0.f, 1.f, 0.f };
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, pRenderTargetTag
		, (_uint)Viewport.Width, (_uint)Viewport.Height, eFormat, vColor)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(pMRT_Tag, pRenderTargetTag)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugGroup(CComponent* pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	m_DebugRender.emplace_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Draw_RenderObjects(HRESULT(*fp)())
{
	if (FAILED(Begin_DefaultRT()))
	{
		MSG_BOX("Failed to Begin_DefaultRT");
		return E_FAIL;
	}

	if (FAILED(Render_Priority()))
	{
		MSG_BOX("Failed to Render_Priority");
		return E_FAIL;
	}
	if (FAILED(Render_ShadowDepth()))
	{
		MSG_BOX("Failed to Render_ShadowDepth");
		return E_FAIL;
	}

	/*if (FAILED(Render_ShadowBlurX()))
	{
		MSG_BOX("Failed to Render_ShadowBlurX");
		return E_FAIL;
	}
	if (FAILED(Render_ShadowBlurY()))
	{
		MSG_BOX("Failed to Render_ShadowBlurY");
		return E_FAIL;
	}
	if (FAILED(Render_ShadowBlur()))
	{
		MSG_BOX("Failed to Render_ShadowBlur");
		return E_FAIL;
	}*/

	if (FAILED(Render_NonBlend()))
	{
		MSG_BOX("Failed to Render_NonBlend");
		return E_FAIL;
	}

	if (FAILED(Render_SSAO()))
	{
		MSG_BOX("Failed to Render_SSAO");
		return E_FAIL;
	}
	if (FAILED(Render_SSAOBlurX()))
	{
		MSG_BOX("Failed to Render_SSAOBlurX");
		return E_FAIL;
	}
	if (FAILED(Render_SSAOBlurY()))
	{
		MSG_BOX("Failed to Render_SSAOBlurY");
		return E_FAIL;
	}
	if (FAILED(Render_SSAOFinal()))
	{
		MSG_BOX("Failed to Render_SSAOFinalBlur");
		return E_FAIL;
	}

	if (FAILED(Render_Lights()))
	{
		MSG_BOX("Failed to Render_Lights");
		return E_FAIL;
	}

	if (FAILED(Render_Deferred()))
	{
		MSG_BOX("Failed to Render_Deferred");
		return E_FAIL;
	}

	if (FAILED(Render_ExportDeferred()))
	{
		MSG_BOX("Failed to Render_ExportBloom");
		return E_FAIL;
	}

	if (FAILED(Render_BlurX()))
	{
		MSG_BOX("Failed to Render_BlurX");
		return E_FAIL;
	}
	if (FAILED(Render_BlurY()))
	{
		MSG_BOX("Failed to Render_BlurY");
		return E_FAIL;
	}
	if (FAILED(Render_CombineBlur())) // 
	{
		MSG_BOX("Failed to Render_CombineBlur");
		return E_FAIL;
	}

	if (FAILED(Render_Bloom())) // 최종적으로 그려짐
	{
		MSG_BOX("Failed to Render_Bloom");
		return E_FAIL;
	}

	if (FAILED(Render_RadialBlur())) // 최종적으로 그려짐
	{
		MSG_BOX("Failed to Render_RadialBlur");
		return E_FAIL;
	}
	if (FAILED(Render_EffectBloom()))
	{
		MSG_BOX("Failed to Render_NonLights");
		return E_FAIL;
	}
		
	if (FAILED(Render_Effect_Particle()))
	{
		MSG_BOX("Failed to Render_Particle");
		return E_FAIL;
	}
	if (FAILED(Render_EffectNoBloom()))
	{
		MSG_BOX("Failed to Render_Blend");
		return E_FAIL;
	}
	if (FAILED(Render_Effect_Envrionment()))
	{
		MSG_BOX("Failed to Render_Blend");
		return E_FAIL;
	}
	/*if (FAILED(Render_Effect()))
	{
	   MSG_BOX("Failed to Render_Effect");
	   return E_FAIL;
	}
	*/

	if (FAILED(Render_World_UI()))
	{
		MSG_BOX("Failed to Render_World_UI");
		return E_FAIL;
	}

	if (FAILED(Render_UI()))
	{
		MSG_BOX("Failed to Render_UI");
		return E_FAIL;
	}

	if (FAILED(Render_TransparentWall()))
	{
		MSG_BOX("Failed to Render_TransparentWall");
		return E_FAIL;
	}
#ifdef _DEBUG
	if (true == m_isRenderDebug)
	{
		if (FAILED(Render_Debug()))
		{
			MSG_BOX("Failed to Render_Debug");
			return E_FAIL;
		}
	}

	if (true == m_isRenderTarget)
	{
		if (FAILED(Render_RenderTaget()))
		{
			MSG_BOX("Failed to Render_Debug");
			return E_FAIL;
		}
	}
#endif // _DEBUG

	if (FAILED(Render_CallBack()))
	{
		MSG_BOX("Failed to Render_CallBack");
		return E_FAIL;
	}

	if (FAILED(End_DefaultRT()))
	{
		MSG_BOX("Failed to Begin_DefaultRT");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Begin_DefaultRT()
{
	if (FAILED(m_pTarget_Manager->Begin_DefaultRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Begin_MRT(const _tchar* pMRT_Tag)
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(pMRT_Tag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::End_DefaultRT()
{
	if (FAILED(m_pTarget_Manager->End_DefaultRT()))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Default"), m_pShader, "g_Texture")))
		return E_FAIL;

	m_pShader->Begin(0);

	m_pVIBuffer->Render();

	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Default"), m_pSSAOShader, "g_Texture")))
		return E_FAIL;

	m_pSSAOShader->Begin(0);

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::End_MRT()
{
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	/* ShadowDepth */
	m_pTarget_Manager->Begin_MRT_LightDepth(m_pContext, TEXT("MRT_LightDepth"));

	D3D11_VIEWPORT VP;
	ZeroMemory(&VP, sizeof(D3D11_VIEWPORT));
	VP.TopLeftX = 0.0f;
	VP.TopLeftY = 0.0f;
	VP.Width = 12800.f;
	VP.Height = 7200.f;
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;

	m_pContext->RSSetViewports(1, &VP);

	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_ShadowDepth();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOWDEPTH].clear();

	m_pTarget_Manager->End_MRT();


	return S_OK;
}

HRESULT CRenderer::Render_ShadowBlurX()
{

	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_ShadowBlurX"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowDepth"), m_pShader, "g_BlurTexture")))
		return E_FAIL;


	if (FAILED(m_pShader->Begin(14)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_ShadowBlurY()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_ShadowBlurY"))))
		return E_FAIL;

	

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowBlurX"), m_pShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_ShadowBlur()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_ShadowBlur"))))
		return E_FAIL;

	

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowBlurX"), m_pShader, "g_BlurXTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowBlurY"), m_pShader, "g_BlurYTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowDepth"), m_pShader, "g_Texture")))
		return E_FAIL;
	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_Texture")))
	   return E_FAIL;*/

	if (FAILED(m_pShader->Begin(16)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_INSERT))
	{
		m_fSSAORadius += 0.0001f;
	}
	if (pGameInstance->Get_DIKeyState(DIK_DELETE))
	{
		m_fSSAORadius -= 0.0001f;
	}
	if (pGameInstance->Get_DIKeyState(DIK_HOME))
	{
		m_fSSAOBias += 0.0001f;
	}
	if (pGameInstance->Get_DIKeyState(DIK_END))
	{
		m_fSSAOBias -= 0.0001f;
	}
	//m_fSSAORadius = 0.000005f;

	if (FAILED(m_pSSAOShader->SetUp_RawValue("g_fRadius", &m_fSSAORadius, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_RawValue("g_fBias", &m_fSSAOBias, sizeof(_float))))
		return E_FAIL;
	Safe_Release(pGameInstance);

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pSSAOShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pSSAOShader, "g_DepthTexture")))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 ViewMatrixInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
		return E_FAIL;
	_float4x4 ProjMatrixInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
		return E_FAIL;

	_float4x4 matViewInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_matViewInv", &matViewInv)))
		return E_FAIL;
	_float4x4 matProjInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_matProjInv", &matProjInv)))
		return E_FAIL;



	_float4 vCamPosition = pPipeLine->Get_CameraPosition();
	if (FAILED(m_pSSAOShader->SetUp_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	if (FAILED(m_pSSAOShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;



	return S_OK;
}

HRESULT CRenderer::Render_SSAOBlurX()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_SSAOBlurX"))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/*m_bSSAOBlur = true;
	if (FAILED(m_pSSAOShader->SetUp_RawValue("g_bSSAO", &m_bSSAOBlur, sizeof(_bool))))
	   return E_FAIL;*/


	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pSSAOShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSAOBlurY()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_SSAOBlurY"))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/*m_bSSAOBlur = true;
	if (FAILED(m_pSSAOShader->SetUp_RawValue("g_bSSAO", &m_bSSAOBlur, sizeof(_bool))))
	   return E_FAIL;*/

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAOBlurX"), m_pSSAOShader, "g_BlurXTexture")))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_SSAOFinal()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_SSAOBlur"))))
		return E_FAIL;


	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAOBlurX"), m_pSSAOShader, "g_BlurXTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAOBlurY"), m_pSSAOShader, "g_BlurYTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pSSAOShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ExportDeferred()
{
	// 최종 장면을 추출
	// 최종 장면에서 밝은 부분만 추출 -> 블룸
	// 추출한걸 X,Y 블러 적용
	// 최종 + X + Y
	// 블룸 적용 -> 최종화면적용
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_ExportHDR"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_CombineBlur"), m_pShader, "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(12)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_BlurX()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_BlurX"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_bSSAOBlur = true;
	if (FAILED(m_pShader->SetUp_RawValue("g_bSSAO", &m_bSSAOBlur, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ExportFinal"), m_pShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(6)))
		return E_FAIL;
	/*if (FAILED(m_pShader->Begin(10)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;



	return S_OK;
}

HRESULT CRenderer::Render_BlurY()
{


	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_BlurY"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_bSSAOBlur = true;
	if (FAILED(m_pShader->SetUp_RawValue("g_bSSAO", &m_bSSAOBlur, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ExportFinal"), m_pShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(7)))
		return E_FAIL;
	/*if (FAILED(m_pShader->Begin(11)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;



	return S_OK;
}

HRESULT CRenderer::Render_CombineBlur()
{

	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_HDR"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_BlurX"), m_pShader, "g_BlurXTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_BlurY"), m_pShader, "g_BlurYTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ExportFinal"), m_pShader, "g_Texture")))
		return E_FAIL;
	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pShader, "g_SSAOTexture")))
	   return E_FAIL;*/

	   /*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAOBlur"), m_pShader, "g_SSAOTexture")))
		  return E_FAIL;*/

	if (FAILED(m_pShader->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_Bloom()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_RadialBlur"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_RawValue("g_bRadialBlur", &m_bRadialBlur, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_HDR"), m_pShader, "g_HDRTexture"))) // 블룸 + 블러 
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ExportFinal"), m_pShader, "g_BloomTextrue"))) // 블룸 -> 밝은 부분만 추출
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_CombineBlur"), m_pShader, "g_FinalTexture"))) // 디퍼드 최종 텍스처
		return E_FAIL;


	if (FAILED(m_pShader->Begin(13)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* 백버퍼를 빼고 */
	/* Diffuse + Normal */

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_GameObject"))))
		return E_FAIL;

	m_pContext->RSSetViewports(1, &m_VP);
	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_EffectBloom()
{

	m_RenderObjects[RENDER_EffectBloom].sort([](CGameObject* pDest, CGameObject* pSrc)->bool {
		return dynamic_cast<CMasterEffect*>(pDest)->Get_Order() > dynamic_cast<CMasterEffect*>(pSrc)->Get_Order();
		});

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_Effect"))))
		return E_FAIL;

	

	for (auto& pGameObject : m_RenderObjects[RENDER_EffectBloom])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_EffectBloom].clear();

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	////디퓨즈
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectColor"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuse"), m_pEffectShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDistortion"), m_pEffectShader, "g_DiffuseDistortion")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 밝은 것만 추출작업
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBloom"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_RawValue("g_fBrigthRatio", &m_fBrigthRatio, sizeof(_float))))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (pGameInstance->Get_DIKeyDown(DIK_UP))
	{
		m_fBloomPower += 0.1f;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_DOWN))
	{
		m_fBloomPower -= 0.1f;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_RIGHT))
	{
		m_fBlurWeight += 0.05f;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_LEFT))
	{
		m_fBlurWeight -= 0.05f;
	}

	if (FAILED(m_pEffectShader->SetUp_RawValue("g_BlurWeight", &m_fBlurWeight, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_RawValue("g_fBloomPower", &m_fBloomPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_FinalTexture")))
		return E_FAIL;


	if (FAILED(m_pEffectShader->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 추출한 것을 블러처리
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBlurX"))))
		return E_FAIL;


	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(2)))
		return E_FAIL;

	/*if (FAILED(m_pEffectShader->Begin(10)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	// Y
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBlurY"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
	   return E_FAIL;*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurX"), m_pEffectShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(3)))
		return E_FAIL;
	/*if (FAILED(m_pEffectShader->Begin(10)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 블러 적용
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectCombineBlur"))))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurX"), m_pEffectShader, "g_BlurXTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurY"), m_pEffectShader, "g_BlurYTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_Texture")))
		return E_FAIL;


	if (FAILED(m_pEffectShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 블룸적용
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurY"), m_pEffectShader, "g_HDRTexture"))) // 블룸 + 블러 
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectCombineBlur"), m_pEffectShader, "g_HDRTexture"))) // 블룸 + 블러 
	    return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BloomTextrue"))) // 블룸 -> 밝은 부분만 추출
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_FinalTexture"))) // 디퍼드 최종 텍스처
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_EffectNoBloom()
{
	m_RenderObjects[RENDER_EffectNoBloom].sort([](CGameObject* pDest, CGameObject* pSrc)->bool {
		return dynamic_cast<CMasterEffect*>(pDest)->Get_Order() > dynamic_cast<CMasterEffect*>(pSrc)->Get_Order();
		});

	for (auto& pGameObject : m_RenderObjects[RENDER_EffectNoBloom])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_EffectNoBloom].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Particle()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_Effect"))))
		return E_FAIL;


	for (auto& pGameObject : m_RenderObjects[RENDER_Effect_Particle])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_Effect_Particle].clear();

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	////디퓨즈
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectColor"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuse"), m_pEffectShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	// 추가 블러
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_DiffuseBlurX"))))
		return E_FAIL;


	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_BlurTexture")))
		return E_FAIL;

	/*if (FAILED(m_pEffectShader->Begin(2)))
		return E_FAIL;*/
	if (FAILED(m_pEffectShader->Begin(5)))
		return E_FAIL;

	/*if (FAILED(m_pEffectShader->Begin(10)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	// Y
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_DiffuseBlurY"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
	   return E_FAIL;*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_DiffuseBlurX"), m_pEffectShader, "g_BlurTexture")))
		return E_FAIL;

	/*if (FAILED(m_pEffectShader->Begin(3)))
		return E_FAIL;*/

	if (FAILED(m_pEffectShader->Begin(6)))
		return E_FAIL;


	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 블러 적용
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectDiffuseCombineBlur"))))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_DiffuseBlurX"), m_pEffectShader, "g_BlurXTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_DiffuseBlurY"), m_pEffectShader, "g_BlurYTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_Texture")))
		return E_FAIL;


	if (FAILED(m_pEffectShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	// 추가블러 끝

	// 밝은 것만 추출작업
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBloom"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	m_fParticle_BrightRatio = 0.80f;
	if (FAILED(m_pEffectShader->SetUp_RawValue("g_fBrigthRatio", &m_fParticle_BrightRatio, sizeof(_float))))
		return E_FAIL;

	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_FinalTexture")))
	   return E_FAIL;*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuseCombineBlur"), m_pEffectShader, "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 추출한 것을 블러처리
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBlurX"))))
		return E_FAIL;


	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(2)))
		return E_FAIL;

	/*if (FAILED(m_pEffectShader->Begin(10)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;
	// Y
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBlurY"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
	   return E_FAIL;*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurX"), m_pEffectShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(3)))
		return E_FAIL;
	/*if (FAILED(m_pEffectShader->Begin(10)))
	   return E_FAIL;*/

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 블러 적용
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectCombineBlur"))))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurX"), m_pEffectShader, "g_BlurXTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurY"), m_pEffectShader, "g_BlurYTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_Texture")))
		return E_FAIL;


	if (FAILED(m_pEffectShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	// 블룸적용
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectCombineBlur"), m_pEffectShader, "g_HDRTexture"))) // 블룸 + 블러 
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BloomTextrue"))) // 블룸 -> 밝은 부분만 추출
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuseCombineBlur"), m_pEffectShader, "g_FinalTexture"))) // 디퍼드 최종 텍스처
		return E_FAIL;

	if (FAILED(m_pEffectShader->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferEffect->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Envrionment()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_EFFECT_ENVIRONMENT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_EFFECT_ENVIRONMENT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RENDER_UI].sort([](CGameObject* pDest, CGameObject* pSrc)->bool {
		return dynamic_cast<CUI*>(pDest)->Get_UI_Layer() < dynamic_cast<CUI*>(pSrc)->Get_UI_Layer();

		});

	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;


	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_RadialBlur"), m_pShader, "g_RadialBlurTexture"))) // 블룸 + 블러 
		return E_FAIL;

	if (FAILED(m_pShader->Begin(17)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CRenderer::Render_CallBack()
{
	if (nullptr != m_Func)
	{
		if (FAILED(m_Func()))
			return S_OK;
	}

	return S_OK;
}

HRESULT CRenderer::Render_TransparentWall()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_TRANSPARENTWALL])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_TRANSPARENTWALL].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/*if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_Effect"))))
	   return E_FAIL;*/

	m_RenderObjects[RENDER_EFFECT].sort([](CGameObject* pDest, CGameObject* pSrc)->bool {
		return dynamic_cast<CMasterEffect*>(pDest)->Get_Order() > dynamic_cast<CMasterEffect*>(pSrc)->Get_Order();
		});

	for (auto& pGameObject : m_RenderObjects[RENDER_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_EFFECT].clear();

	/*if (FAILED(m_pTarget_Manager->End_MRT()))
	   return E_FAIL;*/
	   //디퓨즈
	   /*if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectColor"))))
		  return E_FAIL;*/

		  /*if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			 return E_FAIL;
		  if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			 return E_FAIL;
		  if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			 return E_FAIL;

		  if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuse"), m_pEffectShader, "g_DiffuseTexture")))
			 return E_FAIL;

		  if (FAILED(m_pEffectShader->Begin(1)))
			 return E_FAIL;

		  if (FAILED(m_pVIBuffer->Render()))
			 return E_FAIL;*/

			 /*if (FAILED(m_pTarget_Manager->End_MRT()))
				return E_FAIL;*/

				//// 밝은 것만 추출작업
				//if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBloom"))))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				//   return E_FAIL;

				///*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuse"), m_pEffectShader, "g_FinalTexture")))
				//   return E_FAIL;*/
				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_FinalTexture")))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->Begin(7)))
				//   return E_FAIL;

				//if (FAILED(m_pVIBuffer->Render()))
				//   return E_FAIL;

				//if (FAILED(m_pTarget_Manager->End_MRT()))
				//   return E_FAIL;

				//// 추출한 것을 블러처리
				//if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBlurX"))))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				//   return E_FAIL;

				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->Begin(2)))
				//   return E_FAIL;
				///*if (FAILED(m_pEffectShader->Begin(10)))
				//   return E_FAIL;*/

				//if (FAILED(m_pVIBuffer->Render()))
				//   return E_FAIL;

				//if (FAILED(m_pTarget_Manager->End_MRT()))
				//   return E_FAIL;
				//// Y
				//if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectBlurY"))))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				//   return E_FAIL;

				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BlurTexture")))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->Begin(3)))
				//   return E_FAIL;
				///*if (FAILED(m_pEffectShader->Begin(10)))
				//   return E_FAIL;*/

				//if (FAILED(m_pVIBuffer->Render()))
				//   return E_FAIL;

				//if (FAILED(m_pTarget_Manager->End_MRT()))
				//   return E_FAIL;

				//// 블러 적용
				//if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_EffectCombineBlur"))))
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				//   return E_FAIL;


				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurX"), m_pEffectShader, "g_BlurXTexture")))
				//   return E_FAIL;
				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBlurY"), m_pEffectShader, "g_BlurYTexture")))
				//   return E_FAIL;
				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_Texture")))
				//   return E_FAIL;


				//if (FAILED(m_pEffectShader->Begin(4)))
				//   return E_FAIL;

				//if (FAILED(m_pVIBuffer->Render()))
				//   return E_FAIL;
				//if (FAILED(m_pTarget_Manager->End_MRT()))
				//   return E_FAIL;

				//// 블룸적용
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				//   return E_FAIL;
				//if (FAILED(m_pEffectShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				//   return E_FAIL;


				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectCombineBlur"), m_pEffectShader, "g_HDRTexture"))) // 블룸 + 블러 
				//   return E_FAIL;
				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectBloom"), m_pEffectShader, "g_BloomTextrue"))) // 블룸 -> 밝은 부분만 추출
				//   return E_FAIL;
				//if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectColor"), m_pEffectShader, "g_FinalTexture"))) // 디퍼드 최종 텍스처
				//   return E_FAIL;

				//if (FAILED(m_pEffectShader->Begin(8)))
				//   return E_FAIL;

				//if (FAILED(m_pVIBuffer->Render()))
				//   return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_World_UI()
{
	m_RenderObjects[RENDER_WORLD_UI].sort([](CGameObject* pDest, CGameObject* pSrc)->bool {
		return dynamic_cast<CUI*>(pDest)->Get_UI_Layer() < dynamic_cast<CUI*>(pSrc)->Get_UI_Layer();

		});

	for (auto& pGameObject : m_RenderObjects[RENDER_WORLD_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);

	}

	m_RenderObjects[RENDER_WORLD_UI].clear();


	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pComponent : m_DebugRender)
	{
		if (nullptr != pComponent)
		{
			pComponent->Render();
			Safe_Release(pComponent);
		}
	}

	m_DebugRender.clear();

	return S_OK;
}

HRESULT CRenderer::Render_RenderTaget()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_GameObject"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_SSAO"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurX"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurY"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_CombineBlur"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_SSAO"), m_pSSAOShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_ExportHDR"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_HDR"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 ViewMatrixInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
		return E_FAIL;
	_float4x4 ProjMatrixInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
		return E_FAIL;

	_float4 vCamPosition = pPipeLine->Get_CameraPosition();
	if (FAILED(m_pShader->SetUp_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAOBlur"), m_pShader, "g_SSAOFinalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse_Cha"), m_pShader, "g_DiffuseTexture_Cha")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_TAB))
	{
		if (m_bSSAOSwitch == false)
			m_bSSAOSwitch = true;
		else if (m_bSSAOSwitch == true)
			m_bSSAOSwitch = false;
	}
	if (FAILED(m_pShader->SetUp_RawValue("g_bSSAOSwitch", &m_bSSAOSwitch, sizeof(_bool))))
		return E_FAIL;

	
	Safe_Release(pGameInstance);


	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);

	m_pTarget_Manager->End_MRT();

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_CombineBlur"))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse_Cha"), m_pShader, "g_DiffuseTexture_Cha")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	
	   /*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowBlurX"), m_pShader, "g_ShadowDepthTexture")))
		  return E_FAIL;*/

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowDepth"), m_pShader, "g_ShadowDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Emissive"), m_pShader, "g_EmissiveTexture")))
		return E_FAIL;

	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_EffectDiffuse"), m_pEffectShader, "g_DiffuseTexture")))
	   return E_FAIL;*/


	   // 임시로 만들어 두겠음 -> 이거 나중에 플레이어에서 불변수 넘겨주는 방식으로 합시다.
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	///Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->SetUp_RawValue("g_bInvert", &m_bInvert, sizeof(_bool))))
		return E_FAIL;
	
	static _float fGrayRatio = 1.f;

	if (FAILED(m_pShader->SetUp_RawValue("g_fGrayRatio", &fGrayRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetUp_RawValue("g_bBackLight", &m_bBakcLight, sizeof(_bool))))
		return E_FAIL;

	if (m_bGrayScale)
	{
		fGrayRatio += 0.03f;
		if (fGrayRatio > 1.f)
			fGrayRatio = 1.f;
	}
	else
	{
		fGrayRatio -= 0.03f;
		if (fGrayRatio < 0.f)
			fGrayRatio = 0.f;
	}

	/*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowBlur"), m_pShader, "g_DepthTexture")))
	   return E_FAIL;*/
	   /*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowBlur"), m_pShader, "g_ShadowDepthTexture")))
		  return E_FAIL;*/
		  /*if (FAILED(m_pTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pShader, "g_SSAOTexture")))
			 return E_FAIL;*/


	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);
	
	_float4x4 matViewInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->SetUp_Matrix("g_matViewInv", &matViewInv)))
		return E_FAIL;
	_float4x4 matProjInv = pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->SetUp_Matrix("g_matProjInv", &matProjInv)))
		return E_FAIL;

	Safe_Release(pPipeLine);

	switch (pGameInstance->Get_CurLevelIdx())
	{
	case 5:
		Shadow_Village_Setting();
		break;
	case 6:
		Shadow_House_Setting();
		break;
	case 7:
		Shadow_Train_Setting();
		break;
	case 8:
		Shadow_Final_Setting();
		break;
	}

	//Safe_Release(pGameInstance);

	m_pShader->Begin(3);

	m_pVIBuffer->Render();
	m_pTarget_Manager->End_MRT();


	return S_OK;
}

void CRenderer::Shadow_Village_Setting()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), m_iPlayerIdx, TEXT("Com_Transform")));

	_vector   vLightEye = XMVectorSet(530.f, 50.f, 292.f, 1.f);
	_vector   vLightAt = { 585.f, 0.f, 278.f, 1.f };
	_vector   vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	if (m_bVillage_Battle == true)
	{
		vLightEye = XMVectorSet(424.f, 50.f, 311.f, 1.f);
		vLightAt = XMVectorSet(377.f, 0.f, 311.f, 1.f);
	}			

	if (pPlayerTransformCom != nullptr)
	{
		_vector	  vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

		vLightEye = vPlayerPos + XMVectorSet(-15.f, 40.f, -15.f, 1.f);
		vLightAt = vPlayerPos;
	}


	/*if (pPlayerTransformCom != nullptr)
	{
		_vector   vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

		vLightEye = XMVectorSet(90.f, 50.f, 90.f, 1.f);
		vLightAt = XMVectorSet(128.5f, 0.f, 128.5f, 1.f);

	}*/

	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	m_pShader->SetUp_Matrix("g_matLightView", &FloatLightViewMatrix);
		
	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), _float(1280.f) / _float(720.f), 0.2f, 400.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	m_pShader->SetUp_Matrix("g_matProj", &FloatLightProjMatrix);
		

	Safe_Release(pGameInstance);
}

void CRenderer::Shadow_House_Setting()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), m_iPlayerIdx, TEXT("Com_Transform")));

	_vector   vLightEye = XMVectorSet(530.f, 50.f, 292.f, 1.f);
	_vector   vLightAt = { 585.f, 0.f, 278.f, 1.f };
	_vector   vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	if (pPlayerTransformCom != nullptr)
	{
		_vector	  vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

		vLightEye = vPlayerPos + XMVectorSet(-25.f, 50.f, -25.f, 1.f);
		vLightAt = vPlayerPos;
	}

	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	m_pShader->SetUp_Matrix("g_matLightView", &FloatLightViewMatrix);

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), _float(1280.f) / _float(720.f), 0.2f, 400.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	m_pShader->SetUp_Matrix("g_matProj", &FloatLightProjMatrix);


	Safe_Release(pGameInstance);
}

void CRenderer::Shadow_Train_Setting()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), m_iPlayerIdx, TEXT("Com_Transform")));

	_vector   vLightEye = XMVectorSet(530.f, 50.f, 292.f, 1.f);
	_vector   vLightAt = { 585.f, 0.f, 278.f, 1.f };
	_vector   vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	if (pPlayerTransformCom != nullptr)
	{
		_vector	  vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

		vLightEye = vPlayerPos + XMVectorSet(-20.f, 40.f, -20.f, 1.f);
		vLightAt = vPlayerPos;
	}

	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	m_pShader->SetUp_Matrix("g_matLightView", &FloatLightViewMatrix);

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), _float(1280.f) / _float(720.f), 0.2f, 400.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	m_pShader->SetUp_Matrix("g_matProj", &FloatLightProjMatrix);

	Safe_Release(pGameInstance);

}

void CRenderer::Shadow_Final_Setting()
{
	_vector   vLightEye = XMVectorSet(90.f, 50.f, 90.f, 1.f);
	_vector   vLightAt = { 128.5f, 0.f, 128.5f, 1.f };
	_vector   vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	m_pShader->SetUp_Matrix("g_matLightView", &FloatLightViewMatrix);

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(20.f), _float(1280.f) / _float(720.f), 0.2f, 400.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	m_pShader->SetUp_Matrix("g_matProj", &FloatLightProjMatrix);
		
}


CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pEffectShader);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBufferEffect);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);


	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);

		RenderObjects.clear();
	}

#ifdef _DEBUG
	for (auto& pGameObject : m_DebugRender)
		Safe_Release(pGameObject);

	m_DebugRender.clear();
#endif // _DEBUG

}