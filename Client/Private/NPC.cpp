#include "pch.h"
#include "..\Public\NPC.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "Camera_Free.h"

CNPC::CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CNPC::CNPC(const CNPC& rhs)
	: CCharacter(rhs)
{
}

HRESULT CNPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CNPC::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);



	if (true == m_isDead)
		return;


}

void CNPC::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	
}

HRESULT CNPC::Render()
{

	return S_OK;
}

HRESULT CNPC::Render_ShadowDepth()
{

	return S_OK;
}

void CNPC::Dir_Setting(_bool Reverse)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//카메라 방향 구해놓기
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	_float4 CameraLook = pCamera->Get_CameraLook();
	CameraLook.y = 0.0f;
	CameraLook.w = 0.0f;
	_vector vLook = XMVector4Normalize(XMLoadFloat4(&CameraLook));
	_vector	vUp = { 0.0f, 1.0f, 0.0f , 0.0f };
	_vector crossLeft = XMVector3Cross(vLook, vUp);

	//45degree look
	_vector quaternionRotation = XMQuaternionRotationAxis(vUp, XMConvertToRadians(45.0f));
	_vector v45Rotate = XMVector3Rotate(vLook, quaternionRotation);

	//135degree look
	_vector quaternionRotation2 = XMQuaternionRotationAxis(vUp, XMConvertToRadians(135.0f));
	_vector v135Rotate = XMVector3Rotate(vLook, quaternionRotation2);

	
	if (Reverse)
	{
		v45Rotate = -v45Rotate;
		v135Rotate = -v135Rotate;
	}

	Safe_Release(pGameInstance);
}

HRESULT CNPC::Add_Components()
{

	return S_OK;
}

HRESULT CNPC::SetUp_ShaderResources()
{

	return S_OK;
}

CNPC* CNPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC* pInstance = new CNPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC::Clone(void* pArg)
{
	CNPC* pInstance = new CNPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC::Free()
{
	__super::Free();
}
