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

class COption final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_X_Reverse = { false };
		_bool    m_Is_Y_Reverse = { false };

		_uint    m_Type = { 0 };
		_uint    m_Menu_Type = { 0 };
		_uint    m_Cloud_Type = { 0 };


	}UIDESC;


private:
	COption(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COption(const COption& rhs);
	virtual ~COption() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void Set_UI();
	void Cloud_Control(_double dTimeDelta);

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
	_float                  m_Alpha = { 0.f };
	_float                  m_Alpha_Dir = { 1.f };
	_bool                   m_Is_Reverse = { false };
	_bool                   m_Is_CutScene = false;
	_float                  m_Origin_X = {};
	_float                  m_Origin_Y = {};
	_float                  m_Size_Param = { 1.f };
	
	_float                  m_Origin_PosX = {};
	_float                  m_Origin_PosY = {};
	_float                  m_PosX_Dir = { 1.f };
	_float                  m_PosY_Dir = { 1.f };


private:
	_bool                   m_Is_Select = { 0 };
	_int                   m_Select_Num = { 0 };


	vector<wstring>         m_szCameraMenu = { };
	vector<wstring>         m_szGraphicMenu = { };
	vector<wstring>         m_szSoundMenu = { };

	vector<wstring>         m_szButtonMenu = { };


	
private:
	_float                  m_Time_X = {};
	_float                  m_UV_Speed_X = {};
	_float                  m_Time_Y = {};
	_float                  m_UV_Speed_Y = {};


	_bool                   m_Is_Font_Render = { false };
	_bool                   m_Is_Reset = { false };


private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static COption* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END