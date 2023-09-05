#include "pch.h"
#include "..\Public\Loader.h"

#include "BackGround.h"

#include "Camera_Free.h"
#include "Player.h"
#include "Player_Tanjiro.h"
#include "Player_Zenitsu.h"
#include "Player_Rengoku.h"
#include "NPC_Zenitsu.h"

#include "Monster_Test.h"
#include "Boss_Akaza.h"
#include "Boss_Kyogai.h"
#include "Monster_Spider.h"
#include "Monster_Zako.h"
#include "Monster_StealthZako.h"
#include "Monster_Swamp.h"
#include "SwampShot.h"

#include "StaticMapObject.h"
#include "TerrainMapObject.h"
#include "RotationMapObject.h"
#include "InstanceMapObject.h"
#include "Sky.h"

#include "Player_Battle_Frame.h"
#include "Player_Battle_Hp.h"
#include "Player_Battle_Mp.h"
#include "Boss_Battle_Frame.h"
#include "Boss_Battle_Hp.h"
#include "Player_Battle_Ult_Frame.h"
#include "Player_Battle_Combo.h"
#include "Player_Battle_Ult_Effect.h"
#include "World_UI_Hp.h"
#include "FIcon.h"
#include "Interaction.h"
#include "Dialog.h"
#include "Mission.h"
#include "Mini_Map.h"
#include "Title.h"
#include "Loading.h"
#include "Story_Board.h"
#include "Pause.h"
#include "Fade.h"
#include "Skill_Name.h"
#include "Battle_Signal.h"
#include "Option.h"
#include "Paper.h"


#include "NPC_Female.h"
#include "NPC_AdultM.h"
#include "NPC_ChildF.h"
#include "NPC_ChildM.h"


#include "Sword.h"
#include "SwordHome.h"

#include "AtkCollider.h"

#include "EffectPlayer.h"
#include "ParticleSystem.h"
#include "Effect_Texture.h"
#include "Effect_Mesh.h"
#include "Effect_Particle.h"

#include "CollisionBox.h"

#include "TrainSmoke.h"
#include "RoomSmoke.h"
#include "AlertCircle.h"
#include "AlertRect.h"
#include "SmellBundle.h"

