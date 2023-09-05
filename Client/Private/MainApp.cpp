#include "pch.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "AtkCollManager.h"
#include "ColliderManager.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Title_Manager.h"
#include "Story_Manager.h"
#include "MonsterManager.h"
#include "SwampManager.h"
#include "PlayerManager.h"
#include "Fade_Manager.h"
#include "Mini_Map_Manager.h"
#include "Camera_Manager.h"
#include "Battle_UI_Manager.h"
#include "Title.h"
#include "Loading.h"
#include "Fade.h"
#include "Option.h"
#include "OptionManager.h"

#include "SoundMgr.h"
#include "EffectPlayer.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	CSoundMgr::Get_Instance()->Initialize();

	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	// D3D11_BLEND_DESC
	// D3D11_RASTERIZER_DESC
	// D3D11_DEPTH_STENCIL_DESC
	// D3D11_RASTERIZER_DESC

	//ID3D11BlendState*		pBlendState = { nullptr };

	//D3D11_BLEND_DESC		BlendDesc;

	//m_pDevice->CreateBlendState(&BlendDesc, &pBlendState);

	///* RenderState*/ 
	//m_pContext->OMSetBlendState(pBlendState);
	//m_pContext->OMSetDepthStencilState();
	//m_pContext->RSSetState();

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInstance, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX("Failed to Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/Bombardier.spritefont"))))
	{
		MSG_BOX("Failed to Add_Font");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_KR"), TEXT("../Bin/Resources/Fonts/Mabi.spritefont"))))
	{
		MSG_BOX("Failed to Add_Font");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_DM"), TEXT("../Bin/Resources/Fonts/DMex.spritefont"))))
	{
		MSG_BOX("Failed to Add_Font");
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		MSG_BOX("Failed to Ready_Prototype_Component_For_Static");
		return E_FAIL;
	}

	if (FAILED(SetUp_StartLevel(LEVEL_LOGO)))
	{
		MSG_BOX("Failed to SetUp_StartLevel");
		return E_FAIL;
	}

	

	return S_OK;
}

void CMainApp::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(dTimeDelta);

	CTitleManager::GetInstance()->Tick();
	
	CCameraManager::GetInstance()->Tick(dTimeDelta);

	CBattle_UI_Manager::GetInstance()->Tick(dTimeDelta);

	COptionManager::GetInstance()->Tick(dTimeDelta);

	//CSoundMgr::Get_Instance()->Tick();

	// ���� ����
	if (m_pGameInstance->Get_DIKeyDown(DIK_9)) {

		CFadeManager::GetInstance()->Set_Fade_OutIn(true, 2.4f);
	}

	m_TimeAcc += dTimeDelta;
#ifdef _DEBUG
	Key_Input(dTimeDelta);

	
#endif
}
 
HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance ||
		nullptr == m_pRenderer)
	{
		MSG_BOX("GameInstance or Renderer is null : MainApp");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(m_vTestBG)))
	{
		MSG_BOX("Failed to Clear_BackBuffef_View");
		return E_FAIL;
	}

	if(FAILED(m_pGameInstance->Clear_DepthStencil_View()))
	{
		MSG_BOX("Failed to Clear_DepthStencil_View");
		return E_FAIL;
	}

	if (FAILED(m_pRenderer->Draw_RenderObjects()))
		return E_FAIL;
	
	++m_iRenderCnt;

	if (m_TimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iRenderCnt);
		m_iRenderCnt = 0;
		m_TimeAcc = 0.0;
	}

	if (true == m_isRenderFPS)
	{
		if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), _float2(0.5f, 0.5f))))
			return E_FAIL;
	}
	#ifdef _DEBUG

	if (m_isRenderDebugInfo) {

		if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_KR"), TEXT("Num0 To OnOff StaticCam"), _float2(0.f, 620.f), _float2(0.5f, 0.5f))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_KR"), TEXT("Num9 To OnOff LockMouse"), _float2(0.f, 640.f), _float2(0.5f, 0.5f))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_KR"), TEXT("F6 To OnOff FPS"), _float2(0.f, 660.f), _float2(0.5f, 0.5f))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_KR"), TEXT("F7 To OnOff RenderDebut"), _float2(0.f, 680.f), _float2(0.5f, 0.5f))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_KR"), TEXT("F8 To OnOff RenderTarget"), _float2(0.f, 700.f), _float2(0.5f, 0.5f))))
			return E_FAIL;
	}
	
#endif // _DEBUG

	if(FAILED(m_pGameInstance->Present()))
	{
		//MSG_BOX("Failed to Present");
		return E_FAIL;
	}

	return S_OK;
}


