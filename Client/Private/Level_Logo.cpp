#include "pch.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Title.h"
#include "Title_Manager.h"
#include "Fade.h"
#include "Fade_Manager.h"
#include "Option.h"
#include "OptionManager.h"
#include "SoundMgr.h"

#include "EffectPlayer.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Effect_Texture.h"
#include "Effect_Particle.h"

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

    //_tchar szBgm[MAX_PATH] = TEXT("Intro.mp3");
    //CSoundMgr::Get_Instance()->PlayBGM(szBgm, 0.6f);

	Ready_Layer_Effect();

    return S_OK;
}

void CLevel_Logo::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);

    SetWindowText(g_hWnd, TEXT("Logo"));

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_DIKeyDown(DIK_RETURN))
    {
        if (CTitleManager::GetInstance()->Get_Select_Type() == 0) {

            if (COptionManager::GetInstance()->Get_Is_Option_On() == false)
                CFadeManager::GetInstance()->Set_Ink_In(true);
        }
        else {
            if (COptionManager::GetInstance()->Get_Is_Option_On() == false) {
                COptionManager::GetInstance()->Set_Is_Option_On(true);
                COptionManager::GetInstance()->Set_Is_Reset(false);
            }
        }
    }

    if (CFadeManager::GetInstance()->Get_Ink_In_Done() == true) {


        CFadeManager::GetInstance()->Set_Ink_In_Done(false);

        HRESULT hr = 0;

        if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_LOBBY))
        {
            pGameInstance->Clear_Light();
            hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY), false, false);
        }
        else
        {
            pGameInstance->Clear_Light();
            hr = pGameInstance->Swap_Level(LEVEL_LOBBY);
        }

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

    ZeroMemory(&UIDesc, sizeof UIDesc);

    UIDesc.m_Is_Reverse = false;
    UIDesc.m_Type = 7;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Title"), &UIDesc))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


// Fade
    CFade::UIDESC UIDesc2;
    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc2, sizeof UIDesc2);

    UIDesc2.m_Is_Reverse = false;
    UIDesc2.m_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc2))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
   
// Option
    COption::UIDESC UIDesc3;
    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 3;
    UIDesc3.m_Line_Num = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 3;
    UIDesc3.m_Line_Num = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 3;
    UIDesc3.m_Line_Num = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 3;
    UIDesc3.m_Line_Num = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 3;
    UIDesc3.m_Line_Num = 4;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    for (int i = 0; i < 5; i++) {

        ZeroMemory(&UIDesc3, sizeof UIDesc3);

        UIDesc3.m_Type = 4;
        UIDesc3.m_Line_Num = i;
        UIDesc3.m_Arrow_Type = 0;
        UIDesc3.m_Is_X_Reverse = true;

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }

        ZeroMemory(&UIDesc3, sizeof UIDesc3);

        UIDesc3.m_Type = 4;
        UIDesc3.m_Line_Num = i;
        UIDesc3.m_Arrow_Type = 1;
        UIDesc3.m_Is_X_Reverse = false;

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }
    }

    for (int i = 0; i < 5; i++) {

        ZeroMemory(&UIDesc3, sizeof UIDesc3);

        UIDesc3.m_Type = 5;
        UIDesc3.m_Line_Num = i;

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }

        ZeroMemory(&UIDesc3, sizeof UIDesc3);

        UIDesc3.m_Type = 6;
        UIDesc3.m_Line_Num = i;
     
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
            Safe_Release(pGameInstance);
            return E_FAIL;
        }
    }
   
    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 7;
    UIDesc3.m_Menu_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 7;
    UIDesc3.m_Menu_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 7;
    UIDesc3.m_Menu_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 9;
    UIDesc3.m_Cloud_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 10;
    UIDesc3.m_Cloud_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 11;
    UIDesc3.m_Is_X_Reverse = true;
    UIDesc3.m_Is_Y_Reverse = true;
    UIDesc3.m_Cloud_Type = 1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 12;
    UIDesc3.m_Is_X_Reverse = true;
    UIDesc3.m_Is_Y_Reverse = true;
    UIDesc3.m_Cloud_Type = 0;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 13;
    UIDesc3.m_Cloud_Type = 2;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 14;
    UIDesc3.m_Cloud_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 15;
    UIDesc3.m_Is_X_Reverse = true;
    UIDesc3.m_Is_Y_Reverse = true;
    UIDesc3.m_Cloud_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 16;
    UIDesc3.m_Is_Y_Reverse = true;
    UIDesc3.m_Cloud_Type = 3;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 17;
   
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 18;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    ZeroMemory(&UIDesc3, sizeof UIDesc3);

    UIDesc3.m_Type = 19;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Option"), &UIDesc3))) {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }


    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Effect()
{
#pragma region COMMON
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Spark.bin"), true, 30)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Spark");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_Up.bin"), true, 50)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_Up");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle0.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle0");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/FallDown_Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : FallDown_Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_0_X.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_0_X");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_0_Z.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_0_Z");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_1_X.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_1_X");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_1_Z.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_1_Z");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_2_X.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_2_X");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Particle_2_Z.bin"), true, 20)))
	{
		MSG_BOX("Failed to Load Effect : Hit_Particle_2_Z");
		return E_FAIL;
	}
