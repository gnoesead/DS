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

#include "Pause.h"
#include "Fade.h"
#include "Fade_Manager.h"




CLevel_Village::CLevel_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Village::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

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
        MSG_BOX("Failed to Ready_Layer_Camera : CLevel_Village");
        return E_FAIL;
    }

   
    if (FAILED(Ready_Layer_Player_UI(TEXT("Layer_Player_UI"))))
    {
        MSG_BOX("Failed to Ready_Layer_Player_UI : CLevel_Village");
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


    CFadeManager::GetInstance()->Set_Fade_In(true);


    return S_OK;
}

void CLevel_Village::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);

    SetWindowText(g_hWnd, TEXT("Village"));

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CColliderManager::GetInstance()->Check_Collider(LEVEL_VILLAGE, dTimeDelta);


   
    
    if (pGameInstance->Get_DIKeyDown(DIK_F9))
    {
        CFadeManager::GetInstance()->Set_Fade_Out(true);
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
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    LIGHTDESC LightDesc;
    ZeroMemory(&LightDesc, sizeof LightDesc);

    LightDesc.eType = LIGHTDESC::TYPE_DIRECTION;
    LightDesc.vLightDir         = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vLightDiffuse     = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vLightAmbient     = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vLightSpecular    = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Direction_Light");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

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

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

 
    CharacterDesc.WorldInfo.vPosition = _float4(573.f, 4.5f, 242.f, 1.f);

    CharacterDesc.Land_Y = 0.0f;
    CharacterDesc.eCurNavi = CLandObject::NAVI_VILLAGE_MAINROAD1;
 
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, pLayerTag, 
        TEXT("Prototype_GameObject_Player_Tanjiro"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : CLevel_Village");
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


// Pause
    CPause::UIDESC UIDesc3;
    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    /*UIDesc3.m_Is_Reverse = false;
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
    }*/

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Is_Reverse = false;
    UIDesc3.m_Type = 6;


    if (FAILED(pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Player_UI"),
        TEXT("Prototype_GameObject_Pause"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

   

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