#ifdef _DEBUG
void CMainApp::Key_Input(_double dTimeDelta)
{
	
	if (m_pGameInstance->Get_DIKeyDown(DIK_F6))
		m_isRenderFPS = !m_isRenderFPS;

	if (m_pGameInstance->Get_DIKeyDown(DIK_F7))
		m_pRenderer->OnOff_RenderDebug();

	if (m_pGameInstance->Get_DIKeyDown(DIK_F8))
		m_pRenderer->OnOff_RenderTarget();
	
	if (m_pGameInstance->Get_DIKeyDown(DIK_LCONTROL))
		m_isRenderDebugInfo = !m_isRenderDebugInfo;

}
#endif // _DEBUG


HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
	{
		MSG_BOX("GameInstance is null : MainApp");
		return E_FAIL;
	}

	/* Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Transform");
		return E_FAIL;
	}

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Renderer");
		return E_FAIL;
	}

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), 
				CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_VIBuffer_Rect");
		return E_FAIL;
	}

	/* Protoype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECL::Elements, VTXTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxTex");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.png"), 3))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Texture_BackGround");
		return E_FAIL;
	}

	Load_Effect_Resources();

#pragma region UI
	/* Protoype_GameObject_Title*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Title"),
		CTitle::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Title");
		return E_FAIL;
	}
	/* Protoype_GameObject_Loading*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading"),
		CLoading::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Loading");
		return E_FAIL;
	}
	/* Prototype_GameObject_Fade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fade"),
		CFade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Option */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Option"),
		COption::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Option

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Option/Option_%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Option/Cursor_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option_Cloud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Option/Cloud_%d.png"), 4))))
		return E_FAIL;


#pragma endregion

#pragma region Battle_Manager

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle_Signal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Manager/Signal_%d.png"),2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle_Rank"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Manager/Rank_%d.png"), 6))))
		return E_FAIL;
#pragma endregion

#pragma region Skill_Name

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skill_Name"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Skill_Name/Skill_Name.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region Title_UI

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title/Title_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title_Select"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title/Title_Select_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title_Eff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title/Eff_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title_Ink"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Background_Eff/Ink/tile0%d.png"), 48))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title_Yellow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Background_Eff/Start_Particle/Start_Particles_00%d.png"),101))))
		return E_FAIL;
#pragma endregion

#pragma region Loading_UI

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/Loading_%d.png"), 11))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Door"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/Door/Door_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Walk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/Walk/Walk_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Fade/Black_%d.png"),2))))
		return E_FAIL;

#pragma endregion

#pragma region Pause_UI

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pause"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pause/Pause_%d.png"), 4))))
		return E_FAIL;


#pragma endregion

#pragma region Story_Board_UI

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Bg_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Bg_Deco"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Bg_Deco_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Bg_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Bg_Mask_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Cloud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Cloud_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Cursor.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Icon_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Line"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Line_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Mini_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Mini_Title.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Story_Board/Title_%d.png"), 5))))
		return E_FAIL;



#pragma endregion

#pragma region Player_Battle_Bar_UI	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Bar/P/P_Frame.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Face"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Face/P/C_%d.dds"), 5))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Name"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Name/P/N_%d.dds"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Bar/P/Hp_%d.png"), 4))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Mp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Bar/P/Mp_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Ult_Frame_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Stack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Ult_Stack_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Bar_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Ult_Bar_Color_0.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Combo_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Combo/Combo_Frame_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Num_Combo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Combo/Num_Combo%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Stack_Num"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Ult_Num_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Stock_Eff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Eff/Ult_Stock_Eff_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Awk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Eff/Eff_Awk_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Swk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Eff/Eff_Swk_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Ult_Flower"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Ult_Gauge_Flower.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Lang_Ult"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Lang_Ult_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Battle_Eff_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Ult_Bar/Eff/Eff_Gauge_%d.png"), 2))))
		return E_FAIL;

#pragma endregion

#pragma region Boss_Battle_Bar_UI	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Battle_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Bar/B/B_Frame.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Battle_Face"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Face/B/C_%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Battle_Name"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Name/B/N_%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Battle_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Bar/B/Hp_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_World_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Battle_Bar/M/Hp_%d.dds"), 4))))
		return E_FAIL;

#pragma endregion

#pragma region FIcon_UI	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/FIcon/FIcon_Main_%d.dds"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_Mini"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/FIcon/FIcon_Mini_%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_Talk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/FIcon/FIcon_Talk_%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_LockOn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/FIcon/FIcon_LockOn_%d.dds"), 2))))
		return E_FAIL;

