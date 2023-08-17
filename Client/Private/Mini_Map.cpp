#include "pch.h"
#include "..\Public\Mini_Map.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "MissionManager.h"
#include "Mini_Map_Manager.h"
#include "Fade_Manager.h"
#include "Camera_Manager.h"



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
		m_UI_Layer = 1.f;
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
		m_UI_Layer = 2.f;
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
		m_UI_Layer = 3.f;
	}
	
	// Map
	if (m_UI_Desc.m_Type == 3) {

		m_fX = 1160;
		m_fY = 120;
		m_Origin_PosX = 1160;
		m_Origin_PosY = 120;
		m_Start_PosX = 1190;
		m_Start_PosY = 120;

		m_Origin_X = 280;
		m_Origin_Y = 272;
		m_Size_Param = 0.67f;
		m_UI_Layer = 1.5f;
	}

	// Player
	if (m_UI_Desc.m_Type == 4) {

		m_fX = 1226;
		m_fY = 63;
		m_Origin_PosX = 1160;
		m_Origin_PosY = 120;
		m_Start_PosX = 1190;
		m_Start_PosY = 120;

		m_Origin_X = 40;
		m_Origin_Y = 40;
		m_Size_Param = 0.6f;
		m_UI_Layer = 1.6f;
	}





	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();



	return S_OK;
}

