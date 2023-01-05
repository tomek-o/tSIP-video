//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormVideoConf.h"
#include "VideoConf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmVideoConf *frmVideoConf;
//---------------------------------------------------------------------------
__fastcall TfrmVideoConf::TfrmVideoConf(TComponent* Owner)
	: TForm(Owner),
	cfg(NULL)
{
	cbDisplayParentType->Clear();
	for (unsigned int i=0; i<VideoConf::DISPLAY_PARENT__LIMITER; i++)
	{
		cbDisplayParentType->Items->Add(VideoConf::getDisplayParentTypeName(static_cast<VideoConf::DisplayParentType>(i)));
	}
}
//---------------------------------------------------------------------------

void TfrmVideoConf::SetCfg(VideoConf *cfg)
{
	this->cfg = cfg;

	chbEnabled->Checked = cfg->enabled;
	cbDisplayParentType->ItemIndex = cfg->displayParentType;
	edDisplayParentId->Text = cfg->displayParentId;
}

void TfrmVideoConf::Apply(void)
{
	cfg->enabled = chbEnabled->Checked;
	cfg->displayParentType = static_cast<VideoConf::DisplayParentType>(cbDisplayParentType->ItemIndex);
	cfg->displayParentId = StrToIntDef(edDisplayParentId->Text, cfg->displayParentId);
}