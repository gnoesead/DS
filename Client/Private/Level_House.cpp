#include "pch.h"
#include "..\Public\Level_House.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera.h"
#include "Player.h"
#include "Boss_Kyogai.h"
#include "MapObject.h"
#include "Fade.h"
#include "Fade_Manager.h"
#include "Mini_Map.h"
#include "Mission.h"
#include "Player_Battle_Frame.h"
#include "Player_Battle_Hp.h"
#include "Player_Battle_Mp.h"
#include "Boss_Battle_Frame.h"
#include "Boss_Battle_Hp.h"
#include "Player_Battle_Ult_Frame.h"
#include "Player_Battle_Combo.h"
#include "Player_Battle_Ult_Effect.h"
#include "FIcon.h"
#include "World_UI_Hp.h"
#include "Interaction.h"
#include "Dialog.h"
#include "Skill_Name.h"
#include "Battle_Signal.h"
#include "Pause.h"
#include "Option.h"

#include "ColliderManager.h"



CLevel_House::CLevel_House(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{

}

HRESULT CLevel_House::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Lights()))
    {
        MSG_BOX("Failed to Ready_Lights : CLevel_House");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
    {
        MSG_BOX("Failed to Ready_Layer_BackGround : CLevel_House");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
    {
        MSG_BOX("Failed to Ready_Layer_Camera : CLevel_House");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
    {
        MSG_BOX("Failed to Ready_Layer_Camera : CLevel_House");
        return E_FAIL;
    }

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	{
		MSG_BOX("Failed to Ready_Layer_Monster : CLevel_House");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Boss(TEXT("Layer_Boss"))))
	{
		MSG_BOX("Failed to Ready_Layer_Boss : CLevel_House");
		return E_FAIL;
	}

    if (FAILED(Ready_Layer_Player_UI(TEXT("Layer_Player_UI"))))
    {
        MSG_BOX("Failed to Ready_Layer_Camera : CLevel_House");
        return E_FAIL;
    }

	if (FAILED(Ready_Layer_Player_Battle_UI(TEXT("Layer_Player_Battle_UI"))))
	{
		MSG_BOX("Failed to Ready_Layer_Camera : CLevel_House");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Boss_Battle_UI(TEXT("Layer_Boss_Battle_UI"))))
	{
		MSG_BOX("Failed to Ready_Layer_Camera : CLevel_House");
		return E_FAIL;
	}

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
    {
        MSG_BOX("Failed to Ready_Layer_MapObject : CLevel_House");
        return E_FAIL;
    }


    CFadeManager::GetInstance()->Set_Fade_In(true);
	CFadeManager::GetInstance()->Set_Is_Battle(false);

    return S_OK;
}

void CLevel_House::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
    SetWindowText(g_hWnd, TEXT("House"));

	CColliderManager::GetInstance()->Check_Collider(LEVEL_HOUSE, dTimeDelta);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_DIKeyDown(DIK_F1))
    {
        CFadeManager::GetInstance()->Set_Fade_Out(true);
    }

    if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

        CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

        HRESULT hr = 0;

        if (true == pGameInstance->Get_IsStage())
        {

            if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_LOBBY))
            
                hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY), false, false);           
            else
                hr = pGameInstance->Swap_Level(LEVEL_LOBBY);

			pGameInstance->Clear_Light();

        }
    }

    Safe_Release(pGameInstance);
}

HRESULT CLevel_House::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_House::Ready_Lights()
{
    Load_Lights_Info(TEXT("../../Data/Light/Room/Light_Room.dat"));

    return S_OK;
}

