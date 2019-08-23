// Direct3D11 Render Texture View Wrapper class
// written by Takeshi Maruyama
//

#include "CMD3D11Rtv.h"

#define SAFE_RELEASE(x) if( (x) != nullptr ){ (x)->Release(); x = nullptr; }

extern CMD3D11 d3d11;

// Constructor
CMD3D11Rtv::CMD3D11Rtv(int w, int h)
{
	HRESULT hr = S_OK;
	ID3D11Device* d3ddevice = d3d11.GetDevice();

	// ���A����������
	m_width = w;
	m_height = h;

	// �I�t�X�N���[���e�N�X�`�� �� �쐬
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = w;		// ��
	desc.Height = h;		// ����
//	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	/*DXGI_FORMAT_R8G8B8A8_TYPELESS;*/
	desc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	desc.MipLevels = 1;		// �~�b�v�}�b�v ���x����
	desc.ArraySize = 1;		// �z��T�C�Y
	desc.SampleDesc.Count = 1;    // �}���`�T���v�����O�̐ݒ�
	desc.SampleDesc.Quality = 0;    // �}���`�T���v�����O�̕i��

	desc.Usage = D3D11_USAGE_DEFAULT;		// �g�p���@
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;    // CPU����A�N�Z�X�Ȃ�
	desc.MiscFlags = 0;    // ���̑��̐ݒ�Ȃ�
	hr = d3ddevice->CreateTexture2D(
		&desc,				// �쐬����2D�e�N�X�`��
		nullptr, //&subres,		// �ӂ���nullptr
		&m_pOffScreen			// �쐬�����e�N�X�`�����󂯎��
	);
	if (FAILED(hr))
		return;

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	// �����_�[�^�[�Q�b�g�r���[�̐���
	hr = d3ddevice->CreateRenderTargetView(m_pOffScreen, &rtvDesc, &m_pRenderTargetView);
	if (FAILED(hr))
	{
		//("Error : ID3D11Device::CreateRenderTargetView() Failed.");
		return;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_RESOURCE_DIMENSION type;
	m_pOffScreen->GetType(&type);
	ID3D11Texture2D *pTexture2D = nullptr;

	switch (type) {
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0, sizeof(desc));
		pTexture2D = (ID3D11Texture2D*)m_pOffScreen;
		pTexture2D->GetDesc(&desc);
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		// �V�F�[�_�[���\�[�X�r���[�@�쐬
		d3ddevice->CreateShaderResourceView(m_pOffScreen, &srvDesc, &m_pShaderResView);
		break;

	default:
		//...
		// Error
		break;
	}

	// Unordered Access View
	pTexture2D->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC    uavDesc = { };
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	//	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//uavDesc.Buffer.FirstElement = 0;
	uavDesc.Format = DXGI_FORMAT_R32_UINT;	// DXGI_FORMAT_UNKNOWN;
//	uavDesc.Texture2D.MipSlice = 0;

	d3ddevice->CreateUnorderedAccessView(m_pOffScreen, &uavDesc, &m_pUAV);
}

// Destructor
CMD3D11Rtv::~CMD3D11Rtv()
{
	SAFE_RELEASE(m_pUAV);
	SAFE_RELEASE(m_pShaderResView);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pOffScreen);
}
