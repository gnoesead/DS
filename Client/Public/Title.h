#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CTitle final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitle(const CTitle& rhs);
	virtual ~CTitle() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void Set_UI();


private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	
private:
	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};
	
	UIDESC                  m_UI_Desc = {};
	_float                  m_Alpha = { 1.f };
	_float                  m_Alpha_Dir = { 1.f };
	_bool                   m_Is_Reverse = { false };
	_bool                   m_Is_CutScene = false;
	_float                  m_Origin_X = {};
	_float                  m_Origin_Y = {};
	_float                  m_Size_Param = { 1.f };
	


private:
	_bool                   m_Select = { 0 };
	_tchar                  m_szGameStart[MAX_PATH] = { TEXT("게임 시작") };
	_tchar                  m_szOption[MAX_PATH] = { TEXT("옵션") };

private:
	_float                  m_Time_X = {};
	_float                  m_UV_Speed_X = {};
	_float                  m_Time_Y = {};
	_float                  m_UV_Speed_Y = {};

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CTitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END