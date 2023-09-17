#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Train final : public CLevel
{
private:
	CLevel_Train(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Train() = default;

public:
	virtual _uint	Get_NextLevel() {
		return (_uint)LEVEL_LOADING;
	};

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT	Ready_Lights();
	HRESULT	Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Boss(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player_UI(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player_Battle_UI(const _tchar* pLayerTag);

private:
	HRESULT	Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag);	// �� ������Ʈ �Ľ�(�ȿ�)

	HRESULT Load_Lights_Info(const _tchar* pPath);

private:
	HRESULT	Ready_Layer_Effect();
	HRESULT LoadEffects(const _tchar* pPath);

private:
	void	Train_Sound(_double dTimeDelta);



private:
	_float	m_Battle_TimeAcc = { 0.f };
	_float	m_Battle_MaxTime = { 2.f };
	_float	m_Battle_TimeDir = { 1.f };

private:
	_double	m_dTrainSoundAccTime = { 100.0 };

	


public:
	static			CLevel_Train* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void	Free() override;
};

END