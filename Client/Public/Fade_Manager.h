#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CFadeManager final : public CBase
{
	DECLARE_SINGLETON(CFadeManager)

private:
	CFadeManager();
	virtual ~CFadeManager() = default;

public:
	HRESULT Inititalize();
	void Tick();

	void Fade_In();
	void Fade_Out();
	void Fade_OutIn();

	void Fall_Ink();

	void Set_Fade_In(_bool Fade);
	void Set_Fade_Out(_bool Fade);
	void Set_Fade_OutIn(_bool Fade, _float Time = 0.f);
	void Set_Ink_In(_bool Fade);

	_bool Get_Fade_In();
	_bool Get_Fade_Out();
	_bool Get_Fade_OutIn();
	_bool Get_Ink_In();

	void Set_Fade_In_Done(_bool Fade);
	void Set_Fade_Out_Done(_bool Fade);
	void Set_Fade_OutIn_Done(_bool Fade);
	void Set_Ink_In_Done(_bool Fade);

	_bool Get_Fade_In_Done();
	_bool Get_Fade_Out_Done();
	_bool Get_Fade_OutIn_Done();
	_bool Get_Ink_In_Done();

	_bool Get_Is_Battle();
	void  Set_Is_Battle(_bool is);

	_float Get_Delay_Time();

	_bool Get_Is_House_Boss_On();
	void  Set_Is_House_Boss_On(_bool is);
	
public:
	void Set_Fade_Color(_bool Type);
	_bool Get_Fade_Color();

public:
	void Set_Is_Village_Battle_Start(_bool Is);
	_bool Get_Is_Village_Battle_Start();

	void Set_Is_House_Monster_Battle_Start(_bool Is);
	_bool Get_Is_House_Monster_Battle_Start();

	void Set_Is_House_Boss_Battle_Start(_bool Is);
	_bool Get_Is_House_Boss_Battle_Start();

	void Set_Is_Train_Battle_Start(_bool Is);
	_bool Get_Is_Train_Battle_Start();

	void Set_Is_Final_Battle_Start(_bool Is);
	_bool Get_Is_Final_Battle_Start();

	void Set_Is_House_Monster_Encounter(_bool Is);
	_bool Get_Is_House_Monster_Encounter();

	void Set_Is_House_Boss_Encounter(_bool Is);
	_bool Get_Is_House_Boss_Encounter();
private:
	_bool m_Is_Fade_In = { false };
	_bool m_Is_Fade_Out = { false };
	_bool m_Is_Fade_OutIn = { false };
	_bool m_Is_Ink_In = { false };


	_bool m_Is_Fade_In_Done = { false };
	_bool m_Is_Fade_Out_Done = { false };
	_bool m_Is_Fade_OutIn_Done = { false };
	_bool m_Is_Ink_In_Done = { false };


	_bool m_Is_Battle = { false };
	_bool m_Is_House_Boss_On = { false };


	_uint m_Ink_Sprite = { 0 };

	_float m_Delay_Time = { 0.f };

	_bool m_Fade_Color = { false };


// 배틀 시작 트리거
	_bool m_Is_Village_Battle_Start = { false };
	_bool m_Is_House_Monster_Battle_Start = { false };
	_bool m_Is_House_Boss_Battle_Start = { false };
	_bool m_Is_Train_Battle_Start = { false };
	_bool m_Is_Final_Battle_Start = { false };


	_bool m_Is_House_Monster_Encounter = { false };
	_bool m_Is_House_Boss_Encounter = { false };

public:
	virtual void Free() override;
};

END