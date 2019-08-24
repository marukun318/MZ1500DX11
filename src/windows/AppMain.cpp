// AppMain for Windows

#include	"AppMain.h"

extern CMD3D11		d3d11;

#define TEX_WIDTH	640
#define TEX_HEIGHT	480


// �R���X�g���N�^
AppMain::AppMain()
{
}


// �f�X�g���N�^
AppMain::~AppMain()
{
	free(offscr);							// �I�t�X�N���[�����
	delete offtex;
}

// ������
void AppMain::SetUp()
{
	offtex = new CMD3D11Tex(TEX_WIDTH, TEX_HEIGHT);	// �I�t�X�N���[���e�N�X�`���쐬
	// �I�t�X�N���[���m��
	offscr = (uint32_t *) malloc(sizeof(uint32_t) * TEX_WIDTH * TEX_HEIGHT);
	// ���x�^�e�N�X�`���쐬
	memset(offscr, 0xFF, sizeof(uint32_t) * TEX_WIDTH * TEX_HEIGHT);
}

// �X�V
void AppMain::Update()
{
}

// �`��
void AppMain::Draw()
{
//	offtex->Map2(offscr);
	offtex->Update(offscr);

	d3d11.Render(offtex->m_pShaderResView);			// �I�t�X�N���[����`��
}
