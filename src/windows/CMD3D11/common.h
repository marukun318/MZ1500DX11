#pragma once

#ifndef __D3D11_COMMON_H__
#define __D3D11_COMMON_H__

#include	"../targetver.h"

#ifndef STRICT
#define STRICT						// �^�`�F�b�N�������ɍs��
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN			// �w�b�_�[���炠�܂�g���Ȃ��֐�������
#endif

#include	<cstdint>

#include	<windows.h>
#include	<d3d11_1.h>
#include	<atlbase.h>				// CComPtr �̐錾���܂�
#include	<wrl\client.h>

#endif // ! __D3D11_COMMON_H__

