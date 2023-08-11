#include "pch.h"
#include "..\Public\Level_House.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera.h"
#include "Player.h"
#include "MapObject.h"

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

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
    {
        MSG_BOX("Failed to Ready_Layer_MapObject : CLevel_House");
        return E_FAIL;
    }

    return S_OK;
}

void CLevel_House::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
    SetWindowText(g_hWnd, TEXT("House"));

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_DIKeyDown(DIK_RETURN))
    {
        HRESULT hr = 0;

        if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_TRAIN))
        {
            pGameInstance->Clear_Light();
            hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TRAIN), false, false);
        }
        else
            hr = pGameInstance->Swap_Level(LEVEL_TRAIN);

        if (FAILED(hr))
        {
            Safe_Release(pGameInstance);
            return;
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

    CameraDesc.fFovY = XMConvertToRadians(60.f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNearZ = 0.3f;
    CameraDesc.fFarZ = 400.f;

    CameraDesc.TransformDesc.dSpeedPerSec = 10.0;
    CameraDesc.TransformDesc.dRadianRotationPerSec = XMConvertToRadians(90.f);
    CameraDesc.dSensitivity = 0.1;

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

    // CharacterDesc.WorldInfo.vPosition = _float4(8.f, 0.f, 10.f, 1.f);
    CharacterDesc.WorldInfo.vPosition = _float4(118.f, 0.f, 117.f, 1.f);    // BattleMap

    CharacterDesc.Land_Y = 0.f;
    //CharacterDesc.eCurNavi = CLandObject::NAVI_HOUSE_0_0;
    CharacterDesc.eCurNavi = CLandObject::NAVI_HOUSE_4_0;                   // BattleMap

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, pLayerTag,
        TEXT("Prototype_GameObject_Player_Tanjiro"), &CharacterDesc)))
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