#pragma endregion

#pragma region RENGOKU
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo1_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo1_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo2_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo2_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo3_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo3_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo4_Down_Decal1Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo4_Down_Decal1Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo4_Down_Decal2Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo4_Down_Decal2Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo4_Up_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo4_Up_DecalParticle");
		return E_FAIL;
	}
#pragma endregion

#pragma region TANJIRO
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo1_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo1_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo2_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo2_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo3_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo3_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo4_Down_Decal1Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo4_Down_Decal1Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo4_Down_Decal2Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo4_Down_Decal2Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo4_Up_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo4_Up_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Tilt_DecalParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Tilt_DecalParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo1_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo1_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo2_WaterParticle1.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo2_WaterParticle1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo2_WaterParticle2.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo2_WaterParticle2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo3_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo3_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4_Normal_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4_Normal_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4_Down_WaterParticle1.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4_Down_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4_Down_WaterParticle2.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4_Down_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4_Up_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4_Up_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeTilt_WaterParticle1.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeTilt_WaterParticle1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeTilt_WaterParticle2.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeTilt_WaterParticle2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo_Air1_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo_Air1_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo_Air2_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo_Air2_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo_Air3_WaterParticle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo_Air3_WaterParticle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_GaeBang_Particle.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_GaeBang_Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Charge_Particle.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Charge_Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Particle_W.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Particle_W");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1_Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1_Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1_Particle_SplashCircle.bin"), true, 50)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1_Particle_SplashCircle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1_Particle_SplashFall.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1_Particle_SplashFall");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1_ParticleFountain.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1_ParticleFountain");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1_ParticleFountainMain.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1_ParticleFountainMain");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1_Particle_WaterDrop.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1_Particle_WaterDrop");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Aurora_Particle0.bin"), true, 30)))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Aurora_Particle0");
		return E_FAIL;
	}
#pragma endregion

#pragma region AKAZA
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_0.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_0");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_1.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_1");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_2.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_2");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_2_1.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_2_1");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_3.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_3");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_3_1.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_3_1");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_Upper.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_Upper");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_Punch_0.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_Punch_0");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_Punch_1.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_Punch_1");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchLine_0.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchLine_0");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchLine_1.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchLine_1");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchLine_2.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchLine_2");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchLine_3.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchLine_3");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchEnd_0.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchEnd_0");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchEnd_1.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchEnd_1");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchEnd_2.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchEnd_2");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_Combo_PunchEnd_3.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_Combo_PunchEnd_3");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Part_SkillUp_0.bin"), true, 6)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Part_SkillUp_0");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Akaza/Akaza_Shoot_Aura.bin"), true, 4)))
	{
		MSG_BOX("Failed to Load Effect : Akaza_Shoot_Aura");
		return E_FAIL;
	}
#pragma endregion

