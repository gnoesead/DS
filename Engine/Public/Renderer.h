#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWDEPTH, RENDER_SSAO, RENDER_NONBLEND, RENDER_EffectBloom, RENDER_EffectNoBloom, RENDER_Effect_Particle, RENDER_EFFECT_ENVIRONMENT ,RENDER_UI, RENDER_EFFECT,RENDER_TRANSPARENTWALL, RENDER_WORLD_UI,RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	HRESULT RegistCallBack(T * pClassType, HRESULT(T:: * fp)());

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT	Add_RenderTarget(const _tchar * pMRT_Tag, const _tchar * pRenderTargetTag, enum DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT Draw_RenderObjects(HRESULT(*fp)() = nullptr);
	HRESULT Begin_DefaultRT();
	HRESULT Begin_MRT(const _tchar * pMRT_Tag);
	HRESULT End_DefaultRT();
	HRESULT End_MRT();

#ifdef _DEBUG
public:

	void OnOff_RenderDebug() { m_isRenderDebug = !m_isRenderDebug; }
	void OnOff_RenderTarget() { m_isRenderTarget = !m_isRenderTarget; }

public:
	HRESULT Add_DebugGroup(CComponent * pComponent);
#endif // _DEBUG
public:
	void Set_RadialBlur() { m_bRadialBlur = !m_bRadialBlur; }
	void Set_RadialBlur_On(_bool on) { m_bRadialBlur = on; }
	void Set_GrayScale() { m_bGrayScale = !m_bGrayScale; }
	void Set_GrayScale_On(_bool on) { m_bGrayScale = on; }
	void Set_Invert() { m_bInvert = !m_bInvert; }
	void Set_SSAO() { m_bSSAOSwitch = !m_bSSAOSwitch; }
	void Set_SSAO(_bool Is) { m_bSSAOSwitch = Is; }
	void Set_BloomRatio(_float fBloomRatio = 0.90f) {
		m_fBrigthRatio = fBloomRatio;
	}
	void Set_Village_Battle(_bool Is) { m_bVillage_Battle = Is; }
	void Set_PlayerIndex(_uint iPlayerIndex) { m_iPlayerIdx = iPlayerIndex; }
	void Set_BackLight() { m_bBakcLight = !m_bBakcLight; }
private:
	list<class CGameObject*>	m_RenderObjects[RENDER_END];
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };

private:
	class CShader* m_pShader = { nullptr };
	class CShader* m_pEffectShader = { nullptr };
	class CShader* m_pSSAOShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferEffect = { nullptr };
	
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
	HRESULT Render_Effect_Particle();
	HRESULT Render_Effect_Envrionment();
	HRESULT Render_Effect();
	HRESULT Render_World_UI();
	HRESULT Render_UI();
	HRESULT Render_RadialBlur();


	HRESULT Render_CallBack();

	HRESULT Render_TransparentWall();


#ifdef _DEBUG
private:
	HRESULT Render_Debug();
	HRESULT Render_RenderTaget();
#endif // _DEBUG

private:
	HRESULT Render_Lights();
	HRESULT Render_Deferred();

private:
	void	Shadow_Village_Setting();
	void	Shadow_House_Setting();
	void	Shadow_Train_Setting();
	void	Shadow_Final_Setting();


private:
	_bool					m_bSSAOBlur = { false };
	_bool					m_bSSAOSwitch = { true };
	_bool					m_bInvert = { false };
	_bool					m_bRadialBlur = { false };
	_bool					m_bGrayScale = { false };
	_bool					m_bVillage_Battle = { false };
	_bool					m_bBakcLight = { false };

private:
	D3D11_VIEWPORT			m_VP; // Shadow

	_float					m_fSSAOBias = { 0.0003f };
	_float					m_fSSAORadius = { 0.000005f };
	//_float					m_fSSAORadius = { 0.005f };
	_float					m_fBrigthRatio = { 0.90f };
	_float					m_fParticle_BrightRatio = { 0.8f };
	_float					m_fBloomPower = { 1.5f };
	_float					m_fBlurWeight = { 1.f };

private:
	_uint					m_iPlayerIdx = { 0 };
public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

template<typename T>
inline HRESULT CRenderer::RegistCallBack(T* pClassType, HRESULT(T::* fp)())
{
	if (nullptr != fp)
	{
		m_Func = bind(fp, pClassType);
	}

	return S_OK;
}
