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

class CMission final : public CUI
{
	
public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CMission(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMission(const CMission& rhs);
	virtual ~CMission() = default;

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
	vector<wstring>			m_szMain = {};
	vector<wstring>			m_szMain_Sub = {};
	vector<wstring>		    m_szMain_Sub_Check = {};
	vector<wstring>			m_szMain_Sub_Num_Total = {};
	vector<_uint>           m_Main_Sub_Num_Total = { 0 };
	_tchar                  m_szMain_Sub_Num[MAX_PATH] = {};
	_uint                   m_Main_Sub_Num = { 0 };

	vector<wstring>			m_szSub = {};
	vector<wstring>			m_szSub_Check = {};
	vector<wstring>			m_szSub_Num_Total = {};
	vector<_uint>           m_Sub_Num_Total = { 0 };
	_tchar                  m_szSub_Num[MAX_PATH] = {};
	_uint                   m_Sub_Num = { 0 };

	_uint    m_Main_Type = { 0 };
	_uint    m_Sub_Type = { 0 };
	_bool    m_Is_Dialog_On = { false };


private:
	_bool m_Is_In = { false };
	_bool m_Is_Out = { false };
	_bool m_Is_Font_Render = { true };

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

	_bool m_Is_Boss_Encounter = { false };
	_bool m_Is_Boss_Battle_Done = { false };

	_bool m_Is_Mon_Encounter = { false };
	_bool m_Is_Mon_Battle_Done = { false };


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CMission* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END