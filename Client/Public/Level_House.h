#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_House final : public CLevel
{
private:
	CLevel_House(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_House() = default;

public:
	virtual _uint	Get_NextLevel() {
		return (_uint)LEVEL_LOADING;
	};

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_bool bChangeBattleBGM = { false };
	_bool bChangeBossBGM = { false };

private:
	HRESULT	Ready_Lights();
	HRESULT	Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_StealthObj(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Boss(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player_UI(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Player_Battle_UI(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Boss_Battle_UI(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_SmellBundle(const _tchar* pLayerTag);

	HRESULT	Ready_Layer_Character_Dialog(const _tchar* pLayerTag);


private:
	HRESULT	Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag);	// ¸Ê ¿ÀºêÁ§Æ® ÆÄ½Ì(¾È¿ø)
	HRESULT Load_Lights_Info(const _tchar* pPath);

private:
	HRESULT	Ready_Layer_Effect();
	HRESULT LoadEffects(const _tchar* pPath);


public:
	static			CLevel_House* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void	Free() override;
};

END