#include "..\Public\MasterEffect.h"

#include "GameInstance.h"

CMasterEffect::CMasterEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMasterEffect::CMasterEffect(const CMasterEffect& rhs)
	: CGameObject(rhs)
	, m_fTextureOrder(rhs.m_fTextureOrder)
{
}

void CMasterEffect::Free()
{
	__super::Free();
}
