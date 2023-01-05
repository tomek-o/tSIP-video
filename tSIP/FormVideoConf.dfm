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
  PixelsPerInch = 96
  TextHeight = 13
  object lblDisplayParentType: TLabel
    Left = 3
    Top = 26
    Width = 122
    Height = 13
    Caption = 'Video display parent type'
  end
  object lblDisplayParentId: TLabel
    Left = 3
    Top = 50
    Width = 108
    Height = 13
    Caption = 'Video display parent id'
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
    Left = 147
    Top = 23
    Width = 230
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
  end
  object edDisplayParentId: TEdit
    Left = 147
    Top = 47
    Width = 62
    Height = 21
    TabOrder = 2
  end
end
