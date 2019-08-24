// Direct3D11 Texture Wrapper class
// written by Takeshi Maruyama
//

#include "CMD3D11Tex.h"

#define SAFE_RELEASE(x) if( (x) != nullptr ){ (x)->Release(); x = nullptr; }

extern CMD3D11 d3d11;

extern CRITICAL_SECTION cs;

// Constructor
CMD3D11Tex::CMD3D11Tex(int w, int h, DXGI_FORMAT format, bool bCpu, bool bStaging)
{
	InitTexture(w, h, format, bCpu, bStaging);
}

CMD3D11Tex::CMD3D11Tex(int w, int h)
{
	InitTexture(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, false, false);
}

// Destructor
CMD3D11Tex::~CMD3D11Tex()
{
	SAFE_RELEASE(m_pUAV);
	SAFE_RELEASE(m_pShaderResView);
	SAFE_RELEASE(m_pOffScreen);
}

//
void CMD3D11Tex::InitTexture(int w, int h, DXGI_FORMAT format, bool bCpu, bool bStaging)
{
	HRESULT hr = S_OK;
	ID3D11Device* d3ddevice = d3d11.GetDevice();

	m_bCpu = bCpu;
	m_bStaging = bStaging;

	// ���A����������
	m_width = w;
	m_height = h;

	DXGI_FORMAT fsrv, fuav;

	// �R���s���[�g�V�F�[�_�Ƃ��g���p
	if (format == DXGI_FORMAT_R8G8B8A8_TYPELESS) {
		fsrv = DXGI_FORMAT_R8G8B8A8_UNORM;
		fuav = DXGI_FORMAT_R32_UINT;
	}
	else {
		fsrv = format;
		fuav = format;
	}

	// �I�t�X�N���[���e�N�X�`�� �� �쐬
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = w;		// ��
	descDepth.Height = h;		// ����
	descDepth.Format = format; // DXGI_FORMAT_R8G8B8A8_UNORM; // �t�H�[�}�b�g RGBA8
	descDepth.MipLevels = 1;		// �~�b�v�}�b�v ���x����
	descDepth.ArraySize = 1;		// �z��T�C�Y
	descDepth.SampleDesc.Count = 1;    // �}���`�T���v�����O�̐ݒ�
	descDepth.SampleDesc.Quality = 0;    // �}���`�T���v�����O�̕i��

	if (bCpu)
	{
		descDepth.Usage = D3D11_USAGE_DYNAMIC;		// �g�p���@�@�_�C�i�~�b�N
		descDepth.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		descDepth.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else
	if (bStaging)
	{
		descDepth.Usage = D3D11_USAGE_STAGING;		// �g�p���@�@�X�e�[�W���O
		descDepth.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		descDepth.BindFlags = 0;
	}
	else
	{
		descDepth.Usage = D3D11_USAGE_DEFAULT;		// �g�p���@�@�f�t�H���g
		descDepth.CPUAccessFlags = 0;
		descDepth.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}
	descDepth.MiscFlags = 0;    // ���̑��̐ݒ�Ȃ�
	hr = d3ddevice->CreateTexture2D(
		&descDepth,				// �쐬����2D�e�N�X�`��
		nullptr, //&subres,		// �ӂ���nullptr
		&m_pOffScreen			// �쐬�����e�N�X�`�����󂯎��
	);
	if (FAILED(hr))
		return;

	//https://www.gamedev.net/forums/topic/662238-dxgi-usage-unordered-access-and-rwtexture2d-in-pixel-shader/

	//
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_RESOURCE_DIMENSION type;
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	m_pOffScreen->GetType(&type);
	ID3D11Texture2D *pTexture2D = nullptr;

	if (descDepth.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
		switch (type) {
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			pTexture2D = (ID3D11Texture2D*)m_pOffScreen;
			pTexture2D->GetDesc(&desc);
			memset(&srvDesc, 0, sizeof(srvDesc));
			srvDesc.Format = fsrv;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = desc.MipLevels - 1;
			//		srvDesc.Texture2D.MipLevels = 1;
			//		srvDesc.Texture2D.MostDetailedMip = 0;
			// �V�F�[�_�[���\�[�X�r���[�@�쐬
			d3ddevice->CreateShaderResourceView(m_pOffScreen, &srvDesc, &m_pShaderResView);
			break;

		default:
			//...
			// Error
			break;
		}
	}

	if (!bCpu) {
		if (descDepth.BindFlags & D3D11_BIND_UNORDERED_ACCESS) {
			// UnorderedAccessView
			m_pOffScreen->GetDesc(&desc);

			D3D11_UNORDERED_ACCESS_VIEW_DESC    uavDesc = {};
			uavDesc.Format = fuav;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;

			d3ddevice->CreateUnorderedAccessView(m_pOffScreen, &uavDesc, &m_pUAV);
		}
	}

}


// DYNAMIC ����Ȃ��Ɨ�����
void CMD3D11Tex::Map(uint32_t *src)
{
	D3D11_MAPPED_SUBRESOURCE mappedTex;
	if (m_pOffScreen == nullptr) return;

	D3D11_MAP mapflag = D3D11_MAP_WRITE_DISCARD;

	if (m_bStaging) {
		mapflag = D3D11_MAP_READ_WRITE;
	}

	HRESULT result = d3d11.m_pImmediateContext->Map(m_pOffScreen, 0,mapflag, 0, &mappedTex);
	uint32_t* pTexels = (uint32_t*)mappedTex.pData;

	int rowspan = m_width * sizeof(uint32_t);
	BYTE* mappedData = reinterpret_cast<BYTE*>(mappedTex.pData);
	BYTE* buffer = reinterpret_cast<BYTE*>(src);

#pragma loop(hint_parallel(8))
	for (UINT i = 0; i < m_height; ++i)
	{
		memcpy(mappedData, buffer, rowspan);
		mappedData += mappedTex.RowPitch;
		buffer += rowspan;
	}

	d3d11.m_pImmediateContext->Unmap(m_pOffScreen, 0);
}
// DEFAULT ����Ȃ��Ɨ�����
void CMD3D11Tex::UpdateRect(uint32_t *src)
{
	if (m_pOffScreen == nullptr) return;

	D3D11_BOX box;
	box.front = 0;
	box.back = 1;
	box.left = 0;
	box.right = m_width;
	box.top = 0;
	box.bottom = m_height;

	d3d11.m_pImmediateContext->UpdateSubresource(m_pOffScreen, 0, &box, src, m_width * sizeof(uint32_t), m_width * m_height * sizeof(uint32_t));

}

// DYNAMIC ����Ȃ��Ɨ�����
void CMD3D11Tex::Map(uint16_t *src)
{
	if (m_pOffScreen == nullptr) return;

//	EnterCriticalSection(&cs);

	D3D11_MAPPED_SUBRESOURCE mappedTex;
	HRESULT result = d3d11.m_pImmediateContext->Map(m_pOffScreen, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTex);
	uint16_t* pTexels = (uint16_t*)mappedTex.pData;

#pragma loop(hint_parallel(8))
	for (int i = 0; i<(m_width * m_height); i++) {
		pTexels[i] = src[i];
	}

	d3d11.m_pImmediateContext->Unmap(m_pOffScreen, 0);

//	LeaveCriticalSection(&cs);
}

// DEFAULT ����Ȃ��Ɨ�����
void CMD3D11Tex::Update(uint32_t *src)
{
	if (m_pOffScreen == nullptr || src == nullptr) return;

	d3d11.m_pImmediateContext->UpdateSubresource(m_pOffScreen, 0, nullptr, (UINT32*)src, m_width * sizeof(uint32_t), 0);
}

// DEFAULT ����Ȃ��Ɨ�����
void CMD3D11Tex::Update(uint16_t *src)
{
	if (m_pOffScreen == nullptr || src == nullptr) return;

	d3d11.m_pImmediateContext->UpdateSubresource(m_pOffScreen, 0, nullptr, (UINT16*)src, m_width * sizeof(uint16_t), 0);
}
