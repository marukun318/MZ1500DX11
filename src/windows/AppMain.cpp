// AppMain for Windows

#include	"AppMain.h"

extern CMD3D11		d3d11;

// �R���X�g���N�^
AppMain::AppMain()
{
}


// �f�X�g���N�^
AppMain::~AppMain()
{
}

// ������
void AppMain::SetUp()
{

}

// �X�V
void AppMain::Update()
{
}

// �`��
void AppMain::Draw()
{
	d3d11.Render();
}
