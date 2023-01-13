object frmVideoConf: TfrmVideoConf
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  Caption = 'frmVideoConf'
  ClientHeight = 240
  ClientWidth = 463
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  ExplicitWidth = 320
  PixelsPerInch = 96
  TextHeight = 13
  object lblDisplayParentType: TLabel
    Left = 8
    Top = 90
    Width = 122
    Height = 13
    Caption = 'Video display parent type'
  end
  object lblDisplayParentId: TLabel
    Left = 8
    Top = 114
    Width = 108
    Height = 13
    Caption = 'Video display parent id'
  end
  object lblInputDevice: TLabel
    Left = 7
    Top = 55
    Width = 95
    Height = 13
    Caption = 'Video source device'
  end
  object lblInputModule: TLabel
    Left = 7
    Top = 31
    Width = 98
    Height = 13
    Caption = 'Video source module'
  end
  object lblAudioInput: TLabel
    Left = 3
    Top = 4
    Width = 26
    Height = 13
    Caption = 'Input'
  end
  object chbEnabled: TCheckBox
    Left = 3
    Top = 3
    Width = 325
    Height = 17
    Caption = 'enabled'
    TabOrder = 0
  end
  object cbDisplayParentType: TComboBox
    Left = 152
    Top = 87
    Width = 240
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 1
  end
  object edDisplayParentId: TEdit
    Left = 152
    Top = 111
    Width = 62
    Height = 21
    TabOrder = 2
  end
  object chbSelfviewEnable: TCheckBox
    Left = 8
    Top = 145
    Width = 325
    Height = 17
    Caption = 'enable selfview (show video from local source)'
    TabOrder = 3
  end
  object chbSelfviewPip: TCheckBox
    Left = 19
    Top = 168
    Width = 325
    Height = 17
    Caption = 'show selfview as picture-in-picture'
    TabOrder = 4
  end
  object cbInputDev: TComboBox
    Left = 152
    Top = 52
    Width = 240
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 5
  end
  object cbInputMod: TComboBox
    Left = 152
    Top = 28
    Width = 240
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 6
    OnChange = cbInputModChange
  end
  object edInputFile: TEdit
    Left = 152
    Top = 52
    Width = 240
    Height = 21
    TabOrder = 7
    Visible = False
  end
  object btnSelectInputFile: TButton
    Left = 398
    Top = 54
    Width = 21
    Height = 19
    Caption = '...'
    TabOrder = 8
    Visible = False
  end
end
