#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CRenderer;
END

BEGIN(Client)

class CCharacter_Dialog final : public CUI
{

private:
	CCharacter_Dialog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter_Dialog(const CCharacter_Dialog& rhs);
	virtual ~CCharacter_Dialog() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Set_Dialog(_double dTime, const _tchar* pName, const _tchar* tDialog1, const _tchar* tDialog2);

private:
	CRenderer*				m_pRendererCom = { nullptr };	

private:
	_float					m_fX = { 0.f };
	_float					m_fY = { 0.f };

	_bool					m_bRender = { false };
	_double					m_dRenderAccTime = { 0.0 };
	_double					m_dTargetTime = { 0.0 };

	_bool					m_bTrigger = { false };

	wstring					m_szName;
	wstring					m_Dialog1;
	wstring					m_Dialog2;
	
private:
	HRESULT Add_Components();

public:
	static CCharacter_Dialog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END