HRESULT CLevel_House::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

  /*  if (FAILED(pGameInstance->Add_GameObject(Level_House, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
    {
        MSG_BOX("Failed to Add_GameObject : Terrain");
        return E_FAIL;
    }*/

    ///* For.Sky */
    //if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag,
    //    TEXT("Prototype_GameObject_Sky"))))
    //{
    //    MSG_BOX("Failed to Add_GameObject : Sky");
    //    return E_FAIL;
    //}

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_House::Ready_Layer_Camera(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCamera::CAMERADESC CameraDesc;
    ZeroMemory(&CameraDesc, sizeof CameraDesc);

    CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
    CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

    CameraDesc.fFovY = XMConvertToRadians(50.f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNearZ = 0.3f;
    CameraDesc.fFarZ = 300.f;

    CameraDesc.TransformDesc.dSpeedPerSec = 10.0;
    CameraDesc.TransformDesc.dRadianRotationPerSec = XMConvertToRadians(90.f);
    CameraDesc.dSensitivity = 0.1;
    //CameraDesc.bIs_Battle = false;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag,
        TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Camera_Free");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_House::Ready_Layer_Player(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

     CharacterDesc.WorldInfo.vPosition = _float4(8.f, 0.f, 10.f, 1.f);
    //CharacterDesc.WorldInfo.vPosition = _float4(118.f, 0.f, 117.f, 1.f);    // BattleMap

    CharacterDesc.Land_Y = 0.f;
    CharacterDesc.eCurNavi = CLandObject::NAVI_HOUSE_0_0;
   // CharacterDesc.eCurNavi = CLandObject::NAVI_HOUSE_4_0;                   // BattleMap

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag,
        TEXT("Prototype_GameObject_Player_Tanjiro"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : CLevel_House");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_House::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);

	//CBoss_Kyogai::CHARACTERDESC CharacterDesc;
	//ZeroMemory(&CharacterDesc, sizeof CharacterDesc);


	////CharacterDesc.WorldInfo.vPosition = _float4(140.f, 0.f, 120.f, 1.f);


	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag,
	//	TEXT("Prototype_GameObject_Monster_Kyogai"), &CharacterDesc)))
	//{
	//	MSG_BOX("Failed to Add_GameObject : CLevel_House");
	//	return E_FAIL;
	//}


	//Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_House::Ready_Layer_Boss(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CBoss_Kyogai::CHARACTERDESC CharacterDesc;
	ZeroMemory(&CharacterDesc, sizeof CharacterDesc);


	CharacterDesc.WorldInfo.vPosition = _float4(140.f, 0.f, 120.f, 1.f);


	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag,
		TEXT("Prototype_GameObject_Monster_Kyogai"), &CharacterDesc)))
	{
		MSG_BOX("Failed to Add_GameObject : CLevel_House");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_House::Ready_Layer_MapObject(const _tchar* pLayerTag)
{
    Load_MapObject_Info(TEXT("../../Data/Object/RoomMap/Room.dat"), pLayerTag);

    return S_OK;
}

HRESULT CLevel_House::Ready_Layer_Player_UI(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    // Fade
    CFade::UIDESC UIDesc;
    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


 // Mini_Map
    CMini_Map::UIDESC UIDesc2;
    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Bg
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Wire
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Frame
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Map
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Player_Icon
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


 // Mission

    CMission::UIDESC UIDesc3;
    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    // Main
    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    // Main_Icon
    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }



// Dialog
	CDialog::UIDESC UIDesc4;
	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	// Frame
	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Dialog"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	// Name_Frame
	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Dialog"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_House::Ready_Layer_Player_Battle_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Player_Battle_Frame
	CPlayer_Battle_Frame::UIDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 1;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 2;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Player_Battle_Hp
	CPlayer_Battle_Hp::UIDESC UIDesc2;
	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = true;
	UIDesc2.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = true;
	UIDesc2.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = true;
	UIDesc2.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
#pragma endregion

#pragma region Player_Battle_Mp

	CPlayer_Battle_Mp::UIDESC UIDesc3;
	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	for (int i = 0; i < 4; i++) {

		ZeroMemory(&UIDesc3, sizeof UIDesc3);

		UIDesc3.m_Is_Reverse = false;
		UIDesc3.m_Type = 3;
		UIDesc3.m_Scale_Type = i;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
			TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

	}

#pragma endregion

#pragma region Player_Battle_Ult_Frame

	CPlayer_Battle_Ult_Frame::UIDESC UIDesc4;
	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 4;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 5;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 7;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 9;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 10;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 11;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 12;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 13;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

#pragma endregion

#pragma region Player_Battle_Combo_Hit	
	CPlayer_Battle_Combo::UIDESC UIDesc5;
	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 0;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 1;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 2;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 3;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Player_Battle_Combo_Damage	
	CPlayer_Battle_Combo::UIDESC UIDesc6;
	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 0;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 1;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 2;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 3;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Player_Battle_Ult_Effect

	CPlayer_Battle_Ult_Effect::UIDESC UIDesc7;
	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 4;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 5;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

#pragma endregion


	// Skill_Name
	CSkill_Name::UIDESC UIDesc8;
	ZeroMemory(&UIDesc8, sizeof UIDesc8);

	UIDesc8.m_Is_Reverse = false;
	UIDesc8.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Skill_Name"), &UIDesc8))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


// Battle_Signal
	CBattle_Signal::UIDESC UIDesc9;
	ZeroMemory(&UIDesc9, sizeof UIDesc9);

	// ½ÃÀÛ
	UIDesc9.m_Is_Reverse = false;
	UIDesc9.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc9, sizeof UIDesc9);

	// ½ÂºÎ°áÁ¤
	UIDesc9.m_Is_Reverse = false;
	UIDesc9.m_Type = 7;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc9, sizeof UIDesc9);

	// ·©Å©
	UIDesc9.m_Is_Reverse = false;
	UIDesc9.m_Type = 0;
	UIDesc9.m_Rank = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc9, sizeof UIDesc9);

	// Frame
	UIDesc9.m_Is_Reverse = false;
	UIDesc9.m_Type = 1;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