void CMini_Map::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	// Map
	if (m_UI_Desc.m_Type == 3) {

		if (m_Map_Type == 0) {

			m_fX = 1160 + (0.5 - m_UV_Centor_X) * 260;
			m_fY = 120 - (0.5 - m_UV_Centor_Y) * 260;

			CMiniMapManager::GetInstance()->Set_MiniMap_X((_float)m_fX);
			CMiniMapManager::GetInstance()->Set_MiniMap_Y((_float)m_fY);

			m_Origin_X = 1080;
			m_Origin_Y = 1080;
			m_Size_Param = 0.25f;
			m_UI_Layer = 1.5;

			m_UV_Cut_MinX = { m_UV_Centor_X - 0.3f };
			if (m_UV_Cut_MinX <= 0.f)
				m_UV_Cut_MinX = 0.f;

			m_UV_Cut_MaxX = { m_UV_Centor_X + 0.3f };
			if (m_UV_Cut_MaxX >= 1.f)
				m_UV_Cut_MaxX = 1.f;

			m_UV_Cut_MinY = { (1 - m_UV_Centor_Y) - 0.3f };
			if (m_UV_Cut_MinY <= 0.f)
				m_UV_Cut_MinY = 0.f;

			m_UV_Cut_MaxY = { (1 - m_UV_Centor_Y) + 0.3f };
			if (m_UV_Cut_MaxY >= 1.f)
				m_UV_Cut_MaxY = 1.f;

		}

		if (m_Map_Type == 1) {

			m_fX = 1160 + (0.5 - m_UV_Centor_X) * 260;
			m_fY = 120 - (0.5 - m_UV_Centor_Y) * 260;

			CMiniMapManager::GetInstance()->Set_MiniMap_X((_float)m_fX);
			CMiniMapManager::GetInstance()->Set_MiniMap_Y((_float)m_fY);

			m_Origin_X = 1080;
			m_Origin_Y = 1080;
			m_Size_Param = 0.25f;
			m_UI_Layer = 1.5;

			m_UV_Cut_MinX = { m_UV_Centor_X - 0.3f };
			if (m_UV_Cut_MinX <= 0.f)
				m_UV_Cut_MinX = 0.f;

			m_UV_Cut_MaxX = { m_UV_Centor_X + 0.3f };
			if (m_UV_Cut_MaxX >= 1.f)
				m_UV_Cut_MaxX = 1.f;

			m_UV_Cut_MinY = { (1 - m_UV_Centor_Y) - 0.3f };
			if (m_UV_Cut_MinY <= 0.f)
				m_UV_Cut_MinY = 0.f;

			m_UV_Cut_MaxY = { (1 - m_UV_Centor_Y) + 0.3f };
			if (m_UV_Cut_MaxY >= 1.f)
				m_UV_Cut_MaxY = 1.f;

		}

		if (m_Map_Type == 2) {

			m_fX = 1160 + (0.5 - m_UV_Centor_X) * 370;
			m_fY = 117;

			CMiniMapManager::GetInstance()->Set_MiniMap_X((_float)m_fX);
			CMiniMapManager::GetInstance()->Set_MiniMap_Y((_float)m_fY);

			m_Origin_X = 1080;
			m_Origin_Y = 449;
			m_Size_Param = 0.35f;
			m_UI_Layer = 1.5;

			m_UV_Cut_MinX = { m_UV_Centor_X - 0.21f };
			if (m_UV_Cut_MinX <= 0.f)
				m_UV_Cut_MinX = 0.f;

			m_UV_Cut_MaxX = { m_UV_Centor_X + 0.21f };
			if (m_UV_Cut_MaxX >= 1.f)
				m_UV_Cut_MaxX = 1.f;

			m_UV_Cut_MinY = { (m_UV_Centor_Y) - 0.8f };
			if (m_UV_Cut_MinY <= 0.f)
				m_UV_Cut_MinY = 0.f;

			m_UV_Cut_MaxY = { (m_UV_Centor_Y) + 0.8f };
			if (m_UV_Cut_MaxY >= 1.f)
				m_UV_Cut_MaxY = 1.f;

		}

		if (m_Map_Type == 3) {

			m_fX = 1160 + (0.5 - m_UV_Centor_X) * 260;
			m_fY = 120 - (0.5 - m_UV_Centor_Y) * 260;

			CMiniMapManager::GetInstance()->Set_MiniMap_X((_float)m_fX);
			CMiniMapManager::GetInstance()->Set_MiniMap_Y((_float)m_fY);

			m_Origin_X = 1080;
			m_Origin_Y = 1080;
			m_Size_Param = 0.25f;
			m_UI_Layer = 1.5;

			m_UV_Cut_MinX = { m_UV_Centor_X - 0.3f };
			if (m_UV_Cut_MinX <= 0.f)
				m_UV_Cut_MinX = 0.f;

			m_UV_Cut_MaxX = { m_UV_Centor_X + 0.3f };
			if (m_UV_Cut_MaxX >= 1.f)
				m_UV_Cut_MaxX = 1.f;

			m_UV_Cut_MinY = { (1 - m_UV_Centor_Y) - 0.3f };
			if (m_UV_Cut_MinY <= 0.f)
				m_UV_Cut_MinY = 0.f;

			m_UV_Cut_MaxY = { (1 - m_UV_Centor_Y) + 0.3f };
			if (m_UV_Cut_MaxY >= 1.f)
				m_UV_Cut_MaxY = 1.f;

		}
	}

	// Player
	if (m_UI_Desc.m_Type == 4) {

		if (m_Map_Type == 0) {
			m_fX = 1145 - (0.1912 - m_UV_Player_X) * 310 * 0.8f - (1160 + (0.5 - 0.3) * 260 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_X());

			m_fY = 150 + (0.1765 - m_UV_Player_Y) * 310 * 0.8f - (120 - (0.5 - 0.3) * 260 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_Y());
		}

		if (m_Map_Type == 1) {
			m_fX = 1275 - (0.736 - m_UV_Player_X) * 280 * 0.8f - (1160 + (0.5 - 0.3) * 260 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_X());

			m_fY = 180 + (0.05 - m_UV_Player_Y) * 310 * 0.8f - (120 - (0.5 - 0.3) * 260 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_Y());
		}

		if (m_Map_Type == 2) {
			m_fX = 1380 - (0.85 - m_UV_Player_X) * 440 * 0.8f - (1160 + (0.5 - 0.21) * 370 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_X());

			m_fY = 170 + (0.14 - m_UV_Player_Y) * 170 * 0.8f;
		}

		if (m_Map_Type == 3) {
			m_fX = 1202 - (0.444 - m_UV_Player_X) * 300 * 0.8f - (1160 + (0.5 - 0.3) * 250 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_X());

			m_fY = 170 + (0.136 - m_UV_Player_Y) * 340 * 0.8f - (120 - (0.5 - 0.3) * 268 - (_double)CMiniMapManager::GetInstance()->Get_MiniMap_Y());
		}
	}

	Set_UI();

}

