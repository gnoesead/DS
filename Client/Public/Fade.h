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

class CFade final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CFade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFade(const CFade& rhs);
	virtual ~CFade() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private: /* ���� ����ϰ��ϴ� ������Ʈ�� ����� �˻����� �ٷ� ����� �� �ִ�. */
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
	void Set_UI();
	void Get_Info(_double TimeDelta);
	void Fade_In(_double TimeDelta);
	void Fade_Out(_double TimeDelta);
	void Fade_OutIn(_double TimeDelta);
	void Fade_OutIn_Basic(_double TimeDelta);


private:
	_bool m_Is_In = { false };
	_bool m_Is_Out = { false };
	_bool m_Is_OutIn = { false };
	_bool m_Is_OutIn_Basic = { false };
	_bool m_Is_Ink_On = { false };

	_bool m_Is_In_Done = { false };
	_bool m_Is_Out_Done = { false };
	_bool m_Is_OutIn_Done = { false };
	_bool m_Is_OutIn_Basic_Done = { false };
	_bool m_Is_Ink_On_Done = { false };

	_float m_TimeAcc = { 0.f };

	_float m_InOut_Speed = { 1.f };
	_float m_Delay_TimeAcc = { 0.f };
	_bool  m_Delay_On = { false };
	_float m_Delay_Time = { 1.f };


private:
	_float m_InOut_Basic_Speed = { 1.f };
	_float m_Delay_Basic_TimeAcc = { 0.f };
	_bool  m_Delay_Basic_On = { false };
	_float m_Delay_Basic_Time = { 1.f };




	_float m_Ink_Sprite = { 0.f };


private:
	UIDESC                  m_UI_Desc = {};

	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 0.f };
	_float                  m_Alpha_Dir = { 1.f };


	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_bool m_Pt_In = { false };

	_float m_Size_Param = {};

	_float m_Origin_Size_Param = {};

	_float m_Origin_X = {};
	_float m_Origin_Y = {};


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CFade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END