#pragma endregion

#pragma region Interaction_UI	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Interaction"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Interaction/Interaction_%d.dds"), 2))))
		return E_FAIL;

#pragma endregion

#pragma region Dialog_UI	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dialog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Dialog/Dialog_%d.png"), 3))))
		return E_FAIL;

#pragma endregion

#pragma region Mission_UI	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mission"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Mission/Mission_%d.dds"), 6))))
		return E_FAIL;

#pragma endregion

#pragma region Map_UI	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mini_Map"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Map/Mini/Mini_%d.dds"), 8))))
		return E_FAIL;

#pragma endregion	

#pragma region Paper_UI	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Paper"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Paper/Paper.png")))))
		return E_FAIL;

#pragma endregion	

#pragma region Jump_UI	

	

#pragma endregion	

#pragma region MiniGame_UI	

	

#pragma endregion	






#pragma endregion


#pragma region Navigation

	// Village Map
	/* Prototype_Component_Navigation_Village_MainRoad1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_MainRoad1"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_MainRoad1.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_MainRoad1");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_Village_MainRoad2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_MainRoad2"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_MainRoad2.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_MainRoad2");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_Village_InsideWall1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_InsideWall1"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_InsideWall1.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_InsideWall1");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_Village_InsideWall2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_InsideWall2"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_InsideWall2.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_InsideWall2");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_Village_Roof */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Roof"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_Roof.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_Roof");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_Village_Wall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Wall"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_Wall.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_Wall");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_Village_Battle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Battle"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Village/Navi_Battle.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Village_Battle");
		return E_FAIL;
	}

	// House Map
	/* Prototype_Component_Navigation_House_0_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_0_0"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Room/Navi_Room_0_0.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_House_0_0");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_House_1_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_1_0"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Room/Navi_Room_1_0.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_House_1_0");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_House_1_1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_1_1"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Room/Navi_Room_1_1.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_House_1_1");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_House_2_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_2_0"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Room/Navi_Room_2_0.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_House_2_0");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_House_3_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_3_0"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Room/Navi_Room_3_0.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_House_3_0");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation_House_4_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_4_0"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Room/Navi_Room_4_0.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_House_4_0");
		return E_FAIL;
	}

	// ========================Train Map======================================
	/* Prototype_Component_Navigation_Train*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Train"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Train/Navi_Train.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Train");
		return E_FAIL;
	}

	// ========================Acaza Map====================================

	/* Prototype_Component_Navigation_Acaza */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Acaza"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NaviMesh/Acaza/Navi_Acaza.dat")))))
	{
		MSG_BOX("Failed to Add Prototype_Component_Navigation_Acaza");
		return E_FAIL;
	}


#pragma endregion

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::SetUp_StartLevel(LEVELID eLevelID)
{
	if (LEVEL_STATIC == eLevelID || 
		LEVEL_LOADING == eLevelID)
	{
		MSG_BOX("Out of Range : LEVELID");
		return E_FAIL;
	}

	CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID);
	if (nullptr == pLevel)
	{
		MSG_BOX("Level is null : MainApp");
		return E_FAIL;
	}

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);
}

