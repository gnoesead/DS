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

class CTiming_UI final : public CUI
{
	
public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
		
	}UIDESC;


private:
	CTiming_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTiming_UI(const CTiming_UI& rhs);
	virtual ~CTiming_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private: /* 내가 사용하고하는 컴포넌트의 기능을 검색없이 바로 사용할 수 있다. */	
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

public:
	void Set_UI();
	void Get_Player_Info(_double TimeDelta);
	void Fade_In(_double TimeDelta);
	void Fade_Out(_double TimeDelta);


private:
	_int                   m_Cur_Num = { 2 };
	_bool                  m_Is_Mask = { true };
	_bool                  m_Is_Black_Mask = { true };

	_bool                  m_Is_Selected = { false };

	_float                 m_Sprite = { 0 };
	_float                 m_Sprite_Max = { 0 };
	_float                 m_Speed = { 0.f };


private:
	_bool m_Is_In = { false };
	_bool m_Is_Out = { false };
	_bool m_Is_Font_Render = { false };

	_float m_TimeAcc = { 0.f };

private:
	UIDESC                  m_UI_Desc = {};

	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 1.f };

	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_bool m_Pt_In = { false };

	_float m_Size_Param = {};

	_float m_Origin_Size_Param = {};


	_float m_Origin_X = {};
	_float m_Origin_Y = {};

	_float m_Origin_PosX = {};
	_float m_Origin_PosY = {};
	_float m_Start_PosX = {};
	_float m_Start_PosY = {};

	_bool m_Is_Fix = { true };


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CTiming_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END