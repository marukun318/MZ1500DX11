// AppMain for Windows

#include	"AppMain.h"

extern CMD3D11		d3d11;

#define TEX_WIDTH	screen_width
#define TEX_HEIGHT	screen_height


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
	// ����
	uint32_t* ptr = offscr;
	uint32_t data = 0;
	for (int i = 0; i < TEX_HEIGHT; i++) {
		for (int j = 0; j < TEX_WIDTH; j++) {
			data = 0xFF000000 | ((rand() & 0xFF) << 16) | ((rand() & 0xFF) << 9) | (rand() & 0xFF);
//			*(ptr++) = 0xFFFF0000;		//0xAABBGGRR
			*(ptr++) = data;
		}
	}
	offtex->Update(offscr);
}

// �`��
void AppMain::Draw()
{
	d3d11.Render(offtex->m_pShaderResView);			// �I�t�X�N���[����`��
}
