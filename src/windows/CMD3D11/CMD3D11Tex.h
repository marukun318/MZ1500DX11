// Direct3D11 Texture Wrapper class
// written by Takeshi Maruyama
//

#pragma once

#ifndef __CMD311Tex_H__
#define __CMD311Tex_H__

#ifndef STRICT
#define STRICT						 // �^�`�F�b�N�������ɍs��
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN			// �w�b�_�[���炠�܂�g���Ȃ��֐�������
#endif

#include	<cstdint>

#include	<windows.h>
#include	<d3d11_1.h>
#include	<atlbase.h>				// CComPtr �̐錾���܂�
#include	<wrl\client.h>

#include	"CMD3D11.h"

class CMD3D11Tex
{
public:
	CMD3D11Tex(int, int);
	CMD3D11Tex(int w, int h, DXGI_FORMAT, bool, bool);

	~CMD3D11Tex();

	void Map(uint32_t *);
	void Map2(uint32_t *);
	void Map2(uint16_t *);
	void Update(uint16_t *, ID3D11DeviceContext*);
	void Update(uint32_t *, ID3D11DeviceContext*);

	ID3D11ShaderResourceView*  GetShaderResouceView() { return m_pShaderResView;  }

	ID3D11Texture2D*			m_pOffScreen = nullptr;				// �I�t�X�N���[���e�N�X�`��
	ID3D11ShaderResourceView*	m_pShaderResView = nullptr;			// �e�N�X�`�� �V�F�[�_�[���\�[�X�r���[
	ID3D11UnorderedAccessView*	m_pUAV = nullptr;					// �e�N�X�`�� �A���I�[�_�|�h�A�N�Z�X�r���[
	int m_width;
	int m_height;

private:
	bool m_bCpu;
	bool m_bStaging;
	void InitTexture(int w, int h, DXGI_FORMAT, bool, bool);

};


#endif // ! __CMD311Tex_H__
