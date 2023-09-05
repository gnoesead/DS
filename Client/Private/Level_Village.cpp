#include "pch.h"
#include "..\Public\Level_Village.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ColliderManager.h"

#include "Camera.h"
#include "Player.h"
#include "MapObject.h"

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
#include "Mission.h"
#include "Mini_Map.h"
#include "CollisionBox.h"
#include "Skill_Name.h"
#include "Battle_Signal.h"
#include "Character.h"

#include "Pause.h"
#include "Fade.h"
#include "Fade_Manager.h"
#include "Option.h"

#include "PlayerManager.h"
#include "OptionManager.h"
#include "MissionManager.h"
#include "SmellBundle.h"

#include "ParticleSystem.h"
#include "Effect_Texture.h"
#include "EffectPlayer.h"


CLevel_Village::CLevel_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Village::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    COptionManager::GetInstance()->Set_Is_Set_Origin_Light(false);

    CPlayerManager::GetInstance()->Reset_PlayerManager();

    if (FAILED(Ready_Lights()))
    {
        MSG_BOX("Failed to Ready_Lights : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
    {
        MSG_BOX("Failed to Ready_Layer_BackGround : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
    {
        MSG_BOX("Failed to Ready_Layer_Player : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
    {
        MSG_BOX("Failed to Ready_Layer_Monster : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Boss(TEXT("Layer_Boss"))))
    {
        MSG_BOX("Failed to Ready_Layer_Boss : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
    {
        MSG_BOX("Failed to Ready_Layer_Boss : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Player_UI(TEXT("Layer_Player_UI"))))
    {
        MSG_BOX("Failed to Ready_Layer_Player_UI : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Player_Battle_UI(TEXT("Layer_Player_Battle_UI"))))
    {
        MSG_BOX("Failed to Ready_Player_Battle_UI : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Boss_Battle_UI(TEXT("Layer_Boss_Battle_UI"))))
    {
        MSG_BOX("Failed to Ready_Boss_Battle_UI : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
    {
        MSG_BOX("Failed to Ready_Layer_MapObject : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_CollisionBox(TEXT("Layer_CollisionBox"))))
    {
        MSG_BOX("Failed to Ready_Layer_CollisionBox : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
    {
        MSG_BOX("Failed to Ready_Layer_Camera : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_SmellBundle(TEXT("Layer_SmellBundle"))))
    {
        MSG_BOX("Failed to Ready_Layer_Camera : CLevel_Village");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_Effect()))
    {
        MSG_BOX("Failed to Ready_Layer_Effect : CLevel_Village");
        return E_FAIL;
    }

    CFadeManager::GetInstance()->Set_Fade_In(true);
    CFadeManager::GetInstance()->Set_Is_Battle(false);

    CMissionManager::GetInstance()->Set_Main_Mission_Type(0);
    CMissionManager::GetInstance()->Set_Sub_Mission_Type(0);

    CFadeManager::GetInstance()->Set_Is_House_Monster_Encounter(false);
    CFadeManager::GetInstance()->Set_Is_House_Boss_Encounter(false);

    CFadeManager::GetInstance()->Set_Is_Village_Battle_Start(false);
    CFadeManager::GetInstance()->Set_Is_House_Monster_Battle_Start(false);
    CFadeManager::GetInstance()->Set_Is_House_Boss_Battle_Start(false);
    CFadeManager::GetInstance()->Set_Is_Train_Battle_Start(false);
    CFadeManager::GetInstance()->Set_Is_Final_Battle_Start(false);




    return S_OK;
}

void CLevel_Village::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);

    SetWindowText(g_hWnd, TEXT("Village"));

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CColliderManager::GetInstance()->Check_Collider(LEVEL_VILLAGE, dTimeDelta);

    if (pGameInstance->Get_DIKeyState(DIK_PGUP))
    {
        _float4 vAmbient = pGameInstance->Get_Light(0)->vLightAmbient;

        vAmbient.x += 0.05f;
        vAmbient.y += 0.05f;
        vAmbient.z += 0.05f;

        pGameInstance->Set_Light(0, 2, vAmbient);
    }

    if (pGameInstance->Get_DIKeyState(DIK_PGDN))
    {
        _float4 vAmbient = pGameInstance->Get_Light(0)->vLightDiffuse;

        vAmbient.x -= 0.05f;
        vAmbient.y -= 0.05f;
        vAmbient.y -= 0.05f;

        pGameInstance->Set_Light(0, 2, vAmbient);
    }
   
    
   

    if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

        CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

        HRESULT hr = 0;

        if (true == pGameInstance->Get_IsStage())
        {

            if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_LOBBY))
            {
                pGameInstance->Clear_Light();
                hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY), false, false);
            }
            else
                hr = pGameInstance->Swap_Level(LEVEL_LOBBY);


        }
    }



    Safe_Release(pGameInstance);

   
}

HRESULT CLevel_Village::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Lights()
{
    Load_Lights_Info(TEXT("../../Data/Light/Village/Light_Village.dat"));

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /* For.Sky */
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag,
        TEXT("Prototype_GameObject_Sky"))))
    {
        MSG_BOX("Failed to Add_GameObject : Sky");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Camera(const _tchar* pLayerTag)
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
    CameraDesc.fFarZ = 400.f;

    CameraDesc.TransformDesc.dSpeedPerSec = 10.0;
    CameraDesc.TransformDesc.dRadianRotationPerSec = XMConvertToRadians(90.f);
    CameraDesc.dSensitivity = 0.1;
 

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, 
        TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Camera_Free");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Player(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCharacter::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    CharacterDesc.WorldInfo.vPosition = _float4(573.f, 4.5f, 242.f, 1.f);
   // CharacterDesc.WorldInfo.vPosition = _float4(426.55f, 3.0f, 301.92f, 1.f); // BattleMap

    CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_MAINROAD1;
   // CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_BATTLE;// BattleMap
 
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, 
        TEXT("Prototype_GameObject_Player_Tanjiro"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : CLevel_Village");
        return E_FAIL;
    }
   

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Monster(const _tchar* pLayerTag)
{

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_BATTLE; //abcde

    CharacterDesc.WorldInfo.vPosition = _float4(426.f, 3.f, 290.f, 1.f);

    /*
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag,
        TEXT("Prototype_GameObject_Monster_Zako_0"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Monster_Zako_0");
        return E_FAIL;
    }
    */
    
    CharacterDesc.WorldInfo.vPosition = _float4(429.7f, 3.35f, 292.0f, 1.f);
    CharacterDesc.SwampHorn = 1;
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag,
        TEXT("Prototype_GameObject_Monster_Swamp"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Monster_Swamp");
        return E_FAIL;
    }
    CharacterDesc.WorldInfo.vPosition = _float4(424.57f, 3.35f, 291.7f, 1.f);
    CharacterDesc.SwampHorn = 2;
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag,
        TEXT("Prototype_GameObject_Monster_Swamp"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Monster_Swamp");
        return E_FAIL;
    }
    CharacterDesc.WorldInfo.vPosition = _float4(419.5f, 3.35f, 290.7f, 1.f);
    CharacterDesc.SwampHorn = 3;
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag,
        TEXT("Prototype_GameObject_Monster_Swamp"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Monster_Swamp");
        return E_FAIL;
    }
    

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Boss(const _tchar* pLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_NPC(const _tchar* pLayerTag)
{
    Ready_Layer_NPC_FirstStreet(pLayerTag);
    Ready_Layer_NPC_SecondStreet(pLayerTag);

    Ready_Layer_NPC_Inside(pLayerTag);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_NPC_FirstStreet(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_MAINROAD1; //abcde


    /* //�ʼ����� : ��ġ, ����, ����
    CharacterDesc.WorldInfo.vPosition = _float4(573.2f, 4.55f, 256.4f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, 1.0f, 0.0f }));

    //walk ���� �̵� ���� 3����
    CharacterDesc.NPCDesc.WalkSpot[0] = { 572.8f, 4.55f, 258.05f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 573.95f, 4.55f, 276.12f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 583.37f, 4.55f, 276.15f, 1.f };

    //UI����
    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;
    */

    CharacterDesc.NPCDesc.iSection = 1;

#pragma region WalkNPC
    CharacterDesc.WorldInfo.vPosition = _float4(573.2f, 4.55f, 256.4f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, 1.0f, 0.0f }));

    CharacterDesc.NPCDesc.WalkSpot[0] = { 572.8f, 4.55f, 258.05f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 573.95f, 4.55f, 276.12f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 583.37f, 4.55f, 276.15f, 1.f };

    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(574.5f, 4.55f, 286.76f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    CharacterDesc.NPCDesc.WalkSpot[0] = { 572.3f, 4.55f, 280.12f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 570.13f, 4.55f, 274.04f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 568.34f, 4.55f, 255.33f, 1.f };
    
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(570.71f, 4.55f, 263.13f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    CharacterDesc.NPCDesc.WalkSpot[0] = { 567.75f, 4.55f, 265.6f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 574.3f, 4.55f, 263.28f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 569.23f, 4.55f, 263.5f, 1.f };
    
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }
#pragma endregion


    CharacterDesc.NPCDesc.Icon_Type = 3;
    CharacterDesc.NPCDesc.Dialog_Type = 3;
    CharacterDesc.NPCDesc.Interaction = true;

#pragma region NPC
    CharacterDesc.WorldInfo.vPosition = _float4(565.6f, 4.55f, 255.37f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_SIT;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(565.63f, 4.55f, 254.53f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_SITTALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(565.57f, 4.55f, 262.28f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_STAND;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(575.7f, 4.55f, 267.25f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_STAND;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(565.7f, 4.55f, 270.24f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_TALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(567.1f, 4.55f, 271.42f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_LISTEN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(566.5f, 4.55f, 268.9f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_TALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(574.63f, 4.55f, 268.4f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_TALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(575.76f, 4.55f, 260.34f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WORK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));

    CharacterDesc.NPCDesc.Icon_Type = 4;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(579.2f, 4.55f, 280.76f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(566.57f, 4.55f, 262.86f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_CRY;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(567.98f, 4.55f, 263.42f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_CRY;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(567.19f, 4.55f, 267.7f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_LISTEN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(580.32f, 4.55f, 280.04f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_TALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }
#pragma endregion


    Safe_Release(pGameInstance);
    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_NPC_SecondStreet(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_MAINROAD1; //abcde

    /* //�ʼ����� : ��ġ, ����, ����
    CharacterDesc.WorldInfo.vPosition = _float4(573.2f, 4.55f, 256.4f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, 1.0f, 0.0f }));

    //walk ���� �̵� ���� 3����
    CharacterDesc.NPCDesc.WalkSpot[0] = { 572.8f, 4.55f, 258.05f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 573.95f, 4.55f, 276.12f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 583.37f, 4.55f, 276.15f, 1.f };

    //UI����
    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;
    */

    CharacterDesc.NPCDesc.iSection = 2;

    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;

#pragma region WalkNPC

    CharacterDesc.WorldInfo.vPosition = _float4(603.07f, 4.55f, 261.13f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    CharacterDesc.NPCDesc.WalkSpot[0] = { 601.12f, 4.55f, 262.73f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 600.12f, 4.55f, 276.18f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 597.06f, 4.55f, 260.95f, 1.f };
 
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(603.43f, 4.55f, 258.01f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    CharacterDesc.NPCDesc.WalkSpot[0] = { 600.12f, 4.55f, 260.73f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 599.12f, 4.55f, 274.18f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 596.06f, 4.55f, 258.95f, 1.f };
    
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }
#pragma endregion


#pragma region Second Street
    CharacterDesc.WorldInfo.vPosition = _float4(605.33f, 4.55f, 271.04f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_SITTALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(605.30f, 4.55f, 270.00f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_SITTALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    CharacterDesc.NPCDesc.Icon_Type = 4;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(603.5f, 4.55f, 276.5f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WORK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));

    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(595.68f, 4.55f, 268.16f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WORK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(608.11f, 4.55f, 273.22f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


   
#pragma endregion



    CharacterDesc.NPCDesc.iSection = 3;
#pragma region Third Street

    CharacterDesc.WorldInfo.vPosition = _float4(607.18f, 4.55f, 288.64f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_STAND;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    CharacterDesc.NPCDesc.Icon_Type = 4;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildM");
        return E_FAIL;
    }




    CharacterDesc.WorldInfo.vPosition = _float4(604.07f, 4.55f, 305.43f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWNTALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 1.0f, 0.0f }));

    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }

    CharacterDesc.WorldInfo.vPosition = _float4(607.27f, 4.55f, 309.7f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWNTALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(607.4f, 4.55f, 304.0f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWNTALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, 1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(601.7f, 4.55f, 304.6f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(608.55f, 4.55f, 307.5f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_DOWN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }

#pragma endregion



    Safe_Release(pGameInstance);
    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_NPC_Inside(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_MAINROAD1; //abcde

    /* //�ʼ����� : ��ġ, ����, ����
    CharacterDesc.WorldInfo.vPosition = _float4(573.2f, 4.55f, 256.4f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, 1.0f, 0.0f }));

    //walk ���� �̵� ���� 3����
    CharacterDesc.NPCDesc.WalkSpot[0] = { 572.8f, 4.55f, 258.05f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[1] = { 573.95f, 4.55f, 276.12f, 1.f };
    CharacterDesc.NPCDesc.WalkSpot[2] = { 583.37f, 4.55f, 276.15f, 1.f };

    //UI����
    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;
    */

    CharacterDesc.NPCDesc.iSection = 4;

    CharacterDesc.NPCDesc.Icon_Type = 99;
    CharacterDesc.NPCDesc.Dialog_Type = 99;
    CharacterDesc.NPCDesc.Interaction = false;


    CharacterDesc.WorldInfo.vPosition = _float4(592.8f, 4.55f, 287.32f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_WORK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_AdultM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_AdultM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(587.63f, 4.55f, 294.0f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_TALK;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(589.28f, 4.55f, 293.9f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_LISTEN;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_Female"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_Female");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(593.75f, 4.55f, 288.6f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_CRY;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(590.64f, 4.55f, 285.37f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_STAND;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildM");
        return E_FAIL;
    }


    CharacterDesc.WorldInfo.vPosition = _float4(587.9f, 4.55f, 301.7f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_SIT;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildF"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildF");
        return E_FAIL;
    }



    CharacterDesc.WorldInfo.vPosition = _float4(587.1f, 4.55f, 301.66f, 1.f);
    CharacterDesc.NPCDesc.eNPC = CCharacter::NPC_SIT;
    XMStoreFloat4(&CharacterDesc.NPCDesc.DirNPC, XMVector4Normalize(_vector{ -1.0f, 0.0f, -1.0f, 0.0f }));

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_NPC_ChildM"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : NPC_ChildM");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);
    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_MapObject(const _tchar* pLayerTag)
{
    Load_MapObject_Info(TEXT("../../Data/Object/Village/Village.dat"), pLayerTag);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_CollisionBox(const _tchar* pLayerTag)
{
    Load_CollisionBox_Info(TEXT("../../Data/Collision/Village/Village_Collision.dat"), pLayerTag);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Player_UI(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);


 // Fade
    CFade::UIDESC UIDesc11;
    ZeroMemory(&UIDesc11, sizeof UIDesc11);

    UIDesc11.m_Is_Reverse = false;
    UIDesc11.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc11))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc11, sizeof UIDesc11);

    UIDesc11.m_Is_Reverse = false;
    UIDesc11.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc11))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc11, sizeof UIDesc11);

    UIDesc11.m_Is_Reverse = false;
    UIDesc11.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc11))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

// Dialog
    CDialog::UIDESC UIDesc10;
    ZeroMemory(&UIDesc10, sizeof UIDesc10);

    // Frame
    UIDesc10.m_Is_Reverse = false;
    UIDesc10.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Dialog"), &UIDesc10))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc10, sizeof UIDesc10);

    // Name_Frame
    UIDesc10.m_Is_Reverse = false;
    UIDesc10.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Dialog"), &UIDesc10))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    //ZeroMemory(&UIDesc10, sizeof UIDesc10);

    //// Arrow
    //UIDesc10.m_Is_Reverse = false;
    //UIDesc10.m_Type = 2;

    //if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
    //    TEXT("Prototype_GameObject_Dialog"), &UIDesc10))) {
    //    Safe_Release(pGameInstance);
    //    return E_FAIL;
    //}

// Mission

    CMission::UIDESC UIDesc;
    ZeroMemory(&UIDesc, sizeof UIDesc);

    // Main
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    // Sub
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    
    ZeroMemory(&UIDesc, sizeof UIDesc);

    // Main_Icon
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    // Sub_Icon
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    // Sub_Check
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    // Eff_1
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 5;
    UIDesc.m_Eff_Type = 0;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    // Eff_2
    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 5;
    UIDesc.m_Eff_Type = 1;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mission"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


// Mini_Map
    CMini_Map::UIDESC UIDesc2;
    ZeroMemory(&UIDesc2, sizeof UIDesc2);
   
    // Bg
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Wire
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Frame
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Map
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    // Player_Icon
    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Mini_Map"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

// Pause
    CPause::UIDESC UIDesc3;
    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Pause"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 1;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Pause"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 2;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Pause"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 3;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Pause"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 6;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Pause"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

 // Option
    COption::UIDESC UIDesc4;
    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 3;
    UIDesc4.m_Line_Num = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 3;
    UIDesc4.m_Line_Num = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 3;
    UIDesc4.m_Line_Num = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 3;
    UIDesc4.m_Line_Num = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 3;
    UIDesc4.m_Line_Num = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    for (int i = 0; i < 5; i++) {

        ZeroMemory(&UIDesc4, sizeof UIDesc4);

        UIDesc4.m_Type = 4;
        UIDesc4.m_Line_Num = i;
        UIDesc4.m_Arrow_Type = 0;
        UIDesc4.m_Is_X_Reverse = true;

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }

        ZeroMemory(&UIDesc4, sizeof UIDesc4);

        UIDesc4.m_Type = 4;
        UIDesc4.m_Line_Num = i;
        UIDesc4.m_Arrow_Type = 1;
        UIDesc4.m_Is_X_Reverse = false;

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }
    }

    for (int i = 0; i < 5; i++) {

        ZeroMemory(&UIDesc4, sizeof UIDesc4);

        UIDesc4.m_Type = 5;
        UIDesc4.m_Line_Num = i;
     
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }

        ZeroMemory(&UIDesc4, sizeof UIDesc4);

        UIDesc4.m_Type = 6;
        UIDesc4.m_Line_Num = i;
      
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }
    }


    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 7;
    UIDesc4.m_Menu_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 7;
    UIDesc4.m_Menu_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 7;
    UIDesc4.m_Menu_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 9;
    UIDesc4.m_Cloud_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 10;
    UIDesc4.m_Cloud_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 11;
    UIDesc4.m_Is_X_Reverse = true;
    UIDesc4.m_Is_Y_Reverse = true;
    UIDesc4.m_Cloud_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 12;
    UIDesc4.m_Is_X_Reverse = true;
    UIDesc4.m_Is_Y_Reverse = true;
    UIDesc4.m_Cloud_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 13;
    UIDesc4.m_Cloud_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 14;
    UIDesc4.m_Cloud_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 15;
    UIDesc4.m_Is_X_Reverse = true;
    UIDesc4.m_Is_Y_Reverse = true;
    UIDesc4.m_Cloud_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 16;
    UIDesc4.m_Is_Y_Reverse = true;
    UIDesc4.m_Cloud_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 17;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 18;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Type = 19;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

 // Interaction
    CInteraction::UIDESC UIDesc5;
    ZeroMemory(&UIDesc5, sizeof UIDesc5);

    UIDesc5.m_Type = 1;
    UIDesc5.Pos = { 598.f, 4.5f, 311.f , 1.f };
    UIDesc5.m_Up_Mount = 1.2f;
    UIDesc5.m_Is_Smell = true;
    UIDesc5.m_Dialog_Type = 11;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Interaction"), &UIDesc5))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc5, sizeof UIDesc5);

    UIDesc5.m_Type = 1;
    UIDesc5.Pos = { 607.f, 7.5f, 255.f , 1.f };
    UIDesc5.m_Up_Mount = 1.2f;
    UIDesc5.m_Is_Smell = true;
    UIDesc5.m_Dialog_Type = 12;
   
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Interaction"), &UIDesc5))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc5, sizeof UIDesc5);

    UIDesc5.m_Type = 1;
    UIDesc5.Pos = { 586.f, 4.5f, 351.f , 1.f };
    UIDesc5.m_Up_Mount = 1.2f;
    UIDesc5.m_Is_Smell = true;
    UIDesc5.m_Dialog_Type = 13;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Interaction"), &UIDesc5))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

 // Icon
    CFIcon::UIDESC UIDesc6;
    ZeroMemory(&UIDesc6, sizeof UIDesc6);

    UIDesc6.m_Type = 0;
    UIDesc6.Pos = { 600.3f, 4.5f, 318.37f , 1.f };
    UIDesc6.m_Up_Mount = 1.7f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_FIcon"), &UIDesc6))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

   

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Player_Battle_UI(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

#pragma region Player_Battle_Frame
    CPlayer_Battle_Frame::UIDESC UIDesc;
    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 1;
    UIDesc.m_C_Num = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 2;
    UIDesc.m_C_Num = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = true;
    UIDesc2.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = true;
    UIDesc2.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = true;
    UIDesc2.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    for (int i = 0; i < 4; i++) {

        ZeroMemory(&UIDesc3, sizeof UIDesc3);

        UIDesc3.m_Is_Reverse = false;
        UIDesc3.m_Type = 3;
        UIDesc3.m_Scale_Type = i;

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 5;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 6;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 7;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 9;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 10;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 11;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 12;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc4, sizeof UIDesc4);

    UIDesc4.m_Is_Reverse = false;
    UIDesc4.m_Type = 13;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc5, sizeof UIDesc5);

    UIDesc5.m_Is_Reverse = false;
    UIDesc5.m_Type = 1;
    UIDesc5.m_Combo_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc5, sizeof UIDesc5);

    UIDesc5.m_Is_Reverse = false;
    UIDesc5.m_Type = 2;
    UIDesc5.m_Combo_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc5, sizeof UIDesc5);

    UIDesc5.m_Is_Reverse = false;
    UIDesc5.m_Type = 3;
    UIDesc5.m_Combo_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc6, sizeof UIDesc6);

    UIDesc6.m_Is_Reverse = false;
    UIDesc6.m_Type = 1;
    UIDesc6.m_Combo_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc6, sizeof UIDesc6);

    UIDesc6.m_Is_Reverse = false;
    UIDesc6.m_Type = 2;
    UIDesc6.m_Combo_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc6, sizeof UIDesc6);

    UIDesc6.m_Is_Reverse = false;
    UIDesc6.m_Type = 3;
    UIDesc6.m_Combo_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc7, sizeof UIDesc7);

    UIDesc7.m_Is_Reverse = false;
    UIDesc7.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc7, sizeof UIDesc7);

    UIDesc7.m_Is_Reverse = false;
    UIDesc7.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc7, sizeof UIDesc7);

    UIDesc7.m_Is_Reverse = false;
    UIDesc7.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc7, sizeof UIDesc7);

    UIDesc7.m_Is_Reverse = false;
    UIDesc7.m_Type = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc7, sizeof UIDesc7);

    UIDesc7.m_Is_Reverse = false;
    UIDesc7.m_Type = 5;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc7, sizeof UIDesc7);

    UIDesc7.m_Is_Reverse = false;
    UIDesc7.m_Type = 6;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

#pragma endregion

    // skill_name
    CSkill_Name::UIDESC UIDesc8;
    ZeroMemory(&UIDesc8, sizeof UIDesc8);

    UIDesc8.m_Is_Reverse = false;
    UIDesc8.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Skill_Name"), &UIDesc8))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


    // Battle_Signal
    CBattle_Signal::UIDESC UIDesc9;
    ZeroMemory(&UIDesc9, sizeof UIDesc9);

    // ����
    UIDesc9.m_Is_Reverse = false;
    UIDesc9.m_Type = 6;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc9, sizeof UIDesc9);

    // �ºΰ���
    UIDesc9.m_Is_Reverse = false;
    UIDesc9.m_Type = 7;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc9, sizeof UIDesc9);

    // ��ũ
    UIDesc9.m_Is_Reverse = false;
    UIDesc9.m_Type = 0;
    UIDesc9.m_Rank = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc9, sizeof UIDesc9);

    // Frame
    UIDesc9.m_Is_Reverse = false;
    UIDesc9.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_Battle_Signal"), &UIDesc9))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


 // FIcon 
    CFIcon::UIDESC UIDesc10;
    // ���� ������
    ZeroMemory(&UIDesc10, sizeof UIDesc10);

    UIDesc10.m_Is_Reverse = false;
    UIDesc10.m_Type = 7;
    UIDesc10.m_Up_Mount = 2.1f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_FIcon"), &UIDesc10))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    
    // ���� �۷ο�
    ZeroMemory(&UIDesc10, sizeof UIDesc10);

    UIDesc10.m_Is_Reverse = false;
    UIDesc10.m_Type = 8;
    UIDesc10.m_Up_Mount = 2.1f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_FIcon"), &UIDesc10))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }



    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Boss_Battle_UI(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);


#pragma region Boss_Battle_Frame
    CBoss_Battle_Frame::UIDESC UIDesc;
    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
        TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 1;
    UIDesc.m_C_Num = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
        TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 2;
    UIDesc.m_C_Num = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
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

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
        TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
        TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
        TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Boss_UI"),
        TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

 // Monster_Hp
   /* CWorld_UI_Hp::UIDESC UIDesc3;
    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 0;
    UIDesc3.m_Monster_Index = 0;
    UIDesc3.m_Up_Mount = 1.7f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 1;
    UIDesc3.m_Monster_Index = 0;
    UIDesc3.m_Up_Mount = 1.7f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 2;
    UIDesc3.m_Monster_Index = 0;
    UIDesc3.m_Up_Mount = 1.7f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 3;
    UIDesc3.m_Monster_Index = 0;
    UIDesc3.m_Up_Mount = 1.7f;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }*/

#pragma endregion

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_SmellBundle(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    _uint iLevelIdx = pGameInstance->Get_CurLevelIdx();

    CSmellBundle::EFFECTDESC EffectDesc;


    EffectDesc.vPos = XMVectorSet(598.f, 4.5f, 311.f, 1.f);
    EffectDesc.eType = CSmellBundle::TYPE_NORMAL;

    if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, pLayerTag,
        TEXT("Prototype_GameObject_SmellBundle"), &EffectDesc)))
        return E_FAIL;

    EffectDesc.vPos = XMVectorSet(607.f, 7.5f, 255.f, 1.f);
    EffectDesc.eType = CSmellBundle::TYPE_NORMAL;

    if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, pLayerTag,
        TEXT("Prototype_GameObject_SmellBundle"), &EffectDesc)))
        return E_FAIL;

    EffectDesc.vPos = XMVectorSet(586.f, 4.5f, 351.f, 1.f);
    EffectDesc.eType = CSmellBundle::TYPE_NORMAL;

    if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, pLayerTag,
        TEXT("Prototype_GameObject_SmellBundle"), &EffectDesc)))
        return E_FAIL;

    EffectDesc.eType = CSmellBundle::TYPE_WORLD;

    if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, pLayerTag,
        TEXT("Prototype_GameObject_SmellBundle"), &EffectDesc)))
        return E_FAIL;

    Safe_Release(pGameInstance);
    return S_OK;
}