#include "Swamp.h"
#include "SwampWaterEffect.h"
#include "Swamp_AlertRect.h"
#include "WaterParticleEffect.h"
#include "Swamp_SmokeEffect.h"
#include "GroundSmoke.h"
#include "StoneParticle.h"
#include "SmeshStone.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(pLoader->Get_CS());

	HRESULT	hr = 0;

	switch (pLoader->Get_LevelID())
	{
	case LEVEL_LOGO:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForLogo();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	case LEVEL_LOBBY:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForLobby();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	case LEVEL_GAMEPLAY:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForGamePlay();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	case LEVEL_VILLAGE:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForVillage();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	case LEVEL_HOUSE:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForHouse();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	case LEVEL_TRAIN:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForTrain();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	case LEVEL_FINALBOSS:
		if (false == pLoader->Get_Loaded())
		{
			hr = pLoader->LoadingForFinalBoss();
		}
		else
		{
			pLoader->Set_Finished();
		}
		break;
	}
	
	if (FAILED(hr))
		return 1;

	LeaveCriticalSection(pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eLevelID)
{
	m_eLevelID = eLevelID;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != pGameInstance->Get_LoadedStage(m_eLevelID))
		m_isLoaded = true;

	Safe_Release(pGameInstance);

	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
	{
		MSG_BOX("Failed to BeginThread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::LoadingForAllStage()
{

	return S_OK;
}

HRESULT CLoader::LoadingForLogo()
{
	SetWindowText(g_hWnd, TEXT("LoadingForLogo"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Sky_Train*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sky_Train"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/Sky_Train.bin", PivotMatrix))))
		return E_FAIL;

	Load_MapObjectModel_AllStage(pGameInstance);

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	/* Prototype_Component_Shader_VtxTerrainModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTerrainModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTerrainModel.hlsl"), VTXMODEL_DECL::Elements, VTXMODEL_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxTerrainModel");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxModelInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODELINSTANCE_DECL::Elements, VTXMODELINSTANCE_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxModelInstance");
		return E_FAIL;
	}
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc
	LoadingForEffect(pGameInstance);

#pragma endregion

	
#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));
#pragma region Object
	
#pragma endregion

#pragma region UI
	
#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion

	

#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::LoadingForLobby()
{
	
	SetWindowText(g_hWnd, TEXT("LoadingForStage"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma region EnvironmentTexture


#pragma endregion

#pragma region RampTexture

#pragma endregion

#pragma region EffectTexture



#pragma endregion

#pragma region UITexture

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model

#pragma region Buffer

#pragma endregion
	_matrix		PivotMatrix = XMMatrixIdentity();
#pragma region Effect

#pragma endregion

#pragma region Character
	/* Prototype_Component_Model_Tanjiro */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tanjiro"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Tanjiro/Tanjiro.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Tanjiro");
		return E_FAIL;
	}

	// Prototype_Component_Model_Tanjiro_Sword
	PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tanjiro_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Tanjiro_Sword/Tanjiro_Sword.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Tanjiro_Sword");
		return E_FAIL;
	}

	// Prototype_Component_Model_Tanjiro_Sword_In
	PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tanjiro_Sword_In"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Tanjiro_Sword/Tanjiro_Sword.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Tanjiro_Sword_In");
		return E_FAIL;
	}

	// Prototype_Component_Model_Tanjiro_SwordHome 
	PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tanjiro_SwordHome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Tanjiro_Sword/Tanjiro_SwordHome.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Tanjiro_SwordHome");
		return E_FAIL;
	}


	/* Prototype_Component_Model_Zenitsu */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zenitsu"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Zenitsu/Zenitsu.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Zenitsu");
		return E_FAIL;
	}

	// Prototype_Component_Model_Zenitsu_Sword
	PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zenitsu_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Zenitsu_Sword/Zenitsu_Sword.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Zenitsu_Sword");
		return E_FAIL;
	}

	// Prototype_Component_Model_Zenitsu_Sword_In
	// PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	XMVECTOR pivotTranslation = XMVectorSet(0.0f, 0.0f, -30.0f, 0.0f);
	PivotMatrix = XMMatrixTranslationFromVector(pivotTranslation) * XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zenitsu_Sword_In"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Zenitsu_Sword/Zenitsu_Sword.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Zenitsu_Sword_In");
		return E_FAIL;
	}
	// Prototype_Component_Model_Zenitsu_SwordHome 
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zenitsu_SwordHome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Zenitsu_Sword/Zenitsu_SwordHome.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Tanjiro_SwordHome");
		return E_FAIL;
	}


	/* Prototype_Component_Model_Rengoku */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rengoku"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Rengoku/Rengoku.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Rengoku");
		return E_FAIL;
	}
	// Prototype_Component_Model_Rengoku_Sword
	PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rengoku_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Rengoku_Sword/Rengoku_Sword.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Rengoku_Sword");
		return E_FAIL;
	}
	// Prototype_Component_Model_Rengoku_Sword_In
	PivotMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rengoku_Sword_In"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Rengoku_Sword/Rengoku_Sword.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Rengoku_Sword_In");
		return E_FAIL;
	}
	// Prototype_Component_Model_Rengoku_SwordHome 
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rengoku_SwordHome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Rengoku_Sword/Rengoku_SwordHome.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Rengoku_SwordHome");
		return E_FAIL;
	}


	// Prototype_Component_Model_Akaza
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Akaza"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Akaza/Akaza.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Akaza");
		return E_FAIL;
	}
	// Prototype_Component_Model_Kyogai
	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Kyogai"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Kyogai/Kyogai.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Kyogai");
		return E_FAIL;
	}

	// Prototype_Component_Model_Spider
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Spider"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Spider/Spider.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Spider");
		return E_FAIL;
	}



	// Prototype_Component_Model_NPC_Female
	PivotMatrix = XMMatrixScaling(0.0065f, 0.0065f, 0.0065f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/NPC_Female/NPC_Female.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_NPC_Female");
		return E_FAIL;
	}
	// Prototype_Component_Model_NPC_AdultM
	PivotMatrix = XMMatrixScaling(0.0065f, 0.0065f, 0.0065f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC_AdultM"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/NPC_AdultM/NPC_AdultM.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_NPC_AdultM");
		return E_FAIL;
	}
	// Prototype_Component_Model_NPC_ChildF
	PivotMatrix = XMMatrixScaling(0.0065f, 0.0065f, 0.0065f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC_ChildF"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/NPC_ChildF/NPC_ChildF.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_NPC_ChildF");
		return E_FAIL;
	}
	// Prototype_Component_Model_NPC_ChildM
	PivotMatrix = XMMatrixScaling(0.0065f, 0.0065f, 0.0065f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC_ChildM"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/NPC_ChildM/NPC_ChildM.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_NPC_ChildM");
		return E_FAIL;
	}


	/* Prototype_Component_Model_Monster_Zako_0 */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Zako_0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Character/Zako_0/Zako_0.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Monster_Zako_0");
		return E_FAIL;
	}
	
	/* Prototype_Component_Model_Monster_Swamp_Horn1 */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Swamp_Horn1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Character/Swamp/Swamp_Horn1/SwampHorn1.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Monster_Swamp_Horn1");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Monster_Swamp_Horn2 */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Swamp_Horn2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Character/Swamp/Swamp_Horn2/SwampHorn2.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Monster_Swamp_Horn2");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Monster_Swamp_Horn3 */
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Swamp_Horn3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Character/Swamp/Swamp_Horn3/SwampHorn3.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Monster_Swamp_Horn3");
		return E_FAIL;
	}



#pragma endregion

#pragma region NonCharacter

#pragma endregion

#pragma region Terrain

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECL::Elements, VTXNORTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECL::Elements, VTXMODEL_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxModel");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECL::Elements, VTXANIMMODEL_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxAnimModel");
		return E_FAIL;
	}


