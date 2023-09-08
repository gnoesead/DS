#include "pch.h"
#include "..\Public\Timing_UI.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Fade_Manager.h"
#include "Level_Loading.h"
#include "OptionManager.h"
#include "Battle_UI_Manager.h"


CTiming_UI::CTiming_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTiming_UI::CTiming_UI(const CTiming_UI & rhs)
	: CUI(rhs)
{
}

HRESULT CTiming_UI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTiming_UI::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_Is_TimeFree = true;
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	if (m_UI_Desc.m_Type == 0) {
		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 200.f;
		m_Origin_Y = 200.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 2;

		m_Is_Render = false;

	}
	else if (m_UI_Desc.m_Type == 1) {

		m_fX = 640;
		m_fY = 350;
		m_Origin_X = 504.f;
		m_Origin_Y = 532.f;
		m_Size_Param = 0.8f;
		m_UI_Layer = 1;

		m_Sprite_Max = { 1 };
		m_Speed = { 30.f };

		m_Is_Render = false;

	}
	else if (m_UI_Desc.m_Type == 2) {

		m_fX = 520;
		m_fY = 300;
		m_Origin_X = 500.f;
		m_Origin_Y = 500.f;
		m_Size_Param = 0.8f;
		m_UI_Layer = 0;

		m_Sprite_Max = { 1 };
		m_Speed = { 30.f };

		m_Is_Render = false;

	}
	else if (m_UI_Desc.m_Type == 3) {

		m_fX = 750;
		m_fY = 450;
		m_Origin_X = 500.f;
		m_Origin_Y = 500.f;
		m_Size_Param = 0.8f;
		m_UI_Layer = 0;

		m_Sprite_Max = { 1 };
		m_Speed = { 30.f };

		m_Is_Render = false;

	}
	else if (m_UI_Desc.m_Type == 4) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 336.f;
		m_Origin_Y = 336.f;
		m_Size_Param = 0.5f;
		m_UI_Layer = 0;


	}
	else if (m_UI_Desc.m_Type == 5) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 772.f;
		m_Origin_Y = 772.f;
		m_Size_Param = 0.4f;
		m_UI_Layer = 1;


	}
	else if (m_UI_Desc.m_Type == 6) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 92.f;
		m_Origin_Y = 92.f;
		m_Size_Param = 0.5f;
		m_UI_Layer = 2;


	}
	else if (m_UI_Desc.m_Type == 7) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 92.f;
		m_Origin_Y = 92.f;
		m_Size_Param = 0.5f;
		m_UI_Layer = 2;


	}

	Set_UI();


	
	
	return S_OK;
}