// Pause
	CPause::UIDESC UIDesc10;
	ZeroMemory(&UIDesc10, sizeof UIDesc10);

	UIDesc10.m_Is_Reverse = false;
	UIDesc10.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Pause"), &UIDesc10))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc10, sizeof UIDesc10);

	UIDesc10.m_Is_Reverse = false;
	UIDesc10.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Pause"), &UIDesc10))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc10, sizeof UIDesc10);

	UIDesc10.m_Is_Reverse = false;
	UIDesc10.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Pause"), &UIDesc10))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc10, sizeof UIDesc10);

	UIDesc10.m_Is_Reverse = false;
	UIDesc10.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Pause"), &UIDesc10))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc10, sizeof UIDesc10);

	UIDesc10.m_Is_Reverse = false;
	UIDesc10.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Pause"), &UIDesc10))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



// Option
	COption::UIDESC UIDesc11;
	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 3;
	UIDesc11.m_Line_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 3;
	UIDesc11.m_Line_Num = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 3;
	UIDesc11.m_Line_Num = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 3;
	UIDesc11.m_Line_Num = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 3;
	UIDesc11.m_Line_Num = 4;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 7;
	UIDesc11.m_Menu_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 7;
	UIDesc11.m_Menu_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 7;
	UIDesc11.m_Menu_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 9;
	UIDesc11.m_Cloud_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 10;
	UIDesc11.m_Cloud_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 11;
	UIDesc11.m_Is_X_Reverse = true;
	UIDesc11.m_Is_Y_Reverse = true;
	UIDesc11.m_Cloud_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 12;
	UIDesc11.m_Is_X_Reverse = true;
	UIDesc11.m_Is_Y_Reverse = true;
	UIDesc11.m_Cloud_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 13;
	UIDesc11.m_Cloud_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 14;
	UIDesc11.m_Cloud_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 15;
	UIDesc11.m_Is_X_Reverse = true;
	UIDesc11.m_Is_Y_Reverse = true;
	UIDesc11.m_Cloud_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 16;
	UIDesc11.m_Is_Y_Reverse = true;
	UIDesc11.m_Cloud_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 17;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 18;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc11, sizeof UIDesc11);

	UIDesc11.m_Type = 19;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc11))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}














	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_House::Ready_Layer_Boss_Battle_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


#pragma region Boss_Battle_Frame
	CBoss_Battle_Frame::UIDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 1;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 2;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Boss_Battle_Hp

	CBoss_Battle_Hp::UIDESC UIDesc2;
	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_House::Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    HANDLE hFile = CreateFile(pPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    _ulong			dwByte = 0;
    _ulong			dwStrByte = 0;
    _uint			iSize = 0;

    CGameObject* pGameObject = { nullptr };

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    _uint iLevelIdx = pGameInstance->Get_CurLevelIdx();

    for (_uint i = 0; i < iSize; ++i)
    {
        CMapObject::MAPOBJECT_INFO tMapObject_Info;
        ZeroMemory(&tMapObject_Info, sizeof tMapObject_Info);

        ReadFile(hFile, &tMapObject_Info.vPos, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.vRotAngle, sizeof(_float3), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.vScale, sizeof(_float3), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iMapObjectType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iNumInstance, sizeof(_uint), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fRange, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fMinSize, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fMaxSize, sizeof(_float), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iInstanceType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.bRandomRatationY, sizeof(_bool), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iArrangementType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iSceneType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iRenderGroup, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iInteractionType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.szMeshName, dwStrByte, &dwByte, nullptr);

        const _tchar* pMapObjectTypeTag = TEXT("");

        switch (tMapObject_Info.iMapObjectType)
        {
        case CMapObject::MAPOBJECT_STATIC:
            if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_StaticMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_TERRAIN:
            if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_TerrainMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_ROTATION:
            if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_RotationMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_INSTANCE:
            if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_InstanceMapObject"), &tMapObject_Info)))
                return E_FAIL;
        default:
            break;
        }
    }

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_House::Load_Lights_Info(const _tchar* pPath)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    HANDLE hFile = CreateFile(pPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    _ulong			dwByte = 0;
    _uint			iSize = 0;

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iSize; ++i)
    {
        LIGHTDESC tLight;
        ZeroMemory(&tLight, sizeof tLight);


        ReadFile(hFile, &tLight.eType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tLight.fLightRange, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tLight.vLightAmbient, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tLight.vLightDiffuse, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tLight.vLightSpecular, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tLight.vLightDir, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tLight.vLightPos, sizeof(_float4), &dwByte, nullptr);

        if (tLight.eType == LIGHTDESC::TYPE_DIRECTION)
        {
            tLight.vLightDiffuse = _float4(0.05f, 0.05f, 0.05f, 1.f);
        }


        if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, tLight)))
            return E_FAIL;
    }

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}

CLevel_House* CLevel_House::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_House* pInstance = new CLevel_House(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_House");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_House::Free()
{
    __super::Free();
}
