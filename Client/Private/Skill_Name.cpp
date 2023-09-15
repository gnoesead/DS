#include "pch.h"
#include "..\Public\Skill_Name.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "Fade_Manager.h"
#include "Battle_UI_Manager.h"
#include "PlayerManager.h"



CSkill_Name::CSkill_Name(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CSkill_Name::CSkill_Name(const CSkill_Name & rhs)
	: CUI(rhs)
{
}

HRESULT CSkill_Name::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Name::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Player
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 230;
		m_fY = 335;
		m_Origin_X = 496;
		m_Origin_Y = 16;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Boss
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 1140;
		m_fY = 335;
		m_Origin_X = 496;
		m_Origin_Y = 16;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

		
	m_szTanjiro_Skill.push_back(L"제8형 용소");
	m_szTanjiro_Skill.push_back(L"제2형 물방아");
	m_szTanjiro_Skill.push_back(L"제6형 비틀린 소용돌이");

	m_szZenitsu_Skill.push_back(L"제1형 벽력일섬");
	m_szZenitsu_Skill.push_back(L"제3형 공전");
	m_szZenitsu_Skill.push_back(L"제7형 화뢰신");


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CSkill_Name::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (CBattle_UI_Manager::GetInstance()->Get_Skill_On() == true) {

		m_Is_Font_Render = true;

		m_Alpha += (_float)TimeDelta * 6.f;

		if (m_Alpha > 1.f) {
			m_Alpha = 1.f;
		}
	}
	else {

		m_Is_Font_Render = false;

		m_Alpha -= (_float)TimeDelta * 6.f;

		if (m_Alpha < 0.f) {
			m_Alpha = 0.f;
		}
	}



	Set_UI();

}

void CSkill_Name::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_Player_Index = CBattle_UI_Manager::GetInstance()->Get_Player_Type();
	m_Player_Skill_Index = CBattle_UI_Manager::GetInstance()->Get_Player_Skill_Type();

	m_Is_Render = CFadeManager::GetInstance()->Get_Is_Battle();

	Get_Player_Info(TimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CSkill_Name::Render()
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


		if (m_Is_Font_Render == true) {


			if (m_UI_Desc.m_Type == 0) {
				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);

				if (m_Player_Index == 0) {

					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTanjiro_Skill[m_Player_Skill_Index].c_str(),
						_float2((_float)m_fX - 151.f, (_float)m_fY - 22.f), _float2(0.45f, 0.45f), XMVectorSet(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f))))
						return E_FAIL;
				}
				else if(m_Player_Index == 1){
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szZenitsu_Skill[m_Player_Skill_Index].c_str(),
						_float2((_float)m_fX - 151.f, (_float)m_fY - 22.f), _float2(0.45f, 0.45f), XMVectorSet(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f))))
						return E_FAIL;
				}

				Safe_Release(pGameInstance);
			}

		}
	}
	
	return S_OK;
}

HRESULT CSkill_Name::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skill_Name"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkill_Name::SetUp_ShaderResources()
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


	
	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	
	
	
	return S_OK;
}

void CSkill_Name::Set_UI()
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

void CSkill_Name::Get_Mouse_Pos()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		Viewport;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(g_hWnd, &ptCursor);

	m_MouseX = ptCursor.x;
	m_MouseY = ptCursor.y;

	m_ScreenWidth = g_iWinSizeX;
	m_ScreenHeight = g_iWinSizeY;

	if (m_MouseX < 0) { m_MouseX = 0; }
	if (m_MouseY < 0) { m_MouseY = 0; }

	if (m_MouseX > m_ScreenWidth) { m_MouseX = m_ScreenWidth; }
	if (m_MouseY > m_ScreenHeight) { m_MouseY = m_ScreenHeight; }


	Safe_Release(pGameInstance);
}

void CSkill_Name::Change_Info(_double TimeDelta)
{

	_long		MouseMove = 0;

	if (m_Is_Fix == false) {
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (fabs(MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X)) > 1.f)
		{
			m_fX += TimeDelta * MouseMove * 40.f;
		}

		if (fabs(MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y)) > 1.f)
		{
			m_fY += TimeDelta * MouseMove * 40.f;
		}

		if (GetKeyState(VK_UP) < 0) {
			m_Size_Param += (_float)TimeDelta * 0.5f;

		}
		if (GetKeyState(VK_DOWN) < 0) {
			m_Size_Param -= (_float)TimeDelta * 0.5f;

			if (m_Size_Param <= 0) {
				m_Size_Param = 0;
			}
		}
		if (GetKeyState(VK_LEFT) < 0) {

		}
		if (GetKeyState(VK_RIGHT) < 0) {

		}

		Safe_Release(pGameInstance);
	}
}

void CSkill_Name::Tool_Funtion(_double TimeDelta)
{
	//// 툴 전용 
	//if (CImGUI_Manager::Get_Instance()->Get_Mouse_Layer() == m_UI_Layer || CImGUI_Manager::Get_Instance()->Get_Mouse_Layer() == 99) {

	//	if (m_Pt_In) {

	//		_long		MouseMove = 0;

	//		if (GetKeyState(VK_LBUTTON) < 0) {

	//			if (GetKeyState('F') < 0) {
	//				m_Is_Fix = true;
	//			}
	//			if (GetKeyState('G') < 0) {
	//				m_Is_Fix = false;
	//			}

	//			if (m_Is_Fix == false)
	//				m_Alpha = 0.5f;

	//			Change_Info(TimeDelta);

	//			CImGUI_Manager::Get_Instance()->Update_UI_Info(m_fX, m_fY, m_Origin_X, m_Origin_Y, m_Size_Param, m_UI_Layer);

	//		}
	//		else {
	//			m_Alpha = 1.f;
	//		}

	//	}
	//	else {
	//		m_Alpha = 1.f;
	//	}
	//}
	//else {
	//	m_Alpha = 1.f;
	//}
}

void CSkill_Name::Get_Player_Info(_double TimeDelta)
{

	

}



CSkill_Name * CSkill_Name::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkill_Name*		pInstance = new CSkill_Name(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkill_Name");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CSkill_Name::Clone(void * pArg)
{
	CSkill_Name*		pInstance = new CSkill_Name(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkill_Name");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Name::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