void CMini_Map::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	// Map
	if (m_Map_Type == 0) {

		if (m_UV_Centor_X <= 0.3f) {
			m_UV_Centor_X = 0.3f;
		}
		if (m_UV_Centor_X >= 0.7f) {
			m_UV_Centor_X = 0.7f;
		}

		if (m_UV_Centor_Y <= 0.3f) {
			m_UV_Centor_Y = 0.3f;
		}
		if (m_UV_Centor_Y >= 0.7f) {
			m_UV_Centor_Y = 0.7f;
		}

	}

	if (m_Map_Type == 1) {

		if (m_UV_Centor_X <= 0.3f) {
			m_UV_Centor_X = 0.3f;
		}
		if (m_UV_Centor_X >= 0.7f) {
			m_UV_Centor_X = 0.7f;
		}

		if (m_UV_Centor_Y <= 0.3f) {
			m_UV_Centor_Y = 0.3f;
		}
		if (m_UV_Centor_Y >= 0.7f) {
			m_UV_Centor_Y = 0.7f;
		}

	}

	if (m_Map_Type == 2) {

		if (m_UV_Centor_X <= 0.21f) {
			m_UV_Centor_X = 0.21f;
		}
		if (m_UV_Centor_X >= 0.8f) {
			m_UV_Centor_X = 0.8f;
		}

		if (m_UV_Centor_Y <= 0.2f) {
			m_UV_Centor_Y = 0.2f;
		}
		if (m_UV_Centor_Y >= 0.8f) {
			m_UV_Centor_Y = 0.8f;
		}

	}

	if (m_Map_Type == 3) {

		if (m_UV_Centor_X <= 0.3f) {
			m_UV_Centor_X = 0.3f;
		}
		if (m_UV_Centor_X >= 0.7f) {
			m_UV_Centor_X = 0.7f;
		}

		if (m_UV_Centor_Y <= 0.3f) {
			m_UV_Centor_Y = 0.3f;
		}
		if (m_UV_Centor_Y >= 0.7f) {
			m_UV_Centor_Y = 0.7f;
		}

	}
	

	Safe_Release(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CMini_Map::Render()
{

	if (m_Is_Render == true) {

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		if (m_UI_Desc.m_Type == 3) {
			m_pShaderCom->Begin(13);
		}
		else {
			if (m_Is_Reverse == false)
				m_pShaderCom->Begin(1);
			else {

				m_pShaderCom->Begin(2);

			}
		}


		if (m_Is_CutScene == false) {

			m_pVIBufferCom->Render();

		}

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


	if (FAILED(m_pShaderCom->SetUp_RawValue("g_UV_Cut_MinX", &m_UV_Cut_MinX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_UV_Cut_MaxX", &m_UV_Cut_MaxX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_UV_Cut_MinY", &m_UV_Cut_MinY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_UV_Cut_MaxY", &m_UV_Cut_MaxY, sizeof(_float))))
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

	if (m_UI_Desc.m_Type == 3) {

		if (m_Map_Type == 3) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 7)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_Map_Type + 3)))
				return E_FAIL;
		}
		
	}
	else if(m_UI_Desc.m_Type == 4) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 6)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
			return E_FAIL;
	}

	
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
	_matrix		RotationMatrix = XMMatrixIdentity();

	TransformMatrix.r[0] = TransformMatrix.r[0] * (_float)m_fSizeX;
	TransformMatrix.r[1] = TransformMatrix.r[1] * (_float)m_fSizeY;

	TransformMatrix.r[3] = XMVectorSetX(TransformMatrix.r[3], (_float)m_fX - Viewport.Width * 0.5f);
	TransformMatrix.r[3] = XMVectorSetY(TransformMatrix.r[3], -(_float)m_fY + Viewport.Height * 0.5f);


	if (m_UI_Desc.m_Type == 4) {
		
		_float3		vScale = _float3(XMVectorGetX(XMVector3Length(TransformMatrix.r[0])),
			                         XMVectorGetX(XMVector3Length(TransformMatrix.r[1])),
			                         XMVectorGetX(XMVector3Length(TransformMatrix.r[2])));

		_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
		_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
		_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

		_vector		vAxisX = { 1.f, 0.f, 0.f, 0.f };
		_vector		vAxisY = { 0.f, 1.f, 0.f, 0.f };
		_vector		vAxisZ = { 0.f, 0.f, 1.f, 0.f };

		_matrix		RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), -m_Angle );

		TransformMatrix.r[0] = XMVector3TransformNormal(vRight, RotationMatrix);
		TransformMatrix.r[1] = XMVector3TransformNormal(vUp, RotationMatrix);
		TransformMatrix.r[2] = XMVector3TransformNormal(vLook, RotationMatrix);

	}
	

	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);

}

