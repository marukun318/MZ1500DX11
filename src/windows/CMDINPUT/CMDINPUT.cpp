#include <windows.h>
#include "CMDINPUT.h"

#pragma comment(lib, "dinput8.lib")

#define SAFE_RELEASE(p)			{ if (p) { (p)->Release(); (p)=nullptr; } }
#define KEYDOWN(key)			(key_buffer[key] & 0x80)

//
LPDIRECTINPUT8			pDInput = nullptr;     // DirectInput
LPDIRECTINPUTDEVICE8	pDIDev[DINPUT_PAD_MAX] = { nullptr };     // DirectInput �f�o�C�X
DIDEVCAPS				diDevCaps[DINPUT_PAD_MAX] = { 0 };     // �W���C�X�e�B�b�N�̔\��
DIJOYSTATE2				JoyState[DINPUT_PAD_MAX] = { 0 };

static int pad_num = 0;

static HWND _hWnd = nullptr;

// �G���[���b�Z�[�W
static void ErrorMessageBox(const wchar_t* msg)
{
	MessageBox(nullptr, msg, L"Error", MB_OK);
}

// �����[�h�̐ݒ�
static BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context)
{
	HWND hDlg = (HWND)context;

	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -128;
	propRange.lMax = 127;

	LPDIRECTINPUTDEVICE8 pDev = pDIDev[pad_num];

	// Set the range for the axis
	if (FAILED(pDev->SetProperty(DIPROP_RANGE, &propRange.diph))) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

// �W���C�X�e�B�b�N��񋓂���֐�
static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	HRESULT hr;

	// �񋓂��ꂽ�W���C�X�e�B�b�N�ւ̃C���^�[�t�F�C�X���擾����B
	hr = pDInput->CreateDevice(pdidInstance->guidInstance, &pDIDev[pad_num], nullptr);
	if (FAILED(hr))
		return DIENUM_CONTINUE;

	// �W���C�X�e�B�b�N�̔\�͂𒲂ׂ�
	memset(&diDevCaps[pad_num], 0, sizeof(DIDEVCAPS));
	diDevCaps[pad_num].dwSize = sizeof(DIDEVCAPS);
	hr = pDIDev[pad_num]->GetCapabilities(&diDevCaps[pad_num]);
	if (FAILED(hr)) {
		// �W���C�X�e�B�b�N�\�͂̎擾�Ɏ��s
		SAFE_RELEASE(pDIDev[pad_num]);
		return DIENUM_CONTINUE;
	}

	// �f�[�^�`����ݒ�
	hr = pDIDev[pad_num]->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr)) {
		//		SysPrintf(L"c_dfDIJoystick2�`���̐ݒ�Ɏ��s\n");
		SAFE_RELEASE(pDIDev[pad_num]);
		return DIENUM_CONTINUE;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = pDIDev[pad_num]->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		//		SysPrintf(L"�t�H�A�O���E���h����r�����[�h�̐ݒ�Ɏ��s\n");
		SAFE_RELEASE(pDIDev[pad_num]);
		return DIENUM_CONTINUE;
	}

	// �R�[���o�b�N�֐����g���Ċe���̃��[�h��ݒ�
	hr = pDIDev[pad_num]->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS);
	if (FAILED(hr)) {
		//		SysPrintf(L"�����[�h�̐ݒ�Ɏ��s\n");
		SAFE_RELEASE(pDIDev[pad_num]);
		return DIENUM_CONTINUE;
	}

	// ���͐���J�n
	pDIDev[pad_num]->Acquire();

	pad_num++;

	return (pad_num < DINPUT_PAD_MAX) ? DIENUM_CONTINUE : DIENUM_STOP;
}

// �R���X�g���N�^
CMDINPUT::CMDINPUT()
{
}

// �f�X�g���N�^
CMDINPUT::~CMDINPUT()
{
}

// ������
BOOL CMDINPUT::Init(HINSTANCE inst, HWND hWnd) {
	HRESULT hr = 0;

	_hWnd = hWnd;

	// DirectInput�̍쐬
	hr = DirectInput8Create(inst, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&pDInput, NULL);
	if (FAILED(hr)) {
		ErrorMessageBox(L"DirectInput8�I�u�W�F�N�g�̍쐬�Ɏ��s");
		return FALSE;
	}

	// �f�o�C�X��񋓂��č쐬
	pad_num = 0;
	hr = pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
		NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr)) {
		ErrorMessageBox(L"DirectInputDevice8�f�o�C�X�̗񋓂Ɏ��s");
		return hr;
	}

	//	SysPrintf(L"DirectInputDevice8=%d\n", pad_num);

		// �L�[�{�[�h�f�o�C�X�̍쐬
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIKey, nullptr);
	if FAILED(hr) {
		ErrorMessageBox(L"Keyboard Device Failure");
		return FALSE;
	}
	hr = pDIKey->SetDataFormat(&c_dfDIKeyboard);
	if FAILED(hr) {
		ErrorMessageBox(L"Keyboard Device Failure");
		return FALSE;
	}

	// Set the cooperative level 
	hr = pDIKey->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if FAILED(hr) {
		ErrorMessageBox(L"Keyboard Device Failure");
		return FALSE;
	}

	if (pDIKey) pDIKey->Acquire();

	bINIT = TRUE;

	return TRUE;
}

// 
BOOL CMDINPUT::Acquire() {

	if (!bINIT)
		return FALSE;

	for (int i = 0; i < DINPUT_PAD_MAX; i++) {
		if (pDIDev[i]) {
			pDIDev[i]->Acquire();
		}
	}

	if (pDIKey) pDIKey->Acquire();

	return TRUE;
}