HRESULT CLevel_Village::Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag)
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
        case CMapObject::MAPOBJECT_INSTANCE:
            if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_InstanceMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        default:
            break;
        }
    }

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Load_CollisionBox_Info(const _tchar* pPath, const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    HANDLE hFile = CreateFile(pPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    _ulong			dwByte = 0;
    _uint			iSize = 0;

    CGameObject* pGameObject = { nullptr };

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    _uint iLevelIdx = pGameInstance->Get_CurLevelIdx();

    for (_uint i = 0; i < iSize; ++i)
    {
        CCollisionBox::COLLISIONBOX_INFO tCollisionBox_Info;
        ZeroMemory(&tCollisionBox_Info, sizeof tCollisionBox_Info);

        ReadFile(hFile, &tCollisionBox_Info.vPos, sizeof(_float4), &dwByte, nullptr);
      
        ReadFile(hFile, &tCollisionBox_Info.vScale, sizeof(_float3), &dwByte, nullptr);

        ReadFile(hFile, &tCollisionBox_Info.iCollisionType, sizeof(_uint), &dwByte, nullptr);
     
        if (FAILED(pGameInstance->Add_GameObject(iLevelIdx, TEXT("Layer_CollisionBox"),
            TEXT("Prototype_GameObject_CollisionBox"), &tCollisionBox_Info)))
            return E_FAIL;
    }

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Load_Lights_Info(const _tchar* pPath)
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
            tLight.vLightDiffuse = _float4(0.4f ,  0.4f , 0.4f , 1.f);
        }
           

        if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, tLight)))
            return E_FAIL;
    } 

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Effect()
{
    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Atk_5.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Atk_5");
        return E_FAIL;
    }

    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Atk_10.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Atk_10");
        return E_FAIL;
    }

    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Atk_11.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Atk_11");
        return E_FAIL;
    }

    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Atk_12.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Atk_12");
        return E_FAIL;
    }

    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Atk_13.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Atk_13");
        return E_FAIL;
    }

    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Atk_22.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Atk_22");
        return E_FAIL;
    }

    if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_Stone.bin"))))
    {
        MSG_BOX("Failed to Load Effect : Swamp_Stone");
        return E_FAIL;
    }

  

    return S_OK;
}

