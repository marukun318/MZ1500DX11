#include <windows.h>
#include "CMXINPUT.h"

#pragma comment(lib, "Xinput9_1_0.lib")

// �R���X�g���N�^
CMXINPUT::CMXINPUT()
{
	//	Init();
}

// �f�X�g���N�^
CMXINPUT::~CMXINPUT()
{
	//	Cleanup();
}

/*
* �ڑ�����Ă��邩���ׂA�R���g���[���[�̏�Ԃ��擾����
*/
HRESULT CMXINPUT::UpdateControllerState()
{
	DWORD dwResult = 0;
	pCONTROLER_STATE pC;

	for (DWORD i = 0; i < XI_MAX_CONTROLLERS; i++)
	{
		pC = &GAME_PAD[i];

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &(pC->state));

		if (dwResult == ERROR_SUCCESS)
			pC->bConnected = true;
		else
			pC->bConnected = false;
	}

	return S_OK;
}

/*
* XINPUT������
*/
bool CMXINPUT::Init()
{
	memset(GAME_PAD, 0, sizeof(CONTROLER_STATE) * XI_MAX_CONTROLLERS);

	return true;
}

/*
* XINPUT���
*/
bool CMXINPUT::Cleanup()
{
	// �U����~
	XINPUT_VIBRATION vibration = { };

	for (int i = 0; i < XI_MAX_CONTROLLERS; i++) {
		XInputSetState(i, &vibration);
	}

	return true;
}

/*
*  �X�V
*/
bool CMXINPUT::Update()
{
	HRESULT result;

	result = UpdateControllerState();

	return true;
}

/*
* �Q�[���p�b�h���ڑ�����Ă��邩
*
* no = GamePad 0-3
*/
bool CMXINPUT::Is_GamePad_Connected(int no)
{
	bool result = false;
	pCONTROLER_STATE pC;

	if (no >= 0 && no < XI_MAX_CONTROLLERS)
	{
		pC = &GAME_PAD[no];
		if (pC->bConnected)
			result = true;
	}

	return result;
}

/*
* �Q�[���p�b�h�̐����͏��擾
*
* no = GamePad 0-3
*/
DWORD CMXINPUT::Get_GamePad_RAW(int no)
{
	DWORD result = 0;
	pCONTROLER_STATE pC;
	XINPUT_GAMEPAD * pGamepad;
	WORD buttons;

	if (no >= 0 && no < XI_MAX_CONTROLLERS)
	{
		pC = &GAME_PAD[no];
		if (!pC->bConnected)		// �ڑ�����Ă��Ȃ�
			return result;

		pGamepad = &pC->state.Gamepad;

		// Zero value if thumbsticks are within the dead zone 
		if ((pGamepad->sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			pGamepad->sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(pGamepad->sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				pGamepad->sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			pGamepad->sThumbLX = 0;
			pGamepad->sThumbLY = 0;
		}

		if ((pGamepad->sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			pGamepad->sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(pGamepad->sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				pGamepad->sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			pGamepad->sThumbRX = 0;
			pGamepad->sThumbRY = 0;
		}

		buttons = pGamepad->wButtons;

		// �A�i���O�����L�[
		if (pGamepad->sThumbLY > XI_Threshold) result |= XPAD_RAW_UP;
		if (pGamepad->sThumbLY < -XI_Threshold) result |= XPAD_RAW_DOWN;
		if (pGamepad->sThumbLX > XI_Threshold) result |= XPAD_RAW_RIGHT;
		if (pGamepad->sThumbLX < -XI_Threshold) result |= XPAD_RAW_LEFT;

		// �f�W�^�������L�[
		if (buttons & XINPUT_GAMEPAD_DPAD_UP) result |= XPAD_RAW_UP;
		if (buttons & XINPUT_GAMEPAD_DPAD_DOWN) result |= XPAD_RAW_DOWN;
		if (buttons & XINPUT_GAMEPAD_DPAD_LEFT) result |= XPAD_RAW_LEFT;
		if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) result |= XPAD_RAW_RIGHT;

		// �{�^��
		if (buttons & XINPUT_GAMEPAD_A) result |= XPAD_RAW_A;
		if (buttons & XINPUT_GAMEPAD_B) result |= XPAD_RAW_B;
		if (buttons & XINPUT_GAMEPAD_X) result |= XPAD_RAW_X;
		if (buttons & XINPUT_GAMEPAD_Y) result |= XPAD_RAW_Y;
		if (buttons & XINPUT_GAMEPAD_START) result |= XPAD_RAW_START;
		if (buttons & XINPUT_GAMEPAD_BACK)  result |= XPAD_RAW_BACK;
		//		if (buttons & XINPUT_GAMEPAD_LEFT_THUMB) { } // TODO:���A�i���O�����L�[���p�b�h���ɉ������ꍇ
		//		if (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) { } // TODO:�E�A�i���O�����L�[���p�b�h���ɉ������ꍇ
		if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) result |= XPAD_RAW_L1;
		if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) result |= XPAD_RAW_R1;
		// ���E�̃A�i���O�g���K�[
		if (pC->state.Gamepad.bLeftTrigger >= 128) { result |= XPAD_RAW_L2; }
		if (pC->state.Gamepad.bRightTrigger >= 128) { result |= XPAD_RAW_R2; }

	}

	return result;
}