//
BOOL CMDINPUT::Unacquire() {
	if (!bINIT)
		return FALSE;

	for (int i = 0; i < DINPUT_PAD_MAX; i++) {
		if (pDIDev[i]) {
			pDIDev[i]->Unacquire();
		}
	}

	if (pDIKey) pDIKey->Unacquire();

	return TRUE;
}

// ���
BOOL CMDINPUT::Cleanup() {

	for (int i = DINPUT_PAD_MAX - 1; i >= 0; i--) {
		if (pDIDev[i]) {
			pDIDev[i]->Unacquire();
			pDIDev[i]->Release();
			pDIDev[i] = nullptr;
		}
	}

	if (pDIKey) {
		pDIKey->Unacquire();
		pDIKey->Release();
		pDIKey = nullptr;
	}

	SAFE_RELEASE(pDInput);

	if (!bINIT)
		return FALSE;

	return TRUE;
}


// �X�V
HRESULT CMDINPUT::Update() {
	HRESULT hr;

	if (!bINIT)
		return E_FAIL;

	// Gamepad Update
	for (int i = 0; i < DINPUT_PAD_MAX; i++) {
		if (pDIDev[i]) {
			hr = pDIDev[i]->Poll();
			if (FAILED(hr)) {
				// DInput is telling us that the input stream has been
				// interrupted. We aren't tracking any state between polls, so
				// we don't have any special reset that needs to be done. We
				// just re-acquire and try again.
				hr = pDIDev[i]->Acquire();
				while (hr == DIERR_INPUTLOST) {
					hr = pDIDev[i]->Acquire();
				}

				// If we encounter a fatal error, return failure.
				if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
					return E_FAIL;
				}

				// If another application has control of this device, return successfully.
				// We'll just have to wait our turn to use the joystick.
				if (hr == DIERR_OTHERAPPHASPRIO) {
					return S_OK;
				}
			}

			// Get the input's device state
			if (FAILED(hr = pDIDev[i]->GetDeviceState(sizeof(DIJOYSTATE2), &JoyState[i]))) {
				return hr; // The device should have been acquired during the Poll()
			}

		}
	}

	// Keyboard Update
	hr = pDIKey->GetDeviceState(sizeof(key_buffer), (LPVOID)&key_buffer);
	if (FAILED(hr)) {
		if (hr == DIERR_INPUTLOST) {
			pDIKey->Acquire();
		}
		return E_FAIL;
	}


	return S_OK;
}

// �L�[�{�[�h�����X�V
HRESULT CMDINPUT::GetKeyState(unsigned char *kb)
{
	HRESULT hr;

	if (!bINIT)
		return E_FAIL;

	// Keyboard Update
	hr = pDIKey->GetDeviceState(sizeof(key_buffer), (LPVOID)kb);
	if (FAILED(hr)) {
		if (hr == DIERR_INPUTLOST) {
			pDIKey->Acquire();
		}
		return E_FAIL;
	}

	return S_OK;
}


// �Q�[���p�b�h�����X�V
HRESULT CMDINPUT::GetJoyState(int i)
{
	HRESULT hr;

	if (!bINIT)
		return E_FAIL;

	// Gamepad Update
	if (pDIDev[i]) {
		hr = pDIDev[i]->Poll();
		if (FAILED(hr)) {
			// DInput is telling us that the input stream has been
			// interrupted. We aren't tracking any state between polls, so
			// we don't have any special reset that needs to be done. We
			// just re-acquire and try again.
			hr = pDIDev[i]->Acquire();
			while (hr == DIERR_INPUTLOST) {
				hr = pDIDev[i]->Acquire();
			}

			// If we encounter a fatal error, return failure.
			if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
				return E_FAIL;
			}

			// If another application has control of this device, return successfully.
			// We'll just have to wait our turn to use the joystick.
			if (hr == DIERR_OTHERAPPHASPRIO) {
				return S_OK;
			}
		}

		// Get the input's device state
		if (FAILED(hr = pDIDev[i]->GetDeviceState(sizeof(DIJOYSTATE2), &JoyState[i]))) {
			return hr; // The device should have been acquired during the Poll()
		}

	}

	return S_OK;
}


// �擾
DIJOYSTATE2* CMDINPUT::GetState(int no)
{
	if (no < 0 || no >= DINPUT_PAD_MAX)
		return nullptr;

	return &JoyState[no];
}

// �擾
DWORD CMDINPUT::GetPad(int no)
{
	DWORD pad = 0;

	// GamePad ��Ԏ擾
	DIJOYSTATE2 * js2 = GetState(no);
	if (js2 != nullptr)
	{
		//		SysPrintf(L"lX=%ld lY=%ld ", js2->lX, js2->lY);
		for (int j = 0; j < 128; j++) {
			if (js2->rgbButtons[j] & 0x80) {
				//				SysPrintf(L"[%d] ", j);
			}

		}
		//		SysPrintf(L"\n");
	}

#if 0
	// Keyboard ��Ԏ擾
	bool keyflag = false;
	if (KEYDOWN(DIK_UP)) {
		SysPrintf(L"UP ");
		keyflag = true;
	}
	if (KEYDOWN(DIK_DOWN)) {
		SysPrintf(L"DOWN ");
		keyflag = true;
	}
	if (KEYDOWN(DIK_LEFT)) {
		SysPrintf(L"LEFT ");
		keyflag = true;
	}
	if (KEYDOWN(DIK_RIGHT)) {
		SysPrintf(L"RIGHT ");
		keyflag = true;
	}
	if (keyflag) {
		SysPrintf(L"\n");
	}
#endif

	return pad;
}
