#include "pch.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Lobby.h"
#include "Level_GamePlay.h"
#include "Level_Village.h"
#include "Level_House.h"
#include "Level_Train.h"
#include "Level_FinalBoss.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Loading.h"
#include "Fade.h"
#include "Fade_Manager.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

   /* m_pBackGround = CBackGround::Create(m_pDevice, m_pContext);
    if (nullptr == m_pBackGround)
        return E_FAIL;

    m_pBackGround->Initialize(nullptr);*/


    CLoading::UIDESC UIDesc;

    // Back
    m_pBack = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pBack)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 0;

    m_pBack->Initialize(&UIDesc);

    // Circle
    m_pCircle = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pCircle)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 1;

    m_pCircle->Initialize(&UIDesc);


    // Rect
    m_pRect = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pRect)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 2;

    m_pRect->Initialize(&UIDesc);


    // Title
    m_pTitle = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pTitle)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 8;

    m_pTitle->Initialize(&UIDesc);

    // Cloud_LT
    m_pCloud_LT = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pCloud_LT)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 9;
  
    m_pCloud_LT->Initialize(&UIDesc);


    // Cloud_RT
    m_pCloud_RT = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pCloud_RT)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 10;
   

    m_pCloud_RT->Initialize(&UIDesc);

    // Cloud_LD
    m_pCloud_LD = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pCloud_LD)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 11;
    UIDesc.m_Is_Y_Reverse = true;

    m_pCloud_LD->Initialize(&UIDesc);


    // Cloud_RD
    m_pCloud_RD = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pCloud_RD)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 12;
    UIDesc.m_Is_X_Reverse = true;
    UIDesc.m_Is_Y_Reverse = true;

    m_pCloud_RD->Initialize(&UIDesc);

    // Door
    m_pDoor = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pDoor)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 13;
   
    m_pDoor->Initialize(&UIDesc);


    // Walk
    m_pWalk = CLoading::Create(m_pDevice, m_pContext);

    if (nullptr == m_pWalk)
        return E_FAIL;

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Type = 14;

    m_pWalk->Initialize(&UIDesc);


    // Fade
    CFade::UIDESC UIDesc2;
    m_pFade = CFade::Create(m_pDevice, m_pContext);

    if (nullptr == m_pFade)
        return E_FAIL;

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Type = 0;

    m_pFade->Initialize(&UIDesc2);


    m_eNextLevelID = eNextLevelID;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);

   /* m_pBackGround->Set_TextureIndex(g_iLoadingTextureIndex % 3);
    m_pBackGround->Tick(dTimeDelta);
    m_pBackGround->LateTick(dTimeDelta);*/

    if (g_iLoadingTextureIndex > 0) {

        m_pBack->Tick(dTimeDelta);
        m_pBack->LateTick(dTimeDelta);

        m_pCircle->Tick(dTimeDelta);
        m_pCircle->LateTick(dTimeDelta);

        m_pRect->Tick(dTimeDelta);
        m_pRect->LateTick(dTimeDelta);

        m_pTitle->Tick(dTimeDelta);
        m_pTitle->LateTick(dTimeDelta);

        m_pCloud_LT->Tick(dTimeDelta);
        m_pCloud_LT->LateTick(dTimeDelta);

        m_pCloud_RT->Tick(dTimeDelta);
        m_pCloud_RT->LateTick(dTimeDelta);

        m_pCloud_LD->Tick(dTimeDelta);
        m_pCloud_LD->LateTick(dTimeDelta);

        m_pCloud_RD->Tick(dTimeDelta);
        m_pCloud_RD->LateTick(dTimeDelta);

        m_pDoor->Tick(dTimeDelta);
        m_pDoor->LateTick(dTimeDelta);

        m_pWalk->Tick(dTimeDelta);
        m_pWalk->LateTick(dTimeDelta);

        m_pFade->Tick(dTimeDelta);
        m_pFade->LateTick(dTimeDelta);

    }

   

    if (true == m_pLoader->Get_Finished())
    {
        if (GetKeyState(VK_SPACE) & 0x8000)
        {
            if (g_iLoadingTextureIndex > 0) {

                CFadeManager::GetInstance()->Set_Fade_Out(true);

            }
        }

        if (g_iLoadingTextureIndex == 0 && m_eNextLevelID == LEVEL_LOGO) {

            CLevel* pLevel = { nullptr };

            pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
            
            if (nullptr == pLevel)
                return;

            g_iLoadingTextureIndex += 1;

            HRESULT hr = 0;

            CGameInstance* pGameInstance = CGameInstance::GetInstance();
            Safe_AddRef(pGameInstance);

            hr = pGameInstance->Open_Level(m_eNextLevelID, pLevel, true);
            Safe_Release(pGameInstance);

            if (FAILED(hr))
                return;
        }
        else if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true && m_eNextLevelID != LEVEL_LOGO) {

            CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

            CLevel* pLevel = { nullptr };

            switch (m_eNextLevelID)
            {
            case LEVEL_LOBBY:
                pLevel = CLevel_Lobby::Create(m_pDevice, m_pContext);
                break;
            case LEVEL_GAMEPLAY:
                pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
                break;
            case LEVEL_VILLAGE:
                pLevel = CLevel_Village::Create(m_pDevice, m_pContext);
                break;
            case LEVEL_HOUSE:
                pLevel = CLevel_House::Create(m_pDevice, m_pContext);
                break;
            case LEVEL_TRAIN:
                pLevel = CLevel_Train::Create(m_pDevice, m_pContext);
                break;
            case LEVEL_FINALBOSS:
                pLevel = CLevel_FinalBoss::Create(m_pDevice, m_pContext);
                break;
            }

            if (nullptr == pLevel)
                return;

            g_iLoadingTextureIndex += 1;

            HRESULT hr = 0;

            CGameInstance* pGameInstance = CGameInstance::GetInstance();
            Safe_AddRef(pGameInstance);

            hr = pGameInstance->Open_Level(m_eNextLevelID, pLevel, true);
            Safe_Release(pGameInstance);

            if (FAILED(hr))
                return;


        }

    }
}

HRESULT CLevel_Loading::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
    CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Loading::Free()
{
    __super::Free();

    //Safe_Release(m_pBackGround);
    Safe_Release(m_pBack);
    Safe_Release(m_pCircle);
    Safe_Release(m_pRect);
    Safe_Release(m_pTitle);
    Safe_Release(m_pCloud_LT);
    Safe_Release(m_pCloud_RT);
    Safe_Release(m_pCloud_LD);
    Safe_Release(m_pCloud_RD);
    Safe_Release(m_pDoor);
    Safe_Release(m_pWalk);
    Safe_Release(m_pFade);



    Safe_Release(m_pLoader);
}
