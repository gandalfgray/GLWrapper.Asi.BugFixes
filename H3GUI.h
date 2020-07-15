// _EventMsg_::type values
#define MT_MOUSEOVER 4
#define MT_MOUSEBUTTON 0x200
#define MT_KEYDOWN  1
#define MT_KEYUP  2

// MT_MOUSEBUTTON _EventMsg_::subtype values
#define MST_LBUTTONDOWN 0xc
#define MST_RBUTTONDOWN 0xe
#define MST_LBUTTONCLICK 0xd

// _EventMsg_::flags 
#define MF_SHIFT 1
#define MF_CTRL 4
#define MF_ALT 32


// _Dlg_::flags 
#define DF_NOTSCREEN 0x01
#define DF_SCREENSHOT 0x02
#define DF_SHADOW  0x10
// my _Dlg_::flags 
#define DF_HD   0x04
#define DF_SIMPLEFRAME 0x08
#define DF_GRAYFRAME 0x20
#define DF_FSSHADOW  0x40
#define DF_XORSCREEN 0x80

// _DlgItem_::flags
#define DIF_TRANSPARENT 0x0001
#define DIF_PCX   0x0800
#define DIF_DEF   0x0010
#define DIF_BUTTON  0x0002
#define DIF_TEXT  0x0008 // read only

#define DIID_OK 30725

#define DLG_X_CENTER -1
#define DLG_Y_CENTER -1

struct H3DlgMsg
{
 DWORD type;
 INT32 subtype;
 INT32 item_id;
 DWORD flags;
 DWORD x_abs;
 DWORD y_abs;
 DWORD new_param ;
 DWORD flags_2;
};

typedef H3DlgMsg H3EventMsg;
