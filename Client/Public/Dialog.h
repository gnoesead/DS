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

class CDialog final : public CUI
{
	enum NAME_TYPE { TANJIRO, MOTHER, GIRL, NAME_END };


public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CDialog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialog(const CDialog& rhs);
	virtual ~CDialog() = default;

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
	wstring					m_szName[NAME_END] = {};

	vector<wstring>			m_szMain_1 = {};
	vector<wstring>			m_szMain_2 = {};
	vector<wstring>			m_szMain_3 = {};

	vector<wstring>			m_szSub_1 = {};
	vector<wstring>			m_szSub_2 = {};
	vector<wstring>			m_szSub_3 = {};
	vector<wstring>			m_szSub_4 = {};
	vector<wstring>			m_szSub_5 = {};

	vector<wstring>			m_szTalk_1 = {};
	vector<wstring>			m_szTalk_2 = {};
	vector<wstring>			m_szTalk_3 = {};

	vector<wstring>			m_szVillageSmell_1 = {};
	vector<wstring>			m_szVillageSmell_2 = {};
	vector<wstring>			m_szVillageSmell_3 = {};

	vector<wstring>			m_szHouseSmell_1 = {};
	vector<wstring>			m_szHouseSmell_2 = {};
	vector<wstring>			m_szHouseSmell_3 = {};


	_uint                   m_Dialog_Type = { 99 };

	_uint                   m_Cur_Num = { 0 };

	_int                    m_Name_Type = { 1 };

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

	_float                  m_Alpha = { 0.f };

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
	static CDialog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END