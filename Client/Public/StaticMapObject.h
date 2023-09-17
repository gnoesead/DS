#pragma once

#include "Client_Defines.h"
#include "MapObject.h"
#include "Mesh.h"

BEGIN(Client)

class CStaticMapObject  final : public CMapObject
{
private:
	CStaticMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticMapObject(const CStaticMapObject& rhs);
	virtual ~CStaticMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CStaticMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	void	Interaction_DoorOpen_Manual(_double TimeDelta);		// 문 열림 수동
	void	Interaction_DoorOpen_Auto(_double TimeDelta);		// 문 열림 자동

	void	Room_Change(_double TimeDelta, _uint iInteractionType);
	void	Control_RenderSmell(_double TimeDelta);

	// 카메라 검사
	void	Check_Camera();
	_bool	Is_Blocked_Camera(HWND hWnd, const _uint& iWinSizeX, const _uint& iWinSizeY, class CMesh* pMapObjectMesh, _fvector vCamPos , _fvector vPlayerPos);

private:
	_bool m_bKeyInput = { false };

	_double  m_AccTime = { 0.f };

	// 문 열림
	_bool	 m_bOpen = { false };
	_bool	 m_bStop = { false };

	// 방 이동
	_bool	m_bChageRoomRealTrigger = { false };
	_bool	m_bChangeRoomTrigger = { false };
	_bool	m_bChageRoom = { false };
	_bool	m_bSetInvert = { false };
	// 냄새
	_bool	m_bSmellOn = { false };

	// 북소리
	_bool	m_bBookSound = { false };
	
};

END