//HRESULT CLevel_Village::LoadEffects(const _tchar* pPath, _bool isParticle, _int iCnt)
//{
//    char szFilePathChar[MAX_PATH] = "";
//    WideCharToMultiByte(CP_ACP, 0, pPath, MAX_PATH, szFilePathChar, MAX_PATH, nullptr, nullptr);
//
//    std::ifstream inputFile(szFilePathChar, std::ios::binary);
//
//    if (!inputFile)
//    {
//        MSG_BOX("������ �� �� �����ϴ�.");
//        return E_FAIL;
//    }
//
//    CGameInstance* pGameInstance = CGameInstance::GetInstance();
//    Safe_AddRef(pGameInstance);
//
//    CEffectPlayer* pEffectPlayer = CEffectPlayer::Get_Instance();
//
//    _ulong dwStrLen;
//    int iEnum = 0;
//    _float3 vFloat3 = { 0.f, 0.f, 0.f };
//
//    _uint iNumParticleSystem = 0;
//
//    inputFile.read(reinterpret_cast<char*>(&iNumParticleSystem), sizeof(_uint));
//
//    CEffectPlayer::Get_Instance()->Set_NumParticleSystem(iNumParticleSystem);
//
//    for (_uint i = 0; i < iNumParticleSystem; ++i)
//    {
//        inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));
//
//        char* pTagName = new char[dwStrLen];
//
//        inputFile.read(pTagName, dwStrLen);
//
//        pEffectPlayer->Add_EffectTag(pTagName);
//
//        CParticleSystem* pParentParticleSystem = nullptr;
//
//        _uint iLevelIndex = LEVEL_STATIC;
//
//        //if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("ParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex)))
//        //	MSG_BOX("Failed to Add ParticleSystem");
//
//        //size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("ParticleSystem"));
//        //pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("ParticleSystem"), (_uint)iSize - 1));
//
//        pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));
//
//        pEffectPlayer->Add_Effect(pTagName, pParentParticleSystem);
//
//        int iNumEffects = 0;
//
//        inputFile.read(reinterpret_cast<char*>(&iNumEffects), sizeof(int));
//
//        pParentParticleSystem->Set_NumEffects(iNumEffects);
//
//        for (int j = 0; j < iNumEffects; ++j)
//        {
//            CParticleSystem* pParticleSystem = nullptr;
//            _uint iLevelIndex = LEVEL_STATIC;
//
//            //if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("ChildParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex)))
//            //	MSG_BOX("Failed to Add ParticleSystem");
//            //size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("ChildParticleSystem"));
//            //pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("ChildParticleSystem"), (_uint)iSize - 1));
//
//            pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));
//
//            pParentParticleSystem->Add_Parts(pParticleSystem);
//
//            // ParticleSystem - Transform
//            inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
//            pParticleSystem->Set_Position(vFloat3);
//            inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
//            pParticleSystem->Set_Rotation(vFloat3);
//            inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
//            pParticleSystem->Set_Scale(vFloat3);
//
//            inputFile.read(reinterpret_cast<char*>(&iEnum), sizeof(int));
//            //pParticleSystem->Create_Effect(iEnum);
//            pParticleSystem->Clone_Effect(iEnum);
//
//            //Effect
//            CEffect::EFFECTDESC EffectDesc;
//            ZeroMemory(&EffectDesc, sizeof EffectDesc);
//
//            EffectDesc.eEffectType = iEnum;
//
//            // ParticleSystem
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDuration), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isLooping), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPrewarm), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRandomStartDelay), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartDelayMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartDelayMax), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartLifeTimeOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartLifeTimeMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartLifeTimeMax), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartSpeedOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartSpeedMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartSpeedMax), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartSizeOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.is3DStartSize), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartSizeMin), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartSizeMax), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartRotationOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.is3DStartRotation), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartRotationMin), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartRotationMax), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartColorOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vColor), sizeof(_float4));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eGravityModifierOption), sizeof(int));	// �߰�
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGravityModifier), sizeof(_float2));	// �߰�
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumGravityModiferOverLifetimes), sizeof(int));	// �߰�
//
//            pParticleSystem->Get_Effect()->Reserve_GravityModiferOverLifeTime(EffectDesc.iNumGravityModiferOverLifetimes);
//
//            for (int k = 0; k < EffectDesc.iNumGravityModiferOverLifetimes; ++k)
//            {
//                CEffect::LIFETIMEVALUE LifetimeValue;
//                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                pParticleSystem->Get_Effect()->Add_GravityModiferOverLifetime(LifetimeValue);
//            }
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSimulationSpace), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSimulationSpeed), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eScalingMode), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iMaxParticles), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStopAction), sizeof(int));
//
//            // Emission
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isEmission), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRateOverTimeOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverTimeMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverTimeMax), sizeof(float));
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRateOverTime), sizeof(int));	// �߰�
//
//            pParticleSystem->Get_Effect()->Reserve_RateOverLifeTime(EffectDesc.iNumRateOverTime);
//
//            for (int k = 0; k < EffectDesc.iNumRateOverTime; ++k)
//            {
//                CEffect::LIFETIMEVALUE LifetimeValue;
//                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                pParticleSystem->Get_Effect()->Add_RateOverLifetime(LifetimeValue);
//            }
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRateOverDistanceOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverDistanceMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverDistanceMax), sizeof(float));
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumBursts), sizeof(int));
//
//            pParticleSystem->Get_Effect()->Reserve_BurstList(EffectDesc.iNumBursts);
//
//            for (int k = 0; k < EffectDesc.iNumBursts; ++k)
//            {
//                CEffect::BURSTDESC BurstDesc;
//                ZeroMemory(&BurstDesc, sizeof BurstDesc);
//
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.fTime), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.eCountOption), sizeof(int));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.iCountMin), sizeof(int));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.iCountMax), sizeof(int));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.eCycle), sizeof(int));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.iNumCycles), sizeof(int));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.fInterval), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&BurstDesc.fProbability), sizeof(float));
//
//                pParticleSystem->Get_Effect()->Add_BurstDesc(BurstDesc);
//            }
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumParticlesPerFrame), sizeof(int));	// �߰�
//
//            // Shape
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isShape), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSpark), sizeof(bool));	// �߰�
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendSpeed), sizeof(float));	// �߰�
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendEndSize), sizeof(_float2));	// �߰�
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eShapeType), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeAngle), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeRadius), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeRadiusTickness), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArc), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eArcMode), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArcSpread), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eEmitFromOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArcLength), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapePosition), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapeRotation), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapeScale), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isAlignToDirection), sizeof(bool));
//
//            // Velocity over Lifetime
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isVelocity), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityLinearOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityLinearMin), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityLinearMax), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSpace), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityOrbitalOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOrbitalMin), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOrbitalMax), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityOffsetOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOffsetMin), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOffsetMax), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityRadialOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRadialMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRadialMax), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocitySpeedModifierOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSpeedModifierMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSpeedModifierMax), sizeof(float));
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSpeedOverLifeTimes), sizeof(int)); // �߰�
//
//            pParticleSystem->Get_Effect()->Reserve_SpeedOverLifeTime(EffectDesc.iNumSpeedOverLifeTimes);
//
//            for (int k = 0; k < EffectDesc.iNumSpeedOverLifeTimes; ++k)
//            {
//                CEffect::LIFETIMEVALUE LifetimeValue;
//                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                pParticleSystem->Get_Effect()->Add_SpeedOverLifetime(LifetimeValue);
//            }
//
//            // Limit Velocity over Lifetime
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesLimitVelocity), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eLimitVelocitySpeedOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vLimitVelocitySpeedMin), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vLimitVelocitySpeedMax), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eLimitVelocitySpace), sizeof(int));
//
//            // Inherit Velocity
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isInheritVelocity), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eInheritMode), sizeof(int));
//
//            // Force over Lifetime
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isForceOverLifetime), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vForce), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSpaceForce), sizeof(int));
//
//            // Size over Lifetime
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSizeOverLifetime), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesSzOverLifeTime), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSizeOverLifetimeOption), sizeof(int));
//
//            for (int k = 0; k < 3; ++k)
//            {
//                inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSizes[k]), sizeof(int));
//
//                pParticleSystem->Get_Effect()->Reserve_SizeOverLifeTime(k, EffectDesc.iNumSizes[k]);
//
//                for (int l = 0; l < EffectDesc.iNumSizes[k]; ++l)
//                {
//                    CEffect::LIFETIMEVALUE LifetimeValue;
//                    ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                    pParticleSystem->Get_Effect()->Add_SizeOverLifeTime(k, LifetimeValue);
//                }
//            }
//
//            // Rotation over Lifetime
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRotationOverLifetime), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesRotOverLifeTime), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRotOverLifetimeOption), sizeof(int));
//
//            for (_uint k = 0; k < 3; ++k)
//            {
//                inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRotations[k]), sizeof(int));
//
//                pParticleSystem->Get_Effect()->Reserve_RotationOverLifeTime(k, EffectDesc.iNumRotations[k]);
//
//                for (int l = 0; l < EffectDesc.iNumRotations[k]; ++l)
//                {
//                    CEffect::LIFETIMEVALUE LifetimeValue;
//                    ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                    pParticleSystem->Get_Effect()->Add_RotationOverLifeTime(k, LifetimeValue);
//                }
//            }
//
//            // Position over Lifetime
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPositionOverLifetime), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.ePosOverLifetimeOption), sizeof(int));
//
//            for (_uint k = 0; k < 3; ++k)
//            {
//                inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumPositions[k]), sizeof(int));
//
//                pParticleSystem->Get_Effect()->Reserve_PositionOverLifeTime(k, EffectDesc.iNumRotations[k]);
//
//                for (int l = 0; l < EffectDesc.iNumPositions[k]; ++l)
//                {
//                    CEffect::LIFETIMEVALUE LifetimeValue;
//                    ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                    pParticleSystem->Get_Effect()->Add_PositionOverLifeTime(k, LifetimeValue);
//                }
//            }
//
//            // Rotation by Speed
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRotationBySpeed), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesRotBySpeed), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eAngularVelocityOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vAngularVelocity), sizeof(_float3));
//
//            // Collision
//            // SubEmitters
//            // Texture Sheet Animation
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isTextureSheetAnimation), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vTiles), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eTimeModeOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eFrameOverTimeOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartFrameOption), sizeof(int));
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumFrameOverTime), sizeof(int));
//
//            pParticleSystem->Get_Effect()->Reserve_FrameOverLifeTime(EffectDesc.iNumFrameOverTime);
//
//            for (int k = 0; k < EffectDesc.iNumFrameOverTime; ++k)
//            {
//                CEffect::LIFETIMEVALUE LifetimeValue;
//                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                pParticleSystem->Get_Effect()->Add_FrameOverLifetime(LifetimeValue);
//            }
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iStartFrame), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fFrameSpeedMin), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fFrameSpeedMax), sizeof(float));
//            // Trail
//            // Renderer
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRenderer), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRenderMode), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eMaterialType), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSortMode), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSortingFudge), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fMinParticleSize), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fMaxParticleSize), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPivot), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fTextureOrder), sizeof(float));
//            _float2 vCameraRightLookPos = _float2(0.f, 0.f);
//            inputFile.read(reinterpret_cast<char*>(&vCameraRightLookPos), sizeof(_float2));
//            if (CEffect::EFFECT_TEXTURE == EffectDesc.eEffectType)
//            {
//                CEffect_Texture* pTextureEffect = dynamic_cast<CEffect_Texture*>(pParticleSystem->Get_Effect());
//                pTextureEffect->Set_Order(EffectDesc.fTextureOrder);
//                pTextureEffect->Set_CameraRightLookPos(vCameraRightLookPos);
//            }
//
//            // Texture
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vFlip), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCutOff), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCutOffSoftness), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vBurnColor), sizeof(_float4));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fBurnSize), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDissolveDelay), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDissolveSpeed), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiffuseTilling), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiffuseOffset), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGradientTilling), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGradientOffset), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingDelayStartEnd), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingSpeedStart), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingSpeedEnd), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDistortionStrength), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDistortionSpeed), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.hasSecondTexture), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vNoisePanningSpeed), sizeof(_float4));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vSecondaryNoisePanningSpeed), sizeof(_float4));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPanningSpeed), sizeof(_float4));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vNormalPanningSpeed), sizeof(_float4));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPolarCoordinate), sizeof(bool));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskOuterRadius), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskInnerRadius), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskSmoothness), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskOuterRadius), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskInnerRadius), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskSmoothness), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eCullMode), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiscardedPixelMin), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiscardedPixelMax), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPixelDiscardSpeedMin), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPixelDiscardSpeedMax), sizeof(_float2));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fPixelDiscardDelay), sizeof(float));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumAlpha), sizeof(int));
//
//            pParticleSystem->Get_Effect()->Reserve_AlphaOverLifeTime(EffectDesc.iNumAlpha);
//
//            for (int k = 0; k < EffectDesc.iNumAlpha; ++k)
//            {
//                CEffect::LIFETIMEVALUE LifetimeValue;
//                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
//                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));
//
//                pParticleSystem->Get_Effect()->Add_AlphaOverLifeTime(LifetimeValue);
//            }
//
//            //Material
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRenderGroupOption), sizeof(int));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotScale), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotRot), sizeof(_float3));
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotPos), sizeof(_float3));
//
//            for (_uint k = 0; k < CEffect::TEX_END; ++k)
//            {
//                inputFile.read(reinterpret_cast<char*>(&EffectDesc.eTextureShaderOption[k]), sizeof(int));
//            }
//
//            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fTimeAcc), sizeof(float));
//
//            pParticleSystem->Set_EffectDesc(&EffectDesc);
//
//            // Keys
//            inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));
//
//            char	szTag[MAX_PATH];
//            inputFile.read(szTag, dwStrLen);
//
//            if (strlen(szTag) > 1)
//            {
//                _tchar* pTagName = new _tchar[dwStrLen];
//                MultiByteToWideChar(CP_ACP, 0, szTag, (int)dwStrLen, pTagName, MAX_PATH);
//
//                pParticleSystem->Add_Component_Model(LEVEL_STATIC, pTagName);
//
//                pParticleSystem->Get_Effect()->Set_ModelKey(pTagName);
//
//                if (!pEffectPlayer->Find_ModelKey(pTagName))
//                    pEffectPlayer->Add_ModelTag(pTagName);
//                else
//                    Safe_Delete_Array(pTagName);
//            }
//
//            for (_uint k = 0; k < CEffect::TEX_END; ++k)
//            {
//                inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));
//                inputFile.read(szTag, dwStrLen);
//
//                if (strlen(szTag) > 1)
//                {
//                    if (dwStrLen <= 1)
//                        continue;
//
//                    _tchar* pTagName = new _tchar[dwStrLen];
//                    MultiByteToWideChar(CP_ACP, 0, szTag, (int)dwStrLen, pTagName, MAX_PATH);
//
//                    switch (k)
//                    {
//                    case 0:		// TEX_DIFFUSE
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_DIFFUSE);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(0, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(0, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_DIFFUSE, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    case 1:		//TEX_MASK
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_MASK);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(1, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(1, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_MASK, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    case 2:		//TEX_RAMP
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_RAMP);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(2, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(2, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_RAMP, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    case 3:		//TEX_NOISE
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NOISE);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(3, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(3, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_NOISE, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    case 4:		//TEX_NOISE2
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NOISE2);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(4, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(4, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_NOISE2, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    case 5:		//TEX_DISTORTION
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_DISTORTION);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(5, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(5, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_DISTORTION, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    case 6:		//TEX_NORMAL
//                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NORMAL);
//                        pParticleSystem->Get_Effect()->Set_TextureKey(6, pTagName);
//                        if (!pEffectPlayer->Find_TextureKey(6, pTagName))
//                            pEffectPlayer->Add_TextureTag(CEffect::TEX_NORMAL, pTagName);
//                        else
//                            Safe_Delete_Array(pTagName);
//                        break;
//                    }
//                }
//            }
//        }
//
//        if (isParticle)
//        {
//            pEffectPlayer->Add_ParticlePool(pTagName);
//            pEffectPlayer->Add_Particles_In_Pool(pTagName, iCnt);
//        }
//    }
//
//    Safe_Release(pGameInstance);
//
//    return S_OK;
//}

HRESULT CLevel_Village::LoadEffects(const _tchar* pPath)
{
    char szFilePathChar[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP, 0, pPath, MAX_PATH, szFilePathChar, MAX_PATH, nullptr, nullptr);

    std::ifstream inputFile(szFilePathChar, std::ios::binary);

    if (!inputFile)
    {
        MSG_BOX("������ �� �� �����ϴ�.");
        return E_FAIL;
    }

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CEffectPlayer* pEffectPlayer = CEffectPlayer::Get_Instance();

    _ulong dwStrLen;
    int iEnum = 0;
    _float3 vFloat3 = { 0.f, 0.f, 0.f };

    _uint iNumParticleSystem = 0;

    inputFile.read(reinterpret_cast<char*>(&iNumParticleSystem), sizeof(_uint));

    CEffectPlayer::Get_Instance()->Set_NumParticleSystem(iNumParticleSystem);

    for (_uint i = 0; i < iNumParticleSystem; ++i)
    {
        inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));

        char* pTagName = new char[dwStrLen];

        inputFile.read(pTagName, dwStrLen);

        pEffectPlayer->Add_EffectTag(pTagName);

        CParticleSystem* pParentParticleSystem = nullptr;

        _uint iLevelIndex = LEVEL_STATIC;

        //if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("ParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex)))
        //	MSG_BOX("Failed to Add ParticleSystem");

        //size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("ParticleSystem"));
        //pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("ParticleSystem"), (_uint)iSize - 1));

        pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));

        pEffectPlayer->Add_Effect(pTagName, pParentParticleSystem);

        int iNumEffects = 0;

        inputFile.read(reinterpret_cast<char*>(&iNumEffects), sizeof(int));

        pParentParticleSystem->Set_NumEffects(iNumEffects);

        for (int j = 0; j < iNumEffects; ++j)
        {
            CParticleSystem* pParticleSystem = nullptr;
            _uint iLevelIndex = LEVEL_STATIC;

            //if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("ChildParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex)))
            //	MSG_BOX("Failed to Add ParticleSystem");
            //size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("ChildParticleSystem"));
            //pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("ChildParticleSystem"), (_uint)iSize - 1));

            pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));

            pParentParticleSystem->Add_Parts(pParticleSystem);

            // ParticleSystem - Transform
            inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
            pParticleSystem->Set_Position(vFloat3);
            inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
            pParticleSystem->Set_Rotation(vFloat3);
            inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
            pParticleSystem->Set_Scale(vFloat3);

            inputFile.read(reinterpret_cast<char*>(&iEnum), sizeof(int));
            //pParticleSystem->Create_Effect(iEnum);
            pParticleSystem->Clone_Effect(iEnum);

            //Effect
            CEffect::EFFECTDESC EffectDesc;
            ZeroMemory(&EffectDesc, sizeof EffectDesc);

            EffectDesc.eEffectType = iEnum;

            // ParticleSystem
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDuration), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isLooping), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPrewarm), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRandomStartDelay), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartDelayMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartDelayMax), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartLifeTimeOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartLifeTimeMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartLifeTimeMax), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartSpeedOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartSpeedMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartSpeedMax), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartSizeOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.is3DStartSize), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartSizeMin), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartSizeMax), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartRotationOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.is3DStartRotation), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartRotationMin), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartRotationMax), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartColorOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vColor), sizeof(_float4));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eGravityModifierOption), sizeof(int));	// �߰�
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGravityModifier), sizeof(_float2));	// �߰�

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumGravityModiferOverLifetimes), sizeof(int));	// �߰�

            pParticleSystem->Get_Effect()->Reserve_GravityModiferOverLifeTime(EffectDesc.iNumGravityModiferOverLifetimes);

            for (int k = 0; k < EffectDesc.iNumGravityModiferOverLifetimes; ++k)
            {
                CEffect::LIFETIMEVALUE LifetimeValue;
                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                pParticleSystem->Get_Effect()->Add_GravityModiferOverLifetime(LifetimeValue);
            }

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSimulationSpace), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSimulationSpeed), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eScalingMode), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iMaxParticles), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStopAction), sizeof(int));

            // Emission
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isEmission), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRateOverTimeOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverTimeMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverTimeMax), sizeof(float));

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRateOverTime), sizeof(int));	// �߰�

            pParticleSystem->Get_Effect()->Reserve_RateOverLifeTime(EffectDesc.iNumRateOverTime);

            for (int k = 0; k < EffectDesc.iNumRateOverTime; ++k)
            {
                CEffect::LIFETIMEVALUE LifetimeValue;
                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                pParticleSystem->Get_Effect()->Add_RateOverLifetime(LifetimeValue);
            }

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRateOverDistanceOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverDistanceMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverDistanceMax), sizeof(float));

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumBursts), sizeof(int));

            pParticleSystem->Get_Effect()->Reserve_BurstList(EffectDesc.iNumBursts);

            for (int k = 0; k < EffectDesc.iNumBursts; ++k)
            {
                CEffect::BURSTDESC BurstDesc;
                ZeroMemory(&BurstDesc, sizeof BurstDesc);

                inputFile.read(reinterpret_cast<char*>(&BurstDesc.fTime), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.eCountOption), sizeof(int));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.iCountMin), sizeof(int));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.iCountMax), sizeof(int));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.eCycle), sizeof(int));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.iNumCycles), sizeof(int));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.fInterval), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&BurstDesc.fProbability), sizeof(float));

                pParticleSystem->Get_Effect()->Add_BurstDesc(BurstDesc);
            }

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumParticlesPerFrame), sizeof(int));	// �߰�

            // Shape
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isShape), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSpark), sizeof(bool));	// �߰�
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendSpeed), sizeof(float));	// �߰�
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendEndSize), sizeof(_float2));	// �߰�
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eShapeType), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeAngle), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeRadius), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeRadiusTickness), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArc), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eArcMode), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArcSpread), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eEmitFromOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArcLength), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapePosition), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapeRotation), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapeScale), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isAlignToDirection), sizeof(bool));

            // Velocity over Lifetime
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isVelocity), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityLinearOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityLinearMin), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityLinearMax), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSpace), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityOrbitalOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOrbitalMin), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOrbitalMax), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityOffsetOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOffsetMin), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOffsetMax), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityRadialOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRadialMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRadialMax), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocitySpeedModifierOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSpeedModifierMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSpeedModifierMax), sizeof(float));

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSpeedOverLifeTimes), sizeof(int)); // �߰�

            pParticleSystem->Get_Effect()->Reserve_SpeedOverLifeTime(EffectDesc.iNumSpeedOverLifeTimes);

            for (int k = 0; k < EffectDesc.iNumSpeedOverLifeTimes; ++k)
            {
                CEffect::LIFETIMEVALUE LifetimeValue;
                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                pParticleSystem->Get_Effect()->Add_SpeedOverLifetime(LifetimeValue);
            }

            // Limit Velocity over Lifetime
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesLimitVelocity), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eLimitVelocitySpeedOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vLimitVelocitySpeedMin), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vLimitVelocitySpeedMax), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eLimitVelocitySpace), sizeof(int));

            // Inherit Velocity
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isInheritVelocity), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eInheritMode), sizeof(int));

            // Force over Lifetime
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isForceOverLifetime), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vForce), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSpaceForce), sizeof(int));

            // Size over Lifetime
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSizeOverLifetime), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesSzOverLifeTime), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSizeOverLifetimeOption), sizeof(int));

            for (int k = 0; k < 3; ++k)
            {
                inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSizes[k]), sizeof(int));

                pParticleSystem->Get_Effect()->Reserve_SizeOverLifeTime(k, EffectDesc.iNumSizes[k]);

                for (int l = 0; l < EffectDesc.iNumSizes[k]; ++l)
                {
                    CEffect::LIFETIMEVALUE LifetimeValue;
                    ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                    pParticleSystem->Get_Effect()->Add_SizeOverLifeTime(k, LifetimeValue);
                }
            }

            // Rotation over Lifetime
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRotationOverLifetime), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesRotOverLifeTime), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRotOverLifetimeOption), sizeof(int));

            for (_uint k = 0; k < 3; ++k)
            {
                inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRotations[k]), sizeof(int));

                pParticleSystem->Get_Effect()->Reserve_RotationOverLifeTime(k, EffectDesc.iNumRotations[k]);

                for (int l = 0; l < EffectDesc.iNumRotations[k]; ++l)
                {
                    CEffect::LIFETIMEVALUE LifetimeValue;
                    ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                    pParticleSystem->Get_Effect()->Add_RotationOverLifeTime(k, LifetimeValue);
                }
            }

            // Position over Lifetime
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPositionOverLifetime), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.ePosOverLifetimeOption), sizeof(int));

            for (_uint k = 0; k < 3; ++k)
            {
                inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumPositions[k]), sizeof(int));

                pParticleSystem->Get_Effect()->Reserve_PositionOverLifeTime(k, EffectDesc.iNumRotations[k]);

                for (int l = 0; l < EffectDesc.iNumPositions[k]; ++l)
                {
                    CEffect::LIFETIMEVALUE LifetimeValue;
                    ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                    inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                    pParticleSystem->Get_Effect()->Add_PositionOverLifeTime(k, LifetimeValue);
                }
            }

            // Rotation by Speed
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRotationBySpeed), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesRotBySpeed), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eAngularVelocityOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vAngularVelocity), sizeof(_float3));

            // Collision
            // SubEmitters
            // Texture Sheet Animation
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isTextureSheetAnimation), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vTiles), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eTimeModeOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eFrameOverTimeOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartFrameOption), sizeof(int));

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumFrameOverTime), sizeof(int));

            pParticleSystem->Get_Effect()->Reserve_FrameOverLifeTime(EffectDesc.iNumFrameOverTime);

            for (int k = 0; k < EffectDesc.iNumFrameOverTime; ++k)
            {
                CEffect::LIFETIMEVALUE LifetimeValue;
                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                pParticleSystem->Get_Effect()->Add_FrameOverLifetime(LifetimeValue);
            }

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iStartFrame), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fFrameSpeedMin), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fFrameSpeedMax), sizeof(float));
            // Trail
            // Renderer
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRenderer), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRenderMode), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eMaterialType), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSortMode), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSortingFudge), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fMinParticleSize), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fMaxParticleSize), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPivot), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fTextureOrder), sizeof(float));
            _float2 vCameraRightLookPos = _float2(0.f, 0.f);
            inputFile.read(reinterpret_cast<char*>(&vCameraRightLookPos), sizeof(_float2));
            if (CEffect::EFFECT_TEXTURE == EffectDesc.eEffectType)
            {
                CEffect_Texture* pTextureEffect = dynamic_cast<CEffect_Texture*>(pParticleSystem->Get_Effect());
                pTextureEffect->Set_Order(EffectDesc.fTextureOrder);
                pTextureEffect->Set_CameraRightLookPos(vCameraRightLookPos);
            }

            // Texture
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vFlip), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCutOff), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCutOffSoftness), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vBurnColor), sizeof(_float4));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fBurnSize), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDissolveDelay), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDissolveSpeed), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiffuseTilling), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiffuseOffset), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGradientTilling), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGradientOffset), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingDelayStartEnd), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingSpeedStart), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingSpeedEnd), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDistortionStrength), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDistortionSpeed), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.hasSecondTexture), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vNoisePanningSpeed), sizeof(_float4));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vSecondaryNoisePanningSpeed), sizeof(_float4));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPanningSpeed), sizeof(_float4));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vNormalPanningSpeed), sizeof(_float4));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPolarCoordinate), sizeof(bool));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskOuterRadius), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskInnerRadius), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskSmoothness), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskOuterRadius), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskInnerRadius), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskSmoothness), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eCullMode), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiscardedPixelMin), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiscardedPixelMax), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPixelDiscardSpeedMin), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPixelDiscardSpeedMax), sizeof(_float2));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fPixelDiscardDelay), sizeof(float));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumAlpha), sizeof(int));

            pParticleSystem->Get_Effect()->Reserve_AlphaOverLifeTime(EffectDesc.iNumAlpha);

            for (int k = 0; k < EffectDesc.iNumAlpha; ++k)
            {
                CEffect::LIFETIMEVALUE LifetimeValue;
                ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
                inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

                pParticleSystem->Get_Effect()->Add_AlphaOverLifeTime(LifetimeValue);
            }

            //Material
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRenderGroupOption), sizeof(int));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotScale), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotRot), sizeof(_float3));
            inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotPos), sizeof(_float3));

            for (_uint k = 0; k < CEffect::TEX_END; ++k)
            {
                inputFile.read(reinterpret_cast<char*>(&EffectDesc.eTextureShaderOption[k]), sizeof(int));
            }

            inputFile.read(reinterpret_cast<char*>(&EffectDesc.fTimeAcc), sizeof(float));

            pParticleSystem->Set_EffectDesc(&EffectDesc);

            // Keys
            inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));

            char	szTag[MAX_PATH];
            inputFile.read(szTag, dwStrLen);

            if (strlen(szTag) > 1)
            {
                _tchar* pTagName = new _tchar[dwStrLen];
                MultiByteToWideChar(CP_ACP, 0, szTag, (int)dwStrLen, pTagName, MAX_PATH);

                pParticleSystem->Add_Component_Model(LEVEL_STATIC, pTagName);

                pParticleSystem->Get_Effect()->Set_ModelKey(pTagName);

                if (!pEffectPlayer->Find_ModelKey(pTagName))
                    pEffectPlayer->Add_ModelTag(pTagName);
                else
                    Safe_Delete_Array(pTagName);
            }

            for (_uint k = 0; k < CEffect::TEX_END; ++k)
            {
                inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));
                inputFile.read(szTag, dwStrLen);

                if (strlen(szTag) > 1)
                {
                    if (dwStrLen <= 1)
                        continue;

                    _tchar* pTagName = new _tchar[dwStrLen];
                    MultiByteToWideChar(CP_ACP, 0, szTag, (int)dwStrLen, pTagName, MAX_PATH);

                    switch (k)
                    {
                    case 0:		// TEX_DIFFUSE
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_DIFFUSE);
                        pParticleSystem->Get_Effect()->Set_TextureKey(0, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(0, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_DIFFUSE, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    case 1:		//TEX_MASK
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_MASK);
                        pParticleSystem->Get_Effect()->Set_TextureKey(1, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(1, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_MASK, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    case 2:		//TEX_RAMP
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_RAMP);
                        pParticleSystem->Get_Effect()->Set_TextureKey(2, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(2, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_RAMP, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    case 3:		//TEX_NOISE
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NOISE);
                        pParticleSystem->Get_Effect()->Set_TextureKey(3, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(3, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_NOISE, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    case 4:		//TEX_NOISE2
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NOISE2);
                        pParticleSystem->Get_Effect()->Set_TextureKey(4, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(4, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_NOISE2, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    case 5:		//TEX_DISTORTION
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_DISTORTION);
                        pParticleSystem->Get_Effect()->Set_TextureKey(5, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(5, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_DISTORTION, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    case 6:		//TEX_NORMAL
                        pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NORMAL);
                        pParticleSystem->Get_Effect()->Set_TextureKey(6, pTagName);
                        if (!pEffectPlayer->Find_TextureKey(6, pTagName))
                            pEffectPlayer->Add_TextureTag(CEffect::TEX_NORMAL, pTagName);
                        else
                            Safe_Delete_Array(pTagName);
                        break;
                    }
                }
            }
        }
    }

    Safe_Release(pGameInstance);

    return S_OK;
}


CLevel_Village* CLevel_Village::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Village* pInstance = new CLevel_Village(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Village");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Village::Free()
{
    __super::Free();
}
