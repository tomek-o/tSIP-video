//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormVideoConf.h"
#include "VideoConf.h"
#include "ua/UaConf.h"
#include "ua/VideoModules.h"
#include "VideoDevicesList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmVideoConf *frmVideoConf;
//---------------------------------------------------------------------------
__fastcall TfrmVideoConf::TfrmVideoConf(TComponent* Owner)
	: TForm(Owner),
	cfg(NULL)
{
    VideoModules::FillInputSelectorCb(cbInputMod);

	cbDisplayParentType->Clear();
	for (unsigned int i=0; i<VideoConf::DISPLAY_PARENT__LIMITER; i++)
	{
		cbDisplayParentType->Items->Add(VideoConf::getDisplayParentTypeName(static_cast<VideoConf::DisplayParentType>(i)));
	}
}
//---------------------------------------------------------------------------

void TfrmVideoConf::SetCfg(VideoConf *cfg, UaConf *uaCfg)
{
    VideoDevicesList::Instance().Refresh();

	this->cfg = cfg;
	this->uaCfg = uaCfg;

	chbEnabled->Checked = cfg->enabled;

	cbInputMod->ItemIndex = VideoModules::GetInputModuleCbIndex(uaCfg->video.videoSource.mod.c_str());
	cbInputModChange(NULL);

	cbDisplayParentType->ItemIndex = cfg->displayParentType;
	edDisplayParentId->Text = cfg->displayParentId;

	chbSelfviewEnable->Checked = uaCfg->video.selfview.enabled;
	chbSelfviewPip->Checked = uaCfg->video.selfview.pip;
}

void TfrmVideoConf::Apply(void)
{
	cfg->enabled = chbEnabled->Checked;

	uaCfg->video.videoSource.mod = VideoModules::GetInputModuleFromCbIndex(cbInputMod->ItemIndex);
	if (cbInputDev->Tag == 0 || cbInputDev->ItemIndex != cbInputDev->Items->Count - 1)
	{
		uaCfg->video.videoSource.dev = cbInputDev->Text.c_str();
	}

	cfg->displayParentType = static_cast<VideoConf::DisplayParentType>(cbDisplayParentType->ItemIndex);
	cfg->displayParentId = StrToIntDef(edDisplayParentId->Text, cfg->displayParentId);

	uaCfg->video.selfview.enabled = chbSelfviewEnable->Checked;
	uaCfg->video.selfview.pip = chbSelfviewPip->Checked;
}

void __fastcall TfrmVideoConf::cbInputModChange(TObject *Sender)
{
	AnsiString mod = VideoModules::GetInputModuleFromCbIndex(cbInputMod->ItemIndex);
	if (mod == VideoModules::dshow)
	{
		btnSelectInputFile->Visible = false;
		edInputFile->Visible = false;
		cbInputDev->Visible = true;
		lblInputDevice->Visible = true;
		VideoDevicesList::FillComboBox(cbInputDev, mod, false, uaCfg->video.videoSource.dev.c_str());
	}
	else
	{
		assert(!"Unhandled cbSoundInputMod item index!");
		btnSelectInputFile->Visible = false;
		edInputFile->Visible = false;
		cbInputDev->Visible = false;
		lblInputDevice->Visible = false;
	}
}
//---------------------------------------------------------------------------
