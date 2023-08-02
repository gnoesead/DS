#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)

class CFIcon final : public CUI
{

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = { 0 };
	}UIDESC;


private:
	CFIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFIcon(const CFIcon& rhs);
	virtual ~CFIcon() = default;

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
	CTransform*              m_pTransformCom = { nullptr };
public:
	void Get_Boss_Info(_double TimeDelta);
	void Set_Personal_Pos();

private:	
	_double m_Boss_Max_Hp = { 1 };
	_double m_Boss_Hp = { 1 };

	_double m_Boss_Max_D_Hp = { 1 };
	_double m_Boss_D_Hp = { 1 };

	_double m_Delay_TimeAcc = { 0 };
	_double m_Delay_Max_Time = { 3 };
	_bool m_Delay_Down = { false };

private:
	UIDESC                  m_UI_Desc = {};

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 1.f };

	_tchar					m_szX[MAX_PATH] = TEXT("");
	_tchar					m_szY[MAX_PATH] = TEXT("");

	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_float m_Size_Param = {};

	_float m_Origin_X = {};
	_float m_Origin_Y = {};

	_float m_fZ = {};

	_vector m_vTargetPos = {};
	_float m_Up_Move = { 3.f };
	_float m_Right_Move = { 0.f };

	_vector m_vBattle_Targt = {};

	
	
private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CFIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END