#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc

	/* Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Collider_AABB");
		return E_FAIL;
	}

	/* Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Collider_OBB");
		return E_FAIL;
	}

	/* Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Collider_Sphere");
		return E_FAIL;
	}


#pragma endregion

#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));

	/* Prototype_GameObject_AtkCollider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AtkCollider"),
		CAtkCollider::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_AtkCollider");
		return E_FAIL;
	}

#pragma region Object
	/* Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Camera_Free");
		return E_FAIL;
	}

	/* Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Player_Tanjiro */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Tanjiro"),
		CPlayer_Tanjiro::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Player_Tanjiro");
		return E_FAIL;
	}
	/* Prototype_GameObject_Player_Zenitsu */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Zenitsu"),
		CPlayer_Zenitsu::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Player_Zenitsu");
		return E_FAIL;
	}
	/* Prototype_GameObject_Player_Rengoku */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Rengoku"),
		CPlayer_Rengoku::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Player_Rengoku");
		return E_FAIL;
	}

	/* Prototype_GameObject_NPC_Zenitsu */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_Zenitsu"),
		CNPC_Zenitsu::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_NPC_Zenitsu");
		return E_FAIL;
	}

	/* Prototype_GameObject_NPC_Female */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_Female"),
		CNPC_Female::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_NPC_Female");
		return E_FAIL;
	}
	/* Prototype_GameObject_NPC_AdultM */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_AdultM"),
		CNPC_AdultM::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_NPC_AdultM");
		return E_FAIL;
	}
	// Prototype_GameObject_NPC_ChildF 
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_ChildF"),
		CNPC_ChildF::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_NPC_ChildF");
		return E_FAIL;
	}
	// Prototype_GameObject_NPC_ChildM
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_ChildM"),
		CNPC_ChildM::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_NPC_ChildM");
		return E_FAIL;
	}


	/* Prototype_GameObject_Sword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Sword");
		return E_FAIL;
	}
	/* Prototype_GameObject_SwordHome */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordHome"),
		CSwordHome::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_SwordHome");
		return E_FAIL;
	}


	/* Prototype_GameObject_Boss_Akaza */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Akaza"),
		CBoss_Akaza::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_Akaza");
		return E_FAIL;
	}

	/* Prototype_GameObject_Boss_Kyogai */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Kyogai"),
		CBoss_Kyogai::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_Kyogai");
		return E_FAIL;
	}

	/* Prototype_GameObject_Monster_Spider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Spider"),
		CMonster_Spider::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_Spider");
		return E_FAIL;
	}
	
	/* Prototype_GameObject_Monster_Zako_0 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Zako_0"),
		CMonster_Zako::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_Zako_0");
		return E_FAIL;
	}

	/* Prototype_GameObject_Monster_StealthZako */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_StealthZako"),
		CMonster_StealthZako::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_StealthZako");
		return E_FAIL;
	}

	/* Prototype_GameObject_Monster_Swamp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Swamp"),
		CMonster_Swamp::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_Swamp");
		return E_FAIL;
	}
	/* Prototype_GameObject_SwampShot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwampShot"),
		CSwampShot::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_SwampShot");
		return E_FAIL;
	}


	/* Prototype_GameObject_Monster_Test */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Test"),
		CMonster_Test::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Monster_Test");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_AlertCircle*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AlertCircle"),
		CAlertCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_AlertRect*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AlertRect"),
		CAlertRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SmellBundle*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SmellBundle"),
		CSmellBundle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_GroundSmoke*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundSmoke"),
		CGroundSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StoneParticle*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StoneParticle"),
		CStoneParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion

#pragma region Environment
	/* For.Prototype_GameObject_StaticMapObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticMapObject"),
		CStaticMapObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_StaticMapObject");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_TerrainMapObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TerrainMapObject"),
		CTerrainMapObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_TerrainMapObject");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_RotationMapObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotationMapObject"),
		CRotationMapObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_RotationMapObject");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_InstanceMapObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InstanceMapObject"),
		CInstanceMapObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_InstanceMapObject");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Sky");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_CollisionBox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CollisionBox"),
		CCollisionBox::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_CollisionBox");
		return E_FAIL;
	}


#pragma endregion

#pragma region UI
	/* Prototype_GameObject_Player Battle Frame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Battle_Frame"),
		CPlayer_Battle_Frame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player Battle Hp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Battle_Hp"),
		CPlayer_Battle_Hp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player Battle Mp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Battle_Mp"),
		CPlayer_Battle_Mp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player Battle Ult Frame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"),
		CPlayer_Battle_Ult_Frame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player Battle Combo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Battle_Combo"),
		CPlayer_Battle_Combo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Boss Battle Frame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Battle_Frame"),
		CBoss_Battle_Frame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Boss Battle Hp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Battle_Hp"),
		CBoss_Battle_Hp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player Battle Ult Frame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"),
		CPlayer_Battle_Ult_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_World_UI_Hp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_World_UI_Hp"),
		CWorld_UI_Hp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_FIcon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FIcon"),
		CFIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_interaction */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Interaction"),
		CInteraction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Dialog */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dialog"),
		CDialog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Mission */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mission"),
		CMission::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Mini_Map */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mini_Map"),
		CMini_Map::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Story_Board */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Story_Board"),
		CStory_Board::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Story_Board */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pause"),
		CPause::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Skill_Name */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Name"),
		CSkill_Name::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Battle_Signal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Battle_Signal"),
		CBattle_Signal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Paper */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Paper_UI"),
		CPaper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion

