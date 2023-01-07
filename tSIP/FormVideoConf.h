//---------------------------------------------------------------------------

#ifndef FormVideoConfH
#define FormVideoConfH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

struct VideoConf;
struct UaConf;

class TfrmVideoConf : public TForm
{
__published:	// IDE-managed Components
	TCheckBox *chbEnabled;
	TLabel *lblDisplayParentType;
	TComboBox *cbDisplayParentType;
	TLabel *lblDisplayParentId;
	TEdit *edDisplayParentId;
	TCheckBox *chbSelfviewEnable;
	TCheckBox *chbSelfviewPip;
private:	// User declarations
	VideoConf *cfg;
	UaConf *uaCfg;
public:		// User declarations
	__fastcall TfrmVideoConf(TComponent* Owner);
	void SetCfg(VideoConf *cfg, UaConf *uaCfg);
	void Apply(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmVideoConf *frmVideoConf;
//---------------------------------------------------------------------------
#endif
