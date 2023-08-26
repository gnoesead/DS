#include "..\Public\UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Is_Side_Cut_R = false;
	m_Is_Side_Cut_L = false;

	return S_OK;
}

void CUI::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CUI::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

}

HRESULT CUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

_bool CUI::Pt_InUI()
{
	_vector Pt = { m_MouseX,m_MouseY,0 };

	_vector Axis;
	_vector dir;

	_vector LT = { m_fX - m_fSizeX * 0.5f,m_fY + m_fSizeY * 0.5f,0 };
	_vector RT = { m_fX + m_fSizeX * 0.5f,m_fY + m_fSizeY * 0.5f,0 };
	_vector LD = { m_fX - m_fSizeX * 0.5f,m_fY - m_fSizeY * 0.5f,0 };
	_vector RD = { m_fX + m_fSizeX * 0.5f,m_fY - m_fSizeY * 0.5f,0 };

	// LT
	Axis = LT - LD;
	dir = Pt - LT;

	if (XMVectorGetX(XMVector3Dot(Axis, dir)) > 0) {
		return FALSE;
	}

	// RT
	Axis = RT - LT;
	dir = Pt - RT;

	if (XMVectorGetX(XMVector3Dot(Axis, dir)) > 0) {
		return FALSE;
	}

	// RD
	Axis = RD - RT;
	dir = Pt - RD;

	if (XMVectorGetX(XMVector3Dot(Axis, dir)) > 0) {
		return FALSE;
	}

	// LD
	Axis = LD - RD;
	dir = Pt - LD;

	if (XMVectorGetX(XMVector3Dot(Axis, dir)) > 0) {
		return FALSE;
	}


	return TRUE;
}

void CUI::Free()
{
	__super::Free();
}
