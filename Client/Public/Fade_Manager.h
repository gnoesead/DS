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
	void Set_Fade_OutIn(_bool Fade);

	_bool Get_Fade_In();
	_bool Get_Fade_Out();
	_bool Get_Fade_OutIn();

	void Set_Fade_In_Done(_bool Fade);
	void Set_Fade_Out_Done(_bool Fade);
	void Set_Fade_OutIn_Done(_bool Fade);

	_bool Get_Fade_In_Done();
	_bool Get_Fade_Out_Done();
	_bool Get_Fade_OutIn_Done();

	
private:
	_bool m_Is_Fade_In = { false };
	_bool m_Is_Fade_Out = { false };
	_bool m_Is_Fade_OutIn = { false };

	_bool m_Is_Fade_In_Done = { false };
	_bool m_Is_Fade_Out_Done = { false };
	_bool m_Is_Fade_OutIn_Done = { false };

	_uint m_Ink_Sprite = { 0 };

public:
	virtual void Free() override;
};

END