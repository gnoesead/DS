#include "pch.h"
#include "..\Public\Mini_Map.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "MissionManager.h"



CMini_Map::CMini_Map(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMini_Map::CMini_Map(const CMini_Map & rhs)
	: CUI(rhs)
{
}

HRESULT CMini_Map::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMini_Map::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// BackGround
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 1160;
		m_fY = 120;
		m_Origin_PosX = 1160;
		m_Origin_PosY = 120;
		m_Start_PosX = 1190;
		m_Start_PosY = 120;

		m_Origin_X = 240.f;
		m_Origin_Y = 240.f;
		m_Size_Param = 0.67f;
		m_UI_Layer = 1;
	}

	// Wire
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 1160;
		m_fY = 120;
		m_Origin_PosX = 1160;
		m_Origin_PosY = 120;
		m_Start_PosX = 1190;
		m_Start_PosY = 120;

		m_Origin_X = 240.f;
		m_Origin_Y = 240.f;
		m_Size_Param = 0.67f;
		m_UI_Layer = 2;
	}

	// Frame
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 1160;
		m_fY = 120;
		m_Origin_PosX = 1160;
		m_Origin_PosY = 120;
		m_Start_PosX = 1190;
		m_Start_PosY = 120;

		m_Origin_X = 280;
		m_Origin_Y = 272;
		m_Size_Param = 0.67f;
		m_UI_Layer = 3;
	}
	
	

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();



	return S_OK;
}

void CMini_Map::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	Set_UI();

}

void CMini_Map::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_Is_Dialog_On = CMissionManager::GetInstance()->Get_Is_Dialog_On();

	if (m_Is_Dialog_On == false && m_Is_In == false && m_Is_Out == false) {
		m_Is_In = true;
	}

	if (m_Is_Dialog_On == true && m_Is_In == false && m_Is_Out == false) {
		m_Is_Out = true;
	}


	
	Safe_Release(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CMini_Map::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_Is_Reverse == false)
		m_pShaderCom->Begin(1);
	else {
		m_pShaderCom->Begin(2);
	}

	
	if (m_Is_CutScene == false && m_Is_Render == true) {

		m_pVIBufferCom->Render();

	}

	

	return S_OK;
}

HRESULT CMini_Map::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mini_Map"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CMini_Map::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_R", &m_Is_Side_Cut_R, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_L", &m_Is_Side_Cut_L, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
		return E_FAIL;



	return S_OK;
}

void CMini_Map::Set_UI()
{
	m_fSizeX = m_Origin_X * (_double)m_Size_Param;
	m_fSizeY = m_Origin_Y * (_double)m_Size_Param;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		Viewport;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix.r[0] = TransformMatrix.r[0] * (_float)m_fSizeX;
	TransformMatrix.r[1] = TransformMatrix.r[1] * (_float)m_fSizeY;

	TransformMatrix.r[3] = XMVectorSetX(TransformMatrix.r[3], (_float)m_fX - Viewport.Width * 0.5f);
	TransformMatrix.r[3] = XMVectorSetY(TransformMatrix.r[3], -(_float)m_fY + Viewport.Height * 0.5f);

	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);

}

void CMini_Map::Get_Player_Info(_double TimeDelta)
{

	
}

void CMini_Map::Fade_In(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 2.f;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
	}

	m_fX -= TimeDelta * 120.0;

	if (m_fX <= m_Origin_PosX)
	{
		m_fX = m_Origin_PosX;
	}

	if (m_Alpha >= 1.f && m_fX <= m_Origin_PosX) {
		m_Is_In = false;
	}


}

void CMini_Map::Fade_Out(_double TimeDelta)
{

	m_Alpha -= (_float)TimeDelta * 3.f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
	}

	m_fX += TimeDelta * 100.0;

	if (m_fX >= m_Start_PosX)
	{
		m_fX = m_Start_PosX;
		
	}

	if (m_Alpha <= 0.f && m_fX >= m_Start_PosX) {
		m_Is_Out = false;
	}

}



CMini_Map * CMini_Map::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMini_Map*		pInstance = new CMini_Map(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMini_Map");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CMini_Map::Clone(void * pArg)
{
	CMini_Map*		pInstance = new CMini_Map(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMini_Map");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMini_Map::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
