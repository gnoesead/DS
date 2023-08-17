#include "pch.h"
#include "..\Public\Battle_Signal.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "Fade_Manager.h"
#include "Battle_UI_Manager.h"


CBattle_Signal::CBattle_Signal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBattle_Signal::CBattle_Signal(const CBattle_Signal & rhs)
	: CUI(rhs)
{
}

HRESULT CBattle_Signal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattle_Signal::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	 
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (m_UI_Desc.m_Type == 0) {
		m_fX = 1140;
		m_fY = 605;
		m_Origin_X = 512.f * 0.55f;
		m_Origin_Y = 480.f * 0.55f;
		m_Size_Param = 0.6f;
		m_UI_Layer = 101.f;
		m_Size_Change = 0.25f;
	}

	if (m_UI_Desc.m_Type == 1) {
		m_fX = 1250;
		m_fY = 620;
		m_Origin_X = 502.f;
		m_Origin_Y = 80.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 100.f;
	}

	if (m_UI_Desc.m_Type == 6) {
		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 712.f;
		m_Origin_Y = 456.f;
		m_Size_Param = 0.6f;
		m_UI_Layer = 100.f;
	}

	if (m_UI_Desc.m_Type == 7) {
		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 1068.f;
		m_Origin_Y = 388.f;
		m_Size_Param = 0.6f;
		m_UI_Layer = 100.f;
	}

	m_szTex.push_back(L"배틀 결과");

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();

	return S_OK;
}

void CBattle_Signal::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD2)) {

		CBattle_UI_Manager::GetInstance()->Set_Battle_Start_On(true);
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD3)) {

		CBattle_UI_Manager::GetInstance()->Set_Battle_Finish_On(true);
	}

	Safe_Release(pGameInstance);
	

	if (m_UI_Desc.m_Type == 0) {

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Result_On()) {

			if (CBattle_UI_Manager::GetInstance()->Get_Battle_Result_Size_Reset() == true) {
				m_Size_Change = 0.25f;
				CBattle_UI_Manager::GetInstance()->Set_Battle_Result_Size_Reset(false);
			}

			m_Size_Change -= (_float)TimeDelta * 1.f;

			if (m_Size_Change < 0.f) {
				m_Size_Change = 0.f;
			}

			m_Alpha += (_float)TimeDelta * 15.f;

			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}

			if (m_Size_Change == 0.f && m_Alpha == 1.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Result_On(false);
			}

		}

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Result_Off()) {
			
			m_Alpha -= (_float)TimeDelta * 25.f;

			if (m_Alpha < 0.f) {
				m_Alpha = 0.f;
			}

			if (m_Alpha == 0.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Result_Off(false);
			}

		}
	}

	if (m_UI_Desc.m_Type == 1) {

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Result_On_2()) {

			m_Is_Font_Render = true;

			m_Move_Change -= (_float)TimeDelta * 2000.f;

			if (m_Move_Change < -200.f) {
				m_Move_Change = -200.f;
			}

			m_Alpha += (_float)TimeDelta * 15.f;

			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}

			if (m_Move_Change == -200.f && m_Alpha == 1.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Result_On_2(false);
			}

		}

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Result_Off_2()) {

			m_Is_Font_Render = false;

			m_Move_Change += (_float)TimeDelta * 100.f;

			if (m_Move_Change > 0.f) {
				m_Move_Change = 0.f;
			}

			m_Alpha -= (_float)TimeDelta * 20.f;

			if (m_Alpha < 0.f) {
				m_Alpha = 0.f;
			}

			if (m_Move_Change == 0.f && m_Alpha == 0.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Result_Off_2(false);
			}

		}
	}


	if (m_UI_Desc.m_Type == 6) {

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Start_On()) {
			m_Size_Change += (_float)TimeDelta * 1.5f;

			if (m_Size_Change > 0.f) {
				m_Size_Change = 0.f;
			}

			m_Alpha += (_float)TimeDelta * 1.5f;

			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}

			if (m_Size_Change == 0.f && m_Alpha == 1.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Start_On(false);
			}

		}

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Start_Off()) {
			m_Size_Change -= (_float)TimeDelta;

			if (m_Size_Change < -0.2f) {
				m_Size_Change = -0.2f;
			}

			m_Alpha -= (_float)TimeDelta * 8.f;

			if (m_Alpha < 0.f) {
				m_Alpha = 0.f;
			}

			if (m_Size_Change == -0.2f && m_Alpha == 0.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Start_Off(false);
			}

		}
	}

	if (m_UI_Desc.m_Type == 7) {

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Finish_On()) {
			m_Size_Change += (_float)TimeDelta * 1.5f;

			if (m_Size_Change > 0.f) {
				m_Size_Change = 0.f;
			}

			m_Alpha += (_float)TimeDelta * 1.5f;

			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}

			if (m_Size_Change == 0.f && m_Alpha == 1.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Finish_On(false);
			}

		}

		if (CBattle_UI_Manager::GetInstance()->Get_Battle_Finish_Off()) {
			m_Size_Change -= (_float)TimeDelta;

			if (m_Size_Change < -0.2f) {
				m_Size_Change = -0.2f;
			}

			m_Alpha -= (_float)TimeDelta * 8.f;

			if (m_Alpha < 0.f) {
				m_Alpha = 0.f;
			}

			if (m_Size_Change == -0.2f && m_Alpha == 0.f) {
				CBattle_UI_Manager::GetInstance()->Set_Battle_Finish_Off(false);
			}

		}
	}


	if (m_UI_Desc.m_Type == 0) {
		m_Size_Param = 0.6f + m_Size_Change;
	}

	if (m_UI_Desc.m_Type == 6) {
		m_Size_Param = 0.6f + m_Size_Change;
	}

	if (m_UI_Desc.m_Type == 7) {
		m_Size_Param = 0.6f + m_Size_Change;
	}

	if (m_UI_Desc.m_Type == 1) {
		m_fX = 1250 + (_double)m_Move_Change;
	}

	Set_UI();
}

