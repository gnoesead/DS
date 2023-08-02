#include "pch.h"
#include "..\Public\World_UI_Hp.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"


CWorld_UI_Hp::CWorld_UI_Hp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CWorld_UI_Hp::CWorld_UI_Hp(const CWorld_UI_Hp & rhs)
	: CUI(rhs)
{
}

HRESULT CWorld_UI_Hp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorld_UI_Hp::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Hp_Bar
	if (m_UI_Desc.m_Type == 0) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 612;
		m_Origin_Y = 24;
		m_Size_Param = 1.f;
		m_UI_Layer = 5;

		m_pTransformCom->Scaling({ 0.216f * 5.f, 0.020f * 5.f, 2.f });
	}

	// Hp_Green
	if (m_UI_Desc.m_Type == 1) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 612;
		m_Origin_Y = 24;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 4;
	}

	// Hp_Red
	if (m_UI_Desc.m_Type == 2) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 612;
		m_Origin_Y = 24;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 3;
	}

	// Hp_Black
	if (m_UI_Desc.m_Type == 3) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 620;
		m_Origin_Y = 32;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 2;
	}
		


	return S_OK;
}

void CWorld_UI_Hp::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CWorld_UI_Hp::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	
	m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());
	
	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);


	// Monster
	if (pGameInstance->Get_CurLevelIdx() == 7) {

		CTransform* m_pBattleTargetTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster"), TEXT("Com_Transform")));

		_vector Pos = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	}

	Set_Personal_Pos();

	Safe_Release(pGameInstance);



	Get_Boss_Info(TimeDelta);

	if (m_Delay_Down == true) {
		m_Boss_D_Hp -= TimeDelta * 0.3;
		if (m_Boss_D_Hp <= m_Boss_Hp) {
			m_Boss_D_Hp = m_Boss_Hp;
			m_Delay_Down = false;
		}
	}

	if (m_Boss_Hp != m_Boss_D_Hp && m_Delay_Down == false) {

		m_Delay_TimeAcc += TimeDelta;

		if (m_Delay_TimeAcc > m_Delay_Max_Time) {
			m_Delay_TimeAcc = 0;
			m_Delay_Down = true;
		}
	}

	if (m_UI_Desc.m_Type == 1) {
		m_fX = 919 - 0.302 * 612 * (m_Boss_Max_Hp - m_Boss_Hp);
		m_Origin_X = (_int)(612 * m_Boss_Hp);
	}

	if (m_UI_Desc.m_Type == 2) {
		m_fX = 919 - 0.302 * 612 * (m_Boss_Max_D_Hp - m_Boss_D_Hp);
		m_Origin_X = (_int)(612 * m_Boss_D_Hp);
	}

	
	

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLD_UI, this)))
		return;
}

HRESULT CWorld_UI_Hp::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_Is_Reverse == false)
		m_pShaderCom->Begin(0);
	else {
		m_pShaderCom->Begin(2);
	}

	
	if (m_Is_CutScene == false) {

		m_pVIBufferCom->Render();

	}
	
	
	
	return S_OK;
}

HRESULT CWorld_UI_Hp::Add_Components()
{

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.dSpeedPerSec = 10.f;
	TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_World_Hp"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CWorld_UI_Hp::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;	

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

void CWorld_UI_Hp::Get_Boss_Info(_double TimeDelta)
{
	if (GetKeyState('H') < 0) {

		m_Boss_Hp -= TimeDelta * 0.5;

		if (m_Boss_Hp < 0) {
			m_Boss_Hp = 0;
		}

	}
}

void CWorld_UI_Hp::Set_Personal_Pos()
{

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Pos += {0.f, 2.f, 0.f};

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
}

CWorld_UI_Hp * CWorld_UI_Hp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWorld_UI_Hp*		pInstance = new CWorld_UI_Hp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWorld_UI_Hp");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CWorld_UI_Hp::Clone(void * pArg)
{
	CWorld_UI_Hp*		pInstance = new CWorld_UI_Hp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWorld_UI_Hp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorld_UI_Hp::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
