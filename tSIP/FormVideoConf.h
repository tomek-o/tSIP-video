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

class TfrmVideoConf : public TForm
{
__published:	// IDE-managed Components
	TCheckBox *chbEnabled;
	TLabel *lblDisplayParentType;
	TComboBox *cbDisplayParentType;
	TLabel *lblDisplayParentId;
	TEdit *edDisplayParentId;
private:	// User declarations
	VideoConf *cfg;
public:		// User declarations
	__fastcall TfrmVideoConf(TComponent* Owner);
	void SetCfg(VideoConf *cfg);
	void Apply(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmVideoConf *frmVideoConf;
//---------------------------------------------------------------------------
#endif