#pragma region KYOGAI
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Kyogai/Kyogai_Explosion_Particle.bin"), true, 1)))
	{
		MSG_BOX("Failed to Load Effect : Kyogai_Explosion_Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Kyogai/Kyogai_Swing_Particle1.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Kyogai_Swing_Particle1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Kyogai/Kyogai_Swing_Particle2.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Kyogai_Swing_Particle2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Kyogai/Kyogai_CutScene_Particle.bin"), true, 5)))
	{
		MSG_BOX("Failed to Load Effect : Kyogai_CutScene_Particle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Kyogai/Kyogai_CutScene_Particle2.bin"), true, 4)))
	{
		MSG_BOX("Failed to Load Effect : Kyogai_CutScene_Particle2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Kyogai/Kyogai_CutScene_Particle3.bin"), true, 2)))
	{
		MSG_BOX("Failed to Load Effect : Kyogai_CutScene_Particle3");
		return E_FAIL;
	}

#pragma endregion

#pragma region SWAMP
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_AtkParticle.bin"), true, 6)))
	{
		MSG_BOX("Failed to Load Effect : Swamp_AtkParticle");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_AtkParticle2.bin"), true, 6)))
	{
		MSG_BOX("Failed to Load Effect : Swamp_AtkParticle");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_AtkParticle3.bin"), true, 6)))
	{
		MSG_BOX("Failed to Load Effect : Swamp_AtkParticle");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_AtkParticle4.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Swamp_AtkParticle4");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_AtkParticle_Upper.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Swamp_AtkParticle_Upper");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Swamp/Swamp_AtkParticle_Kulbam.bin"), true, 3)))
	{
		MSG_BOX("Failed to Load Effect : Swamp_AtkParticle_Kulbam");
		return E_FAIL;
	}
	

#pragma endregion

#pragma region Zako
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zako/Zako_Claws_Particle_L.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Zako_Claws_Particle_L");
		return E_FAIL;
	}
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zako/Zako_Claws_Particle_R.bin"), true, 10)))
	{
		MSG_BOX("Failed to Load Effect : Zako_Claws_Particle_R");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Logo::LoadEffects(const _tchar* pPath, _bool isParticle, _int iCnt)
{
	char szFilePathChar[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, pPath, MAX_PATH, szFilePathChar, MAX_PATH, nullptr, nullptr);

	std::ifstream inputFile(szFilePathChar, std::ios::binary);

	if (!inputFile)
	{
		MSG_BOX("파일을 열 수 없습니다.");
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
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSetYToGround), sizeof(bool));
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
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eGravityModifierOption), sizeof(int));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGravityModifier), sizeof(_float2));	// 추가

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumGravityModiferOverLifetimes), sizeof(int));	// 추가

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

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRateOverTime), sizeof(int));	// 추가

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

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumParticlesPerFrame), sizeof(int));	// 추가

			// Shape
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isShape), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSpark), sizeof(bool));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendSpeed), sizeof(float));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendEndSize), sizeof(_float2));	// 추가
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

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSpeedOverLifeTimes), sizeof(int)); // 추가

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
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isAttachedToBone), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vUVStart), sizeof(_float3));
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
			pParticleSystem->Get_Effect()->Set_Order(EffectDesc.fTextureOrder);

			_float2 vCameraRightLookPos = _float2(0.f, 0.f);
			inputFile.read(reinterpret_cast<char*>(&vCameraRightLookPos), sizeof(_float2));
			if (CEffect::EFFECT_TEXTURE == EffectDesc.eEffectType)
			{
				CEffect_Texture* pTextureEffect = dynamic_cast<CEffect_Texture*>(pParticleSystem->Get_Effect());
				pTextureEffect->Set_CameraRightLookPos(vCameraRightLookPos);
			}
			else if (CEffect::EFFECT_PARTICLE == EffectDesc.eEffectType)
			{
				CEffect_Particle* pParticleEffect = dynamic_cast<CEffect_Particle*>(pParticleSystem->Get_Effect());
				pParticleEffect->Set_CameraRightLookPos(vCameraRightLookPos);
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

		if (isParticle)
		{
			pEffectPlayer->Add_ParticlePool(pTagName);
			pEffectPlayer->Add_Particles_In_Pool(pTagName, iCnt);
		}
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
