#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWDEPTH, RENDER_SSAO, RENDER_NONBLEND, RENDER_EffectBloom, RENDER_EffectNoBloom, RENDER_UI, RENDER_EFFECT, RENDER_WORLD_UI,RENDER_END };
private:
	CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	_bool	Get_GrayScale() {
		return m_bGrayScale;
	}

public:
	template<typename T>
	HRESULT RegistCallBack(T* pClassType, HRESULT(T::*fp)());

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT	Add_RenderTarget(const _tchar* pMRT_Tag, const _tchar* pRenderTargetTag, enum DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT Draw_RenderObjects(HRESULT (*fp)() = nullptr);
	HRESULT Begin_DefaultRT();
	HRESULT Begin_MRT(const _tchar* pMRT_Tag);
	HRESULT End_DefaultRT();
	HRESULT End_MRT();

#ifdef _DEBUG
public:
		
	void OnOff_RenderDebug()	{ m_isRenderDebug = !m_isRenderDebug; }
	void OnOff_RenderTarget()	{ m_isRenderTarget = !m_isRenderTarget; }

public:
	HRESULT Add_DebugGroup(CComponent* pComponent);
#endif // _DEBUG
public:
	void Set_RadialBlur() { m_bRadialBlur = !m_bRadialBlur; }
	void Set_GrayScale() { m_bGrayScale = !m_bGrayScale; }
	void Set_Invert() { m_bInvert = !m_bInvert; }
	void Set_SSAO() { m_bSSAOSwitch = !m_bSSAOSwitch; }
	void Set_SSAO(_bool Is) { m_bSSAOSwitch = Is; }

private:
	list<class CGameObject*>	m_RenderObjects[RENDER_END];
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };

private:
	class CShader*			m_pShader = { nullptr };
	class CShader*			m_pEffectShader = { nullptr };
	class CShader*			m_pSSAOShader = { nullptr };
	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };
	_float4x4				m_WorldMatrix;
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;
	
	function<HRESULT()>		m_Func;


#ifdef _DEBUG
private:
	list<CComponent*>		m_DebugRender;

	_bool					m_isRenderDebug = { false };
	_bool					m_isRenderTarget = { false };
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowDepth(); // Shadow
	HRESULT Render_ShadowBlurX();
	HRESULT Render_ShadowBlurY();
	HRESULT Render_ShadowBlur();
	HRESULT Render_SSAO();
	HRESULT Render_SSAOBlurX();
	HRESULT Render_SSAOBlurY();
	HRESULT Render_SSAOFinal();
	HRESULT Render_ExportDeferred();
	HRESULT Render_BlurX();
	HRESULT Render_BlurY();
	HRESULT Render_CombineBlur();
	HRESULT Render_Bloom();
	HRESULT Render_NonBlend();
	HRESULT Render_EffectBloom();
	HRESULT Render_EffectNoBloom();
	HRESULT Render_Effect(); // Effect
	HRESULT Render_World_UI();
	HRESULT Render_UI();
	HRESULT Render_RadialBlur();

	HRESULT Render_CallBack();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
	HRESULT Render_RenderTaget();
#endif // _DEBUG

private:
	HRESULT Render_Lights();
	HRESULT Render_Deferred();
	
		
private:
	_bool					m_bSSAOBlur = { false };
	_bool					m_bSSAOSwitch = { true };
	_bool					m_bInvert = { false };
	_bool					m_bRadialBlur = { false };
	_bool					m_bGrayScale = { false };
private:
	D3D11_VIEWPORT			m_VP; // Shadow

	_float					m_fSSAOBias = { 0.0003f };
	//_float					m_fSSAORadius = { 0.000005f };
	_float					m_fSSAORadius = { 0.005f };
public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

template<typename T>
inline HRESULT CRenderer::RegistCallBack(T* pClassType, HRESULT(T::*fp)())
{
	if (nullptr != fp)
	{
		m_Func = bind(fp, pClassType);
	}

	return S_OK;
}