#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::LoadingForGamePlay()
{
	SetWindowText(g_hWnd, TEXT("LoadingForLogo"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc

#pragma endregion


#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));
#pragma region Object

#pragma endregion

#pragma region UI
	

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion



#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::LoadingForVillage()
{
	SetWindowText(g_hWnd, TEXT("LoadingForVillage"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma region EnvironmentTexture

#pragma endregion

#pragma region RampTexture

#pragma endregion

#pragma region EffectTexture

	/* Prototype_Component_Texture_Swamp*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_Swamp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Swamp/Swamp%d.dds"), 14))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Texture_Swamp");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_SwampWater*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_SwampWater"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Swamp/Water/Spr_Effect_Docheol_FloorFireRayUp_%d.dds"), 20))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Texture_SwampWater");
		return E_FAIL;
	}


#pragma endregion

#pragma region UITexture

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model

#pragma region Buffer

#pragma endregion
	_matrix		PivotMatrix = XMMatrixIdentity();
#pragma region Effect

#pragma endregion

#pragma region Character
	
#pragma endregion

#pragma region NonCharacter

#pragma endregion

#pragma region Terrain

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc

#pragma endregion

#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));

	/* Prototype_GameObject_Swamp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Swamp"),
		CSwamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_SwampWaterEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwampWaterEffect"),
		CSwampWaterEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_SwampAlertRect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Swamp_AlertRect"),
		CSwamp_AlertRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_WaterParticleEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterParticleEffect"),
		CWaterParticleEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Swamp_SmokeEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Swamp_SmokeEffect"),
		CSwamp_SmokeEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_SmeshStone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SmeshStone"),
		CSmeshStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma region Object
	
#pragma endregion

#pragma region Environment
	Load_MapObjectModel_Village();
#pragma endregion

#pragma region UI

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion

#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::LoadingForHouse()
{
	SetWindowText(g_hWnd, TEXT("LoadingForHouse"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma region EnvironmentTexture

#pragma endregion

#pragma region RampTexture

#pragma endregion

#pragma region EffectTexture

#pragma endregion

#pragma region UITexture

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model

#pragma region Buffer

#pragma endregion
	_matrix		PivotMatrix = XMMatrixIdentity();
#pragma region Effect

#pragma endregion

#pragma region Character

#pragma endregion

#pragma region NonCharacter
	
#pragma endregion

#pragma region Terrain
	Load_MapObjectModel_House();
	
#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc

#pragma endregion

#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));

	/* For.Prototype_GameObject_RoomSmoke*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RoomSmoke"),
		CRoomSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Object
	
#pragma endregion

#pragma region Environment
	
#pragma endregion

#pragma region UI

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion

#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::LoadingForTrain()
{
	SetWindowText(g_hWnd, TEXT("LoadingForTrain"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma region EnvironmentTexture

#pragma endregion

#pragma region RampTexture

#pragma endregion

#pragma region EffectTexture

	/* For.Prototype_Component_Texture_Train_Smoke*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Texture_Train_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Environments/Map/Effect/T_e_cmn_Smoke008.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region UITexture

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model

#pragma region Buffer

#pragma endregion
	_matrix		PivotMatrix = XMMatrixIdentity();
#pragma region Effect

#pragma endregion

#pragma region Character

#pragma endregion

#pragma region NonCharacter
	
#pragma endregion

#pragma region Terrain
	Load_MapObjectModel_Train();
#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc

#pragma endregion

#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));
#pragma region Object
	
	/* For.Prototype_GameObject_TrainSmoke*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrainSmoke"),
		CTrainSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Environment
	
#pragma endregion

#pragma region UI

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion

#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::LoadingForFinalBoss()
{
	SetWindowText(g_hWnd, TEXT("LoadingForFinalBoss"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma region EnvironmentTexture
	
#pragma endregion

#pragma region RampTexture

#pragma endregion

#pragma region EffectTexture

#pragma endregion

#pragma region UITexture

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model

#pragma region Buffer

#pragma endregion
	_matrix		PivotMatrix = XMMatrixIdentity();
#pragma region Effect

#pragma endregion

#pragma region Character
	
#pragma endregion

#pragma region NonCharacter
	
#pragma endregion

#pragma region Terrain
	Load_MapObjectModel_FinalBoss();	// 맵 오브젝트 로드(안원 전용)	
#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc

#pragma endregion

#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));
#pragma region Object

#pragma endregion

#pragma region Environment

#pragma endregion

#pragma region UI

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion

#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_MapObjectModel_AllStage(CGameInstance* pGameInstance)
{
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	// ===========Texture=============
	/* For.Prototype_Component_Texture_TerrainMask*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TerrainMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Environments/Map/Acaza_vs/Filter.jpg")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Splating*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Splating"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Environments/Map/Acaza_vs/T_d_114_RiceField_01a_BC.dds")))))
		return E_FAIL;

	// ===========Model=============
	/* For.Prototype_Component_ModelInstance_114_Grass_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_114_Grass_01a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Grass_01a.bin", PivotMatrix, 1000))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_114_Grass_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_114_Grass_02a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Grass_02a.bin", PivotMatrix, 1000))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_114_Grass_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_114_Grass_03a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Grass_03a.bin", PivotMatrix, 1000))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_GrassStrip_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_GrassStrip_01a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Acaza_vs/GrassStrip_01a.bin", PivotMatrix, 1000))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_cmn_mount_Coniferous_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_cmn_mount_Coniferous_01a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_01a.bin", PivotMatrix, 20))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_cmn_mount_Coniferous_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_cmn_mount_Coniferous_02a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_02a.bin", PivotMatrix, 20))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_cmn_mount_Coniferous_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_cmn_mount_Coniferous_03a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_03a.bin", PivotMatrix, 20))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_cmn_mount_Coniferous_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_cmn_mount_Coniferous_04a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_04a.bin", PivotMatrix, 20))))
		return E_FAIL;

	/* For.Prototype_Component_Model_006_Trees_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_006_Trees_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/006_Trees_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_114_Bush_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_114_Bush_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Bush_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_114_Seedlings_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_114_Seedlings_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Seedlings_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_114_SmallTree_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_114_SmallTree_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_SmallTree_03a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_006_MiddleTree_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_006_MiddleTree_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/006_MiddleTree_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_006_MiddleTree_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_006_MiddleTree_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/006_MiddleTree_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Coniferous_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Coniferous_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Coniferous_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Coniferous_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Coniferous_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Coniferous_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Coniferous_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Coniferous_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/cmn_mount_Coniferous_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MangroveTree_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MangroveTree_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/MangroveTree_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_RedPine_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RedPine_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/RedPine_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Broadleaf_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Broadleaf_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/Broadleaf_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TreeStump_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TreeStump_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/TreeStump_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BrokenTree_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BrokenTree_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/BrokenTree_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BrokenTree_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BrokenTree_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/BrokenTree_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BrokenTree_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BrokenTree_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/BrokenTree_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BrokenTree_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BrokenTree_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/BrokenTree_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BrokenTree_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BrokenTree_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/BrokenTree_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BrokenTree_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BrokenTree_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/BrokenTree_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_03c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_03c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_03c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_06a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_06a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_06a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_09a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_09a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_09a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_10a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_10a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_10a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_mount_Bush_12a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cmn_mount_Bush_12a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/cmn_mount_Bush_12a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Flower_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Flower_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/Flower_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Flower_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Flower_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/Flower_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Flower_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Flower_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/Flower_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Lavender_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lavender_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Bushes/Lavender_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rock_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Rocks/Rock_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rock_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Rocks/Rock_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rock_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Rocks/Rock_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rock_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Rocks/Rock_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rock_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Rocks/Rock_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_Stone_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_Stone_01a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Village/Stone_01a.bin", PivotMatrix, 20))))
		return E_FAIL;

	/* For.Prototype_Component_ModelInstance_Stone_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ModelInstance_Stone_01b"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Village/Stone_01b.bin", PivotMatrix, 20))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell1_1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell1_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell1_1.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell1_2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell1_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell1_2.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell1_3*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell1_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell1_3.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell1_4*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell1_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell1_4.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell2_1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell2_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell2_1.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell2_2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell2_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell2_2.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell2_3*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell2_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell2_3.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell2_4*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell2_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell2_4.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell3_1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell3_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell3_1.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell3_2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell3_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell3_2.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell3_3*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell3_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell3_3.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell3_4*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell3_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell3_4.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell4_1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell4_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell4_1.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell4_2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell4_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell4_2.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell4_3*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell4_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell4_3.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Smell4_4*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smell4_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Smell4_4.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wind1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wind1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Wind1.bin", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Fog*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Fog.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fog2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Fog2.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fog3*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Fog3.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fog4*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Fog4.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fog5*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Fog5.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_AlertRect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_AlertRect"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/AlertRect.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Swamp_AlertRect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Swamp_AlertRect"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Swamp_AlertRect.bin", PivotMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_MapObjectModel_Village()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_matrix			PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	/* For.Prototype_Component_Model_Sky_Village*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sky_Village"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Sky_Village.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_VillageTerrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_VillageTerrain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,"../Bin/Resources/Models/Environments/Map/Village/VillageTerrain.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_104_River_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_104_River_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/104_River_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_002_Bridge_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_002_Bridge_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/002_Bridge_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_06a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_06a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_06a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_07a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_07a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_07a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_07d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_07d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_07d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Build_Shop_09a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Build_Shop_09a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Build_Shop_09a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_05b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_05b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_05b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_07a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_07a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_07a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_09a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_09a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_09a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_10a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_10a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_10a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_House_11a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_House_11a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/House_11a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Warehouse_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Warehouse_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Warehouse_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Warehouse_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Warehouse_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Warehouse_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Warehouse_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Warehouse_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Warehouse_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yashiki_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Yashiki_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Yashiki_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yashiki_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Yashiki_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Yashiki_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarHouse_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_FarHouse_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/FarHouse_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarHouse_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_FarHouse_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/FarHouse_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarWareHouse_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_FarWareHouse_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/FarWareHouse_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarWareHouse_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_FarWareHouse_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/FarWareHouse_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarTower_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_FarTower_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/FarTower_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_01c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_01c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_01c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_01d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_01d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_01d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_02c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_02c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_02c.bin", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Wall_02d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_02d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_02d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_03c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_03c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_03c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_03d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_03d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_03d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_03e*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_03e"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_03e.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_04b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_04b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_04b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_04c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_04c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_04c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_06a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_06a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_06a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_06d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_06d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_06d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_06f*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_06f"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_06f.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_07a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_07a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_07a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_07b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_07b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_07b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_07c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_07c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_07c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_08a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_08a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_08a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_08b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_08b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_08b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_08c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_08c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_08c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wall_08d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wall_08d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wall_08d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_01c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_01c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_01c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallStone_04b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallStone_04b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallStone_04b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodFence_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodFence_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodFence_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodFence_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodFence_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodFence_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodFence_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodFence_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodFence_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_06a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_06a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_06a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_07a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_07a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_07a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndL_08a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndL_08a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndL_08a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndR_04b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndR_04b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndR_04b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndR_07b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndR_07b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndR_07b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WallEndR_08b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WallEndR_08b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WallEndR_08b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Gate_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Gate_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Gate_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Gate_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Gate_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Gate_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Gate_02c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Gate_02c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Gate_02c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_GardenDoor_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_GardenDoor_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/GardenDoor_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_GardenDoor_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_GardenDoor_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/GardenDoor_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_GardenDoor_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_GardenDoor_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/GardenDoor_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_GardenDoor_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_GardenDoor_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/GardenDoor_04a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BellTower_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_BellTower_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/BellTower_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BossHouse*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_BossHouse"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/BossHouse.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StairsCanal_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_StairsCanal_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/StairsCanal_01a.bin", PivotMatrix))))
		return E_FAIL;

	// Object

	/* For.Prototype_Component_Model_ActionCrate_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ActionCrate_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ActionCrate_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShinobiGaeshi_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ShinobiGaeshi_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ShinobiGaeshi_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_UtilityPole_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_UtilityPole_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/UtilityPole_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_UtilityPoleLine_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_UtilityPoleLine_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/UtilityPoleLine_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bag_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bag_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bag_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bag_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bag_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bag_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BambooBlind_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_BambooBlind_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/BambooBlind_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BambooTable_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_BambooTable_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/BambooTable_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BambooTable_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_BambooTable_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/BambooTable_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Barrel_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Barrel_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Barrel_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Barrel_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Barrel_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Barrel_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Barrel_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Barrel_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Barrel_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Basket_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_01b.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_01c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_01c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_01c.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_02a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_02b.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_02c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_02c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_02c.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_03a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_03b.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_03c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_03c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_03c.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_04a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_05a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Basket_06a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Basket_06a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Basket_06a.bin", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Bucket_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bucket_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bucket_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bucket_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bucket_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bucket_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bucket_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bucket_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bucket_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CherryBlossom_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_CherryBlossom_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/CherryBlossom_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CherryBlossom_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_CherryBlossom_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/CherryBlossom_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CherryBlossom_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_CherryBlossom_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/CherryBlossom_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CrateSet_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_CrateSet_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/CrateSet_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FabricBox_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_FabricBox_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/FabricBox_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HayBag_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_HayBag_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/HayBag_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HayStack_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_HayStack_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/HayStack_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rope_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Rope_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Rope_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rope_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Rope_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Rope_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Timber_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Timber_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Timber_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Timber_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Timber_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Timber_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TimberBasket_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TimberBasket_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/TimberBasket_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TimberWall_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TimberWall_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/TimberWall_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TimberWall_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TimberWall_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/TimberWall_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TimberWall_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TimberWall_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/TimberWall_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Trash_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Trash_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Trash_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Trash_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Trash_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Trash_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WillowTree_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WillowTree_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WillowTree_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wisteria_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wisteria_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wisteria_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wisteria_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wisteria_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wisteria_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wisteria_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wisteria_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wisteria_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wisteria_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Wisteria_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Wisteria_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodBox_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodBox_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodBox_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodBox_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodBox_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodBox_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodBox_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodBox_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodBox_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodBoxSet_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodBoxSet_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodBoxSet_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodenCart_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodenCart_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodenCart_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodenCart_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodenCart_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodenCart_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodStack_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_WoodStack_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/WoodStack_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bench_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bench_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bench_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bench_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bench_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bench_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bottles_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bottles_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bottles_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bottles_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bottles_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bottles_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bottles_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bottles_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bottles_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bottles_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bottles_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bottles_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bousuki_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Bousuki_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Bousuki_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Box_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Box_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Box_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_04b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_04b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_04b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ClayPot_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ClayPot_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/ClayPot_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Stone_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Stone_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Stone_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Stone_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Stone_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Stone_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Lamp_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Lamp_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Lamp_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Lamp_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Lamp_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/Lamp_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_far_hill_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_far_hill_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/far_hill_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_far_hill_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_far_hill_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/far_hill_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BigMountain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_BigMountain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Village/BigMountain.bin", PivotMatrix))))
		return E_FAIL;

	// Instance
	/* For.Prototype_Component_ModelInstance_WoodFence_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_ModelInstance_WoodFence_01a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Village/WoodFence_01a.bin", PivotMatrix, 50))))
		return E_FAIL;
	 
	/* For.Prototype_Component_ModelInstance_Wall_07a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_ModelInstance_Wall_07a"),
		CModel_Instance::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environments/Map/Village/Wall_07a.bin", PivotMatrix, 30))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Load_MapObjectModel_House()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_matrix			PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	// =============== StaticMapObject==============//

	/* For.Prototype_Component_Model_CorridorWood_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_01*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_01.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_02*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_02.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_06*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_06"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_06.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_06b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_06b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_06b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_06c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_06c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_06c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_01c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_01c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_01c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_02c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_02c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_02c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorShoji_03c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorShoji_03c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorShoji_03c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorEntrance_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorEntrance_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorEntrance_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01g*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01g"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01g.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02g*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02g"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02g.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Paper_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Paper_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Paper_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Hibachi_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Hibachi_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Hibachi_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_RoomLight_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_RoomLight_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/RoomLight_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_RoomLight_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_RoomLight_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/RoomLight_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Lantern_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Lantern_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Lantern_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Blood*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Blood"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Blood.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Scratch*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Scratch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Scratch.bin", PivotMatrix))))
		return E_FAIL;

	///======================================================================================
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	/* For.Prototype_Component_Model_Room_S_03*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_03.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_04*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_04"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_04.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_05*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_05"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_05.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_05b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_05b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_05b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_07*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_07"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_07.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_08*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_08"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_08.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_09a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_09a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_09a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_09b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_09b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_09b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_10a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_10a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_10a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_11a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_11a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_11a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_12a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_12a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_12a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Room_S_13a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Room_S_13a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Room_S_13a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_06a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_06a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_06a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_07a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_07a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_07a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_08a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_08a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_08a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_08b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_08b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_08b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_09a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_09a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_09a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_10a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_10a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_10a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_11a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_11a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_11a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWood_12a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWood_12a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWood_12a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWall_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWall_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWall_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWoodCorner_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWoodCorner_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWoodCorner_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWoodCorner_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWoodCorner_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWoodCorner_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWoodCorner_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWoodCorner_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWoodCorner_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWoodCorner_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWoodCorner_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWoodCorner_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CorridorWoodCorner_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_CorridorWoodCorner_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/CorridorWoodCorner_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01e*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01e"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01e.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_01f*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_01f"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_01f.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02e*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02e"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02e.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DoorFusuma_02f*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DoorFusuma_02f"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DoorFusuma_02f.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Table_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Table_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Table_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Scroll_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Scroll_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Scroll_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rubble_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Rubble_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Rubble_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Rubble_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Rubble_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Rubble_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Picture_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Picture_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Picture_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Jar_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Jar_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Jar_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_004_Flower_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_004_Flower_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/004_Flower_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DeskItem_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DeskItem_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DeskItem_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_DeskItem_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_DeskItem_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/DeskItem_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cushion_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Cushion_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Cushion_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Chest_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Chest_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Chest_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Chest_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Chest_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Chest_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bowl_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Bowl_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Bowl_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BookShelf_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BookShelf_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BookShelf_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_Chest_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_cmn_Chest_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/cmn_Chest_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_Chest_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_cmn_Chest_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/cmn_Chest_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_cmn_Chest_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_cmn_Chest_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/cmn_Chest_03a.bin", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Desk_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Desk_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Desk_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Nagahibachi_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Nagahibachi_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Nagahibachi_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SitPillow_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_SitPillow_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/SitPillow_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaBox_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaBox_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaBox_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaBox_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaBox_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaBox_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaBox_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaBox_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaBox_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Teaset_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Teaset_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Teaset_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaShelf_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaShelf_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaShelf_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaVase_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaVase_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaVase_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaVase_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaVase_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaVase_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeaVase_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_TeaVase_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/TeaVase_03a.bin", PivotMatrix))))
		return E_FAIL;

	// ===============RoomMap BossRoom======================
	/* For.Prototype_Component_Model_BossRoom*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BossRoom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BossRoom.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_107_DoorWood_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_107_DoorWood_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/107_DoorWood_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_107_DoorWood_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_107_DoorWood_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/107_DoorWood_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WindowDoor*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_WindowDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/WindowDoor.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WindowDoor_Small*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_WindowDoor_Small"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/WindowDoor_Small.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodWall*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_WoodWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/WoodWall.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_AllWoodWall*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_AllWoodWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/AllWoodWall.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BattleRoomBase*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BattleRoomBase"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BattleRoomBase.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Veranda_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Veranda_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Veranda_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Veranda_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_Veranda_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/Veranda_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_RoomLight_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_RoomLight_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/RoomLight_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BattleRoomCorridor_01*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BattleRoomCorridor_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BattleRoomCorridor_01.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BattleRoomCorridor_02*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BattleRoomCorridor_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BattleRoomCorridor_02.bin", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_BattleRoomCorridor_03*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BattleRoomCorridor_03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BattleRoomCorridor_03.bin", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_BattleRoomCorridor_04*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BattleRoomCorridor_04"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BattleRoomCorridor_04.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BattleRoomCorridor_05*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_BattleRoomCorridor_05"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/BattleRoomCorridor_05.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_107_OuterWall_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HOUSE, TEXT("Prototype_Component_Model_107_OuterWall_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/RoomMap/107_OuterWall_01a.bin", PivotMatrix))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Load_MapObjectModel_Train()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_matrix			PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	/* For.Prototype_Component_Model_Scrollground_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_Scrollground_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/Scrollground_01a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Scrollground_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_Scrollground_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/Scrollground_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Locomotive_01*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_Locomotive_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/Locomotive_01.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Locomotive_02*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_Locomotive_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/Locomotive_02.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Locomotive_03*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_Locomotive_03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/Locomotive_03.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarHouse_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_FarHouse_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/FarHouse_03a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarHouse_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_FarHouse_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/FarHouse_03b.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarHouse_03c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_FarHouse_03c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/FarHouse_03c.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FarHouse_03d*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_FarHouse_03d"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Train/FarHouse_03d.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Train_Wind*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TRAIN, TEXT("Prototype_Component_Model_Train_Wind"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Effect/Train_Wind.bin", PivotMatrix))))
		return E_FAIL;



	 
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Load_MapObjectModel_FinalBoss()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_matrix			PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	// ===============BattleMap Acaza======================
	/* For.Prototype_Component_Model_BattleMap_Acaza*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BattleMap_Acaza"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/BattleMap_Acaza.bin", PivotMatrix))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_BrokenLocomotive_01*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BrokenLocomotive_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/BrokenLocomotive_01.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BrokenLocomotive_01c*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BrokenLocomotive_01c"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/BrokenLocomotive_01c.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BrokenLocomotive_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BrokenLocomotive_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/BrokenLocomotive_02a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BrokenLocomotive_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BrokenLocomotive_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/BrokenLocomotive_03a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BrokenLocomotive_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BrokenLocomotive_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/BrokenLocomotive_04a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/Sky.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Far_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Far_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/Far_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Far_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Far_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/Far_02a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Far_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Far_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/Far_04a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_far_hill_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_far_hill_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/far_hill_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_far_hill_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_far_hill_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/far_hill_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_114_SmallCliff_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_SmallCliff_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_SmallCliff_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rail_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Rail_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/Rail_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rail_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Rail_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/Rail_02a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_114_Middle_01a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_01a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_01a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_02a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_02a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_02a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_02b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_02b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_02b.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_03a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_03a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_03a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_03b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_03b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_03b.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_04a.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_04b*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_04b"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_04b.bin", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_114_Middle_05a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_114_Middle_05a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Acaza_vs/114_Middle_05a.bin", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_006_Trees_04a*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_006_Trees_04a"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Map/Trees/006_Trees_04a.bin", PivotMatrix))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::LoadingForEffect(CGameInstance* pGameInstance)
{
	SetWindowText(g_hWnd, TEXT("LoadingForEffect"));
	
#pragma region Buffer
	/* Prototype_Component_VIBuffer_Point_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Effect"),
		CVIBuffer_Point_Effect::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_VIBuffer_Point_Effect");
		return E_FAIL;
	}

	/* Prototype_Component_VIBuffer_Point_Instance_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_Effect"),
		CVIBuffer_Point_Instance_Effect::Create(m_pDevice, m_pContext, 1000))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_VIBuffer_Point_Instance_Effect");
		return E_FAIL;
	}
#pragma endregion

#pragma region Shader
	/* For.Prototype_Component_Shader_VtxPoint_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_Effect.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxPoint_Effect");
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_VtxModel_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Effect.hlsl"), VTXMODEL_DECL::Elements, VTXMODEL_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxModel_Effect");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxPointInstanceEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstanceEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_Effect.hlsl"), VTXPOINTINSTANCEEFFECT_DECL::Elements, VTXPOINTINSTANCEEFFECT_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxPointInstanceEffect");
		return E_FAIL;
	}
#pragma endregion

#pragma region GAMEOBJECTS
	/* Prototype_GameObject_ParticleSystem */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleSystem"),
		CParticleSystem::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_ParticleSystem");
		return E_FAIL;
	}

	/* Prototype_GameObject_EffectTexture */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectTexture"),
		CEffect_Texture::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_EffectTexture");
		return E_FAIL;
	}

	/* Prototype_GameObject_EffectMesh */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectMesh"),
		CEffect_Mesh::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_EffectMesh");
		return E_FAIL;
	}

	/* Prototype_GameObject_EffectParticle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectParticle"),
		CEffect_Particle::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_EffectParticle");
		return E_FAIL;
	}
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Finished!!!"));
	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLevelID)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CS);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