void CMini_Map::Get_Player_Info(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	m_Is_Dialog_On = CMissionManager::GetInstance()->Get_Is_Dialog_On();

	_bool Is_Battle = CFadeManager::GetInstance()->Get_Is_Battle();

	if (Is_Battle == true) {
		m_Is_Dialog_On = true;
	}

	if (m_Is_Dialog_On == false && m_Is_In == false && m_Is_Out == false) {
		m_Is_In = true;
	}

	if (m_Is_Dialog_On == true && m_Is_In == false && m_Is_Out == false) {
		m_Is_Out = true;
	}


	// Player
	CTransform* m_pTargetTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector Pos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE) {

		_vector Trigger = { 131.f, 3.f, 57.f };
		_vector vDist = Pos - Trigger;

		_float dist = XMVectorGetX(XMVector3Length(vDist));

		if (XMVectorGetX(Pos) > 34) {
			m_Map_Type = 1;
		}

		if (XMVectorGetX(Pos) > 115) {
			m_Map_Type = 2;
		}

	}

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE) {

		m_Map_Type = 3;

		_vector Trigger = { 600.f, 4.5f, 317.f };
		_vector vDist = Pos - Trigger;

		_float dist = XMVectorGetX(XMVector3Length(vDist));

	}

	if (m_Map_Type == 0) {

		m_UV_Centor_X = XMVectorGetX(Pos);

		m_UV_Centor_X = _float((m_UV_Centor_X - 1.2) / (36.5 - 1.2));

		m_UV_Centor_Y = XMVectorGetZ(Pos);

		m_UV_Centor_Y = _float((m_UV_Centor_Y - 2.413) / (40.729 - 2.413));

		m_UV_Player_X = m_UV_Centor_X;
		m_UV_Player_Y = m_UV_Centor_Y;

		_vector Look = m_pTargetTransformCom->Get_State(CTransform::STATE_LOOK);

		Look = XMVectorSetY(Look, 0.f);

		_vector Z = { 0.f,0.f,1.f };

		Look = XMVector3Normalize(Look);

		if (XMVectorGetY(XMVector3Cross(Z, Look)) > 0) {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, Look)));
		}
		else {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, -Look)));

			m_Angle = m_Angle + XMConvertToRadians(180.f);
		}

	}

	if (m_Map_Type == 1) {

		m_UV_Centor_X = XMVectorGetX(Pos);

		m_UV_Centor_X = (m_UV_Centor_X - 35) / (95 - 29);

		m_UV_Centor_Y = XMVectorGetZ(Pos);

		m_UV_Centor_Y = (m_UV_Centor_Y - 1) / (79 - 1);

		m_UV_Player_X = m_UV_Centor_X;
		m_UV_Player_Y = m_UV_Centor_Y;

		_vector Look = m_pTargetTransformCom->Get_State(CTransform::STATE_LOOK);

		Look = XMVectorSetY(Look, 0.f);

		_vector Z = { 0.f,0.f,1.f };

		Look = XMVector3Normalize(Look);

		if (XMVectorGetY(XMVector3Cross(Z, Look)) > 0) {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, Look)));
		}
		else {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, -Look)));

			m_Angle = m_Angle + XMConvertToRadians(180.f);
		}

	}

	if (m_Map_Type == 2) {

		m_UV_Centor_X = XMVectorGetX(Pos);

		m_UV_Centor_X = (m_UV_Centor_X - 116) / (201 - 116);

		m_UV_Centor_Y = XMVectorGetZ(Pos);

		m_UV_Centor_Y = (m_UV_Centor_Y - 25) / (63 - 25);

		m_UV_Player_X = m_UV_Centor_X;
		m_UV_Player_Y = m_UV_Centor_Y;

		_vector Look = m_pTargetTransformCom->Get_State(CTransform::STATE_LOOK);

		Look = XMVectorSetY(Look, 0.f);

		_vector Z = { 0.f,0.f,1.f };

		Look = XMVector3Normalize(Look);

		if (XMVectorGetY(XMVector3Cross(Z, Look)) > 0) {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, Look)));
		}
		else {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, -Look)));

			m_Angle = m_Angle + XMConvertToRadians(180.f);
		}

	}

	if (m_Map_Type == 3) {

		m_UV_Centor_X = XMVectorGetX(Pos);

		m_UV_Centor_X = (m_UV_Centor_X - 480) / (689 - 480);

		m_UV_Centor_Y = XMVectorGetZ(Pos);

		m_UV_Centor_Y = (m_UV_Centor_Y - 214) / (420 - 214);

		m_UV_Player_X = m_UV_Centor_X;
		m_UV_Player_Y = m_UV_Centor_Y;

		_vector Look = m_pTargetTransformCom->Get_State(CTransform::STATE_LOOK);

		Look = XMVectorSetY(Look, 0.f);

		_vector Z = { 0.f,0.f,1.f };

		Look = XMVector3Normalize(Look);

		if (XMVectorGetY(XMVector3Cross(Z, Look)) > 0) {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, Look)));
		}
		else {
			m_Angle = acos(XMVectorGetY(XMVector3Dot(Z, -Look)));

			m_Angle = m_Angle + XMConvertToRadians(180.f);
		}

	}


	Safe_Release(pGameInstance);
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

	if (m_UI_Desc.m_Type == 3 || m_UI_Desc.m_Type == 4) {
		if (m_Alpha >= 1.f) {
			m_Is_In = false;
		
		}
	}
	else {
		if (m_Alpha >= 1.f && m_fX <= m_Origin_PosX) {
			m_Is_In = false;
		
		}
	}
}

void CMini_Map::Fade_Out(_double TimeDelta)
{

	m_Alpha -= (_float)TimeDelta * 3.f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
	}

	m_fX += TimeDelta * 120.0;

	if (m_fX >= m_Start_PosX)
	{
		m_fX = m_Start_PosX;
		
	}


	if (m_UI_Desc.m_Type == 3 || m_UI_Desc.m_Type == 4) {
		if (m_Alpha <= 0.f) {
			m_Is_Out = false;
		
		}
	}
	else {
		if (m_Alpha <= 0.f && m_fX >= m_Start_PosX) {
			m_Is_Out = false;
		
		}
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
