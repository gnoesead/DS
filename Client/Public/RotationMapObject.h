#pragma once

#include "Client_Defines.h"
#include "MapObject.h"
#include "GameInstance.h"

BEGIN(Client)

class CRotationMapObject  final : public CMapObject
{
public:
	enum ROTATION { ROT_X_PLUS, ROT_X_MINUS, ROT_Z_PLUS, ROT_Z_MINUS, ROT_END };
private:
	CRotationMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotationMapObject(const CRotationMapObject& rhs);
	virtual ~CRotationMapObject() = default;

public:
	void   Set_TurnRoomTriggerOn(ROTATION eRot) { m_bTriggerTurnRoom = true; m_eRotation = eRot; }
	_bool   Get_TurnRoom() {
		return m_bTurn;
	}

	_float3 Get_RotAngle() { return m_vRotAngle; }

	_vector Get_RoomPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CRotationMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	void   TurnRoom(_double TimeDelta);

private:
	_double         m_TimeAcc = { 0.0 };
	_bool         m_bTurn = { false };

	_float3         m_vTargetRotAngle{ 0.f ,0.f , 0.f };
	_float3         m_vRotAngle = { 0.f , 0.f , 0.f };
	_double         m_RotTime = { 1.0 };

	ROTATION      m_eRotation = { ROT_END };

	_bool         m_bTriggerTurnRoom = { false };
};

END