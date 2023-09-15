#include "pch.h"
#include "..\Public\Character_Dialog.h"

#include "GameInstance.h"
#include "Layer.h"

CCharacter_Dialog::CCharacter_Dialog(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCharacter_Dialog::CCharacter_Dialog(const CCharacter_Dialog & rhs)
	: CUI(rhs)
{
}

HRESULT CCharacter_Dialog::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter_Dialog::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCharacter_Dialog::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
		m_fX -= 0.1f;

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
		m_fX += 0.1f;

	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
		m_fY -= 0.1f;

	if (pGameInstance->Get_DIKeyState(DIK_UP))
		m_fY += 0.1f;

	Safe_Release(pGameInstance);

	/*if (m_bTrigger)
	{
		m_bRender = true;
		m_bTrigger = false;
		m_dRenderAccTime = 0.0;
	}*/

	if (m_bRender)
	{
		m_dRenderAccTime += TimeDelta;

		if (m_dRenderAccTime > m_dTargetTime)
		{
			m_bRender = false;
			m_dRenderAccTime = 0.0;
		}
	}
}

void CCharacter_Dialog::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (m_bRender)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return;
	}
}

HRESULT CCharacter_Dialog::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName.c_str(), _float2(382.f, 526.f), _float2(0.6f, 0.6f))))
		return E_FAIL;

	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_Dialog1.c_str(), _float2(440.f, 580.f), _float2(0.55f, 0.55f))))
		return E_FAIL;

	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_Dialog2.c_str(), _float2(440.f, 620.f), _float2(0.55f, 0.55f))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CCharacter_Dialog::Set_Dialog(_double dTime, const _tchar* pName, const _tchar* tDialog1, const _tchar* tDialog2)
{
	m_bRender = true;

	m_dTargetTime = dTime;

	m_szName = pName;
	m_Dialog1 = tDialog1;
	m_Dialog2 = tDialog2;
}

HRESULT CCharacter_Dialog::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CCharacter_Dialog * CCharacter_Dialog::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCharacter_Dialog*		pInstance = new CCharacter_Dialog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCharacter_Dialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CCharacter_Dialog::Clone(void * pArg)
{
	CCharacter_Dialog*		pInstance = new CCharacter_Dialog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCharacter_Dialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacter_Dialog::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
}
