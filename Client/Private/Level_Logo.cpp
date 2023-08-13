#include "pch.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Title.h"
#include "Title_Manager.h"
#include "Fade.h"
#include "Fade_Manager.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);

    SetWindowText(g_hWnd, TEXT("Logo"));

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (GetKeyState(VK_RETURN) & 0x8000 && CTitleManager::GetInstance()->Get_Select_Type() == 0)
    {
        CFadeManager::GetInstance()->Set_Fade_Out(true);
    }

    if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {


        CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

        HRESULT hr = 0;

        if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_LOBBY))
        {
            pGameInstance->Clear_Light();
            hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY), false, false);
        }
        else
            hr = pGameInstance->Swap_Level(LEVEL_LOBBY);

        if (FAILED(hr))
        {
            Safe_Release(pGameInstance);
            return;
        }

    }



    Safe_Release(pGameInstance);
}

HRESULT CLevel_Logo::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    
    CTitle::UIDESC UIDesc;
    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
        
    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 5;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 6;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    CFade::UIDESC UIDesc2;
    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

   

    Safe_Release(pGameInstance);

    return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Logo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();
}
