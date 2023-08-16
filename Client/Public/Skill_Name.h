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

class CSkill_Name final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;

private:
	CSkill_Name(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Name(const CSkill_Name& rhs);
	virtual ~CSkill_Name() = default;

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
	void Get_Mouse_Pos();
	void Change_Info(_double TimeDelta);
	void Tool_Funtion(_double TimeDelta);
	void Get_Player_Info(_double TimeDelta);
	
private:
	_double m_Delay_TimeAcc = { 0 };
	_double m_Delay_Max_Time = { 3 };
	_bool m_Delay_Down = { false };

private:
	

private:
	UIDESC                  m_UI_Desc = {};

	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;	

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 1.f };

	vector<wstring>			m_szTanjiro_Skill = {};
	vector<wstring>			m_szZenitsu_Skill = {};
	vector<wstring>			m_szRengoku_Skill = {};
	vector<wstring>			m_szAkaza_Skill = {};

	_uint m_Player_Index = { 0 };
	_uint m_Player_Skill_Index = { 0 };

	_uint m_Boss_Index = { 0 };
	_uint m_Boss_Skill_Index = { 0 };


	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_bool m_Pt_In = { false };

	_float m_Size_Param = {};

	_float m_Origin_Size_Param = {};


	_float m_Origin_X = {};
	_float m_Origin_Y = {};

	_bool m_Is_Fix = { true };

	_bool m_Is_Font_Render = { false };



private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CSkill_Name* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END