void CBattle_Signal::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Info(TimeDelta);

	if (m_Alpha == 0.f) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CBattle_Signal::Render()
{
	if (m_Is_Render == true) {

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		if (m_Is_Reverse == false)
			m_pShaderCom->Begin(1);
		else {
			m_pShaderCom->Begin(2);
		}


		if (m_Is_CutScene == false) {

			m_pVIBufferCom->Render();

		}

	}


	// 폰트 랜더
	if (m_Is_Font_Render && m_UI_Desc.m_Type == 1) {

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);


		if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTex[0].c_str(), _float2((_float)m_fX - 70.f + 4.f, (_float)m_fY - 16.f), _float2(0.5f, 0.5f))))
			return E_FAIL;

		Safe_Release(pGameInstance);

	}



	return S_OK;
}

HRESULT CBattle_Signal::Add_Components()
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


	if (m_UI_Desc.m_Type >= 0 && m_UI_Desc.m_Type <= 1) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle_Rank"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type >= 6 && m_UI_Desc.m_Type <= 7) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle_Signal"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	
	


	return S_OK;
}

HRESULT CBattle_Signal::SetUp_ShaderResources()
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

	

	if (m_UI_Desc.m_Type == 0 ) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Rank)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 1) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 5)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type >= 6 && m_UI_Desc.m_Type <= 7) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 6)))
			return E_FAIL;
	}
	

	return S_OK;
}

void CBattle_Signal::Set_UI()
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

void CBattle_Signal::Get_Info(_double TimeDelta)
{
	if (m_UI_Desc.m_Type == 0) {


		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")) != nullptr) {

			CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));

			if (pPlayer->Get_Status().fHp / pPlayer->Get_Status().fHp_Max > 0.9f)
				m_UI_Desc.m_Rank = 4;
			else if(pPlayer->Get_Status().fHp / pPlayer->Get_Status().fHp_Max > 0.8f)
				m_UI_Desc.m_Rank = 0;
			else if (pPlayer->Get_Status().fHp / pPlayer->Get_Status().fHp_Max > 0.7f)
				m_UI_Desc.m_Rank = 1;
			else if (pPlayer->Get_Status().fHp / pPlayer->Get_Status().fHp_Max > 0.6f)
				m_UI_Desc.m_Rank = 2;
			else if (pPlayer->Get_Status().fHp / pPlayer->Get_Status().fHp_Max > 0.5f)
				m_UI_Desc.m_Rank = 3;
			
		}

		Safe_Release(pGameInstance);

	}
	
}

CBattle_Signal * CBattle_Signal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattle_Signal*		pInstance = new CBattle_Signal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBattle_Signal");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CBattle_Signal::Clone(void * pArg)
{
	CBattle_Signal*		pInstance = new CBattle_Signal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBattle_Signal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_Signal::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