HRESULT CMainApp::Load_Effect_Resources()
{
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA("../Bin/Resources/Textures/Effect/*.dds", &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char* filename = new char[MAX_PATH];
				strcpy_s(filename, MAX_PATH, fd.cFileName);
				//m_listFileName.push_back(filename);


				char szFull[MAX_PATH] = "";

				strcpy_s(szFull, "../Bin/Resources/Textures/Effect/");
				strcat_s(szFull, filename);

				_tchar FullFilePath[MAX_PATH] = TEXT("");

				_int iLength = (_int)strlen(szFull) + 1;

				MultiByteToWideChar(CP_ACP, 0, szFull, (_int)iLength, FullFilePath, MAX_PATH);

				const _tchar* RealFilePath = FullFilePath;

				_splitpath_s(filename, nullptr, 0, nullptr, 0, filename, MAX_PATH, nullptr, 0);

				char szProtoTag[MAX_PATH] = "";

				strcpy_s(szProtoTag, "Prototype_Component_Texture_");
				strcat_s(szProtoTag, filename);

				iLength = (_int)strlen(szProtoTag) + 1;

				_tchar* RealProtoTag = new _tchar[iLength];

				MultiByteToWideChar(CP_ACP, 0, szProtoTag, (_int)iLength, RealProtoTag, MAX_PATH);

				_matrix		PivotMatrix = XMMatrixIdentity();

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, RealProtoTag,
					CTexture::Create(m_pDevice, m_pContext, RealFilePath))))
					return E_FAIL;

				m_ProtoTag.emplace_back(RealProtoTag);

				delete[] filename;

			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}

	WIN32_FIND_DATAA fd2;
	HANDLE hFind2 = ::FindFirstFileA("../Bin/Resources/Textures/Effect/Ramp/*.dds", &fd2);
	if (hFind2 != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char* filename = new char[MAX_PATH];
				strcpy_s(filename, MAX_PATH, fd2.cFileName);
				//m_listFileName.push_back(filename);


				char szFull[MAX_PATH] = "";

				strcpy_s(szFull, "../Bin/Resources/Textures/Effect/Ramp/");
				strcat_s(szFull, filename);

				_tchar FullFilePath[MAX_PATH] = TEXT("");

				_int iLength = (_int)strlen(szFull) + 1;

				MultiByteToWideChar(CP_ACP, 0, szFull, (_int)iLength, FullFilePath, MAX_PATH);

				const _tchar* RealFilePath = FullFilePath;

				_splitpath_s(filename, nullptr, 0, nullptr, 0, filename, MAX_PATH, nullptr, 0);

				char szProtoTag[MAX_PATH] = "";

				strcpy_s(szProtoTag, "Prototype_Component_Texture_");
				strcat_s(szProtoTag, filename);

				iLength = (_int)strlen(szProtoTag) + 1;

				_tchar* RealProtoTag = new _tchar[iLength];

				MultiByteToWideChar(CP_ACP, 0, szProtoTag, (_int)iLength, RealProtoTag, MAX_PATH);

				_matrix		PivotMatrix = XMMatrixIdentity();

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, RealProtoTag,
					/*pTexture = */CTexture::Create(m_pDevice, m_pContext, RealFilePath))))
					return E_FAIL;

				m_ProtoTag.emplace_back(RealProtoTag);

				delete[] filename;

			}
		} while (::FindNextFileA(hFind2, &fd2));
		::FindClose(hFind2);
	}

	WIN32_FIND_DATAA fd3;
	HANDLE hFind3 = ::FindFirstFileA("../Bin/Resources/Models/Effect/*.bin", &fd3);
	if (hFind3 != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd3.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char* filename = new char[MAX_PATH];
				strcpy_s(filename, MAX_PATH, fd3.cFileName);
				//m_listFileName.push_back(filename);


				char szFull[MAX_PATH] = "";

				strcpy_s(szFull, "../Bin/Resources/Models/Effect/");
				strcat_s(szFull, filename);

				_splitpath_s(filename, nullptr, 0, nullptr, 0, filename, MAX_PATH, nullptr, 0);

				char szProtoTag[MAX_PATH] = "";

				strcpy_s(szProtoTag, "Prototype_Component_Model_");
				strcat_s(szProtoTag, filename);

				_int iLength = (_int)strlen(szProtoTag) + 1;

				_tchar* RealProtoTag = new _tchar[iLength];

				MultiByteToWideChar(CP_ACP, 0, szProtoTag, (_int)iLength, RealProtoTag, MAX_PATH);

				_matrix		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, RealProtoTag,
					/*pModel = */CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFull, PivotMatrix))))
					return E_FAIL;

				m_ProtoTag.emplace_back(RealProtoTag);

				delete[] filename;

			}
		} while (::FindNextFileA(hFind3, &fd3));
		::FindClose(hFind3);
	}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	for (auto ProtoTag : m_ProtoTag)
		Safe_Delete_Array(ProtoTag);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	CSoundMgr::Get_Instance()->StopAll();

	CAtkCollManager::GetInstance()->DestroyInstance();
	CMonsterManager::GetInstance()->DestroyInstance();
	CSwampManager::GetInstance()->DestroyInstance();
	CPlayerManager::GetInstance()->DestroyInstance();

	CColliderManager::GetInstance()->DestroyInstance();
	CEffectPlayer::Get_Instance()->Destroy_Instance();
	CSoundMgr::Get_Instance()->Destroy_Instance();
	CDialogManager::GetInstance()->DestroyInstance();
	CMissionManager::GetInstance()->DestroyInstance();
	CTitleManager::GetInstance()->DestroyInstance();
	CStoryManager::GetInstance()->DestroyInstance();
	CFadeManager::GetInstance()->DestroyInstance();
	CMiniMapManager::GetInstance()->DestroyInstance();
	CCameraManager::GetInstance()->DestroyInstance();
	CBattle_UI_Manager::GetInstance()->DestroyInstance();
	COptionManager::GetInstance()->DestroyInstance();

	CGameInstance::Release_Engine();
}
