#include "pch.h"
#include "..\Public\Paper.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Fade_Manager.h"
#include "Level_Loading.h"
#include "OptionManager.h"

CPaper::CPaper(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPaper::CPaper(const CPaper & rhs)
	: CUI(rhs)
{
}

HRESULT CPaper::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPaper::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Frame
	if (m_UI_Desc.m_Type == 0) {
		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = 640;
		m_Origin_PosY = 220;
		
		m_Origin_X = 860.f;
		m_Origin_Y = 592.f;
		m_Size_Param = 0.8f;
		m_UI_Layer = 11;
	}

	m_Is_TimeFree = true;
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();

	m_szText_0.push_back(L"장구 소리에 귀를 기울여라");

	m_szText_1.push_back(L"항상 주위를 경계하라");
	
	m_szText_2.push_back(L"피 냄새를 쫓아 나에게 와라");
	
	
	return S_OK;
}

void CPaper::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	Set_UI();

}

void CPaper::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	// Player
	if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")) != nullptr) {

		CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));

		CTransform* m_pTargetTransformCom = pPlayer->Get_TransformCom();

		_vector PlayerPos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector Pos1 = { 31.36f,0.f,32.f };
		_vector Pos2 = { 81.15f,0.f,56.77f };
		_vector Pos3 = { 48.71f,0.f,15.85f };

		_float Dist1 = Convert::GetLength(Pos1 - PlayerPos);
		_float Dist2 = Convert::GetLength(Pos2 - PlayerPos);
		_float Dist3 = Convert::GetLength(Pos3 - PlayerPos);

		if (Dist1 < 2.f) {
			if (pGameInstance->Get_DIKeyDown(DIK_F)) {

				m_UI_Desc.m_Paper_Type = 0;

				if (m_Alpha == 0.f) {

					m_Is_In = true;
				}
				else if (m_Alpha == 1.f) {
					m_Is_Out = true;
					m_Is_Font_Render = false;

				}

			}
		}
		if (Dist2 < 2.f) {
			if (pGameInstance->Get_DIKeyDown(DIK_F)) {

				m_UI_Desc.m_Paper_Type = 1;

				if (m_Alpha == 0.f) {

					m_Is_In = true;
				}
				else if (m_Alpha == 1.f) {
					m_Is_Out = true;
					m_Is_Font_Render = false;

				}

			}
		}
		if (Dist3 < 2.f) {
			if (pGameInstance->Get_DIKeyDown(DIK_F)) {

				m_UI_Desc.m_Paper_Type = 2;

				if (m_Alpha == 0.f) {

					m_Is_In = true;
				}
				else if (m_Alpha == 1.f) {
					m_Is_Out = true;
					m_Is_Font_Render = false;

				}

			}
		}

		
	}

	Safe_Release(pGameInstance);

	if (m_Alpha == 0.f) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CPaper::Render()
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
	if (m_Is_Font_Render) {

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		/*if (m_UI_Desc.m_Paper_Type == 0) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_0[0].c_str(), _float2((_float)m_fX - 200.f 
				, (_float)m_fY), _float2(0.6f, 0.6f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Paper_Type == 1) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_1[0].c_str(), _float2((_float)m_fX - 200.f
				, (_float)m_fY), _float2(0.6f, 0.6f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Paper_Type == 2) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_2[0].c_str(), _float2((_float)m_fX - 200.f
				, (_float)m_fY), _float2(0.6f, 0.6f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;
		}*/
		

		Safe_Release(pGameInstance);

	}


	return S_OK;
}

HRESULT CPaper::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Paper"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPaper::SetUp_ShaderResources()
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


	if (m_UI_Desc.m_Type == 0) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	


	return S_OK;
}

void CPaper::Set_UI()
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

void CPaper::Get_Player_Info(_double TimeDelta)
{

	
}

void CPaper::Fade_In(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 2.7f;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
	}

	if (m_Alpha >= 0.5f) {
		m_Is_Font_Render = true;
	}
	
	if (m_Alpha >= 1.f) {
		m_Is_In = false;
		
	}

}

void CPaper::Fade_Out(_double TimeDelta)
{

	m_Alpha -= (_float)TimeDelta * 3.2f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
	}


	if (m_Alpha <= 0.f) {
		m_Is_Out = false;

	}

}



CPaper * CPaper::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPaper*		pInstance = new CPaper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPaper");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CPaper::Clone(void * pArg)
{
	CPaper*		pInstance = new CPaper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPaper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPaper::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