void CTiming_UI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	if (m_UI_Desc.m_Type == 0) {
		
		if (CBattle_UI_Manager::GetInstance()->Get_Timing_Light() == true) {
			m_Is_Render = true;

			m_Origin_X += 1000.f * (_float)TimeDelta * 7.f;
			m_Origin_Y -= 180.f * (_float)TimeDelta * 7.f;


			if (m_Origin_Y < 12.f) {
				m_Origin_Y = 12.f;
			}


			if (m_Origin_X > 1400.f) {
				m_Origin_X = 200.f;
				m_Origin_Y = 200.f;
				m_Is_Render = false;
				CBattle_UI_Manager::GetInstance()->Set_Timing_Light(false);
				CBattle_UI_Manager::GetInstance()->Set_Timing_Success(false);
			}
		}
	

	}
	else if (m_UI_Desc.m_Type == 1) {

		m_Speed = 14.f;

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_Sprite1() == true) {
			m_Is_Render = true;

			m_Size_Param += (_float)TimeDelta * 7.f;

			m_Sprite += (_float)TimeDelta * m_Speed;


			if (m_Sprite > 2.f) {
				m_Size_Param = 0.8f;
				m_Sprite = 0.f;
				m_Is_Render = false;
				CBattle_UI_Manager::GetInstance()->Set_Timing_Sprite1(false);
				CBattle_UI_Manager::GetInstance()->Set_Timing_Success(false);
			}

		}

		

	}
	else if (m_UI_Desc.m_Type == 2) {

		m_Size_Param = 1.f;

		m_Speed = 14.f;

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_Sprite2() == true) {
			m_Is_Render = true;

			m_Size_Param += (_float)TimeDelta * 10.f;

			m_Sprite += (_float)TimeDelta * m_Speed;

			m_fX -= 5.2 * TimeDelta * 150;
			m_fY -= 3 * TimeDelta * 150;

			if (m_Sprite > 2.f) {
				m_fX = 520;
				m_fY = 300;
				m_Size_Param = 0.8f;
				m_Sprite = 0.f;
				m_Is_Render = false;
				CBattle_UI_Manager::GetInstance()->Set_Timing_Sprite2(false);
				CBattle_UI_Manager::GetInstance()->Set_Timing_Success(false);
			}

		}

	}
	else if (m_UI_Desc.m_Type == 3) {

		
		m_Size_Param = 1.f;

		m_Speed = 14.f;

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_Sprite3() == true) {
			m_Is_Render = true;

			m_Size_Param += (_float)TimeDelta * 7.f;

			m_Sprite += (_float)TimeDelta * m_Speed;

			m_fX += 7.5 * TimeDelta * 70;
			m_fY += 4.5 * TimeDelta * 70;

			if (m_Sprite > 2.f) {
				m_fX = 750;
				m_fY = 450;
				m_Size_Param = 0.8f;
				m_Sprite = 0.f;
				m_Is_Render = false;
				CBattle_UI_Manager::GetInstance()->Set_Timing_Sprite3(false);
				CBattle_UI_Manager::GetInstance()->Set_Timing_Success(false);
			}

		}

	}
	else if (m_UI_Desc.m_Type == 4) {

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_On() == true) {
			m_Alpha += (_float)TimeDelta * 1.f;
			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}
		}
		else {
			m_Alpha = 0;
		}
	}
	else if (m_UI_Desc.m_Type == 5) {

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_On() == true) {

			m_Size_Param -= (_float)TimeDelta * 0.5f;

			if (pGameInstance->Get_DIKeyDown(DIK_SPACE)) {

				if (m_Size_Param <= 0.18f) {

					CBattle_UI_Manager::GetInstance()->Set_Timing_Success(true);
					m_Size_Param = 0.4f;
					CBattle_UI_Manager::GetInstance()->Set_Timing_On(false);
				}
				else {
					//CBattle_UI_Manager::GetInstance()->Set_Timing_Failed(true);
					m_Size_Param = 0.4f;
					CBattle_UI_Manager::GetInstance()->Set_Timing_On(false);
				}
			}

			if (m_Size_Param < 0.05f) {
				m_Size_Param = 0.4f;
				CBattle_UI_Manager::GetInstance()->Set_Timing_On(false);
				//CBattle_UI_Manager::GetInstance()->Set_Timing_Failed(true);
			}
		}

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_On() == true) {
			m_Alpha += (_float)TimeDelta * 1.f;
			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}
		}
		else {
			m_Alpha = 0;
		}
	}
	else if (m_UI_Desc.m_Type == 6) {

		if (CBattle_UI_Manager::GetInstance()->Get_Timing_On() == true) {
			m_Alpha += (_float)TimeDelta * 1.f;
			if (m_Alpha > 1.f) {
				m_Alpha = 1.f;
			}
		}
		else {
			m_Alpha = 0;
		}
	}

	

	Safe_Release(pGameInstance);


	Set_UI();

}

void CTiming_UI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CTiming_UI::Render()
{

	if (m_Is_Render == true) {

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

	
		m_pShaderCom->Begin(1);
		
		m_pVIBufferCom->Render();

	}

	

	return S_OK;
}

HRESULT CTiming_UI::Add_Components()
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

	if (m_UI_Desc.m_Type == 0) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Light_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 1) {

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Light_Sprite_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 2) {

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Spike1_Sprite_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 3) {

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Spike2_Sprite_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 4) {

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Circle_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 5) {

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Circle_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 6) {

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timing_Circle_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CTiming_UI::SetUp_ShaderResources()
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
	else if (m_UI_Desc.m_Type == 1) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", (_uint)m_Sprite)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 2) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", (_uint)m_Sprite)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 3) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", (_uint)m_Sprite)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 4) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 5) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 1)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 6) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 2)))
			return E_FAIL;
	}
	

	return S_OK;
}

void CTiming_UI::Set_UI()
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

void CTiming_UI::Get_Player_Info(_double TimeDelta)
{

	
}

void CTiming_UI::Fade_In(_double TimeDelta)
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

void CTiming_UI::Fade_Out(_double TimeDelta)
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



CTiming_UI * CTiming_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTiming_UI*		pInstance = new CTiming_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTiming_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CTiming_UI::Clone(void * pArg)
{
	CTiming_UI*		pInstance = new CTiming_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTiming_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTiming_UI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
