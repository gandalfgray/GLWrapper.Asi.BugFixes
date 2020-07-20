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

struct H3DlgMsg;
struct H3Dlg;
struct H3DlgItem;

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

struct H3Dlg
{
 _ptr_*  v_table;
 DWORD  z_order;
 H3Dlg*  next_dlg;
 H3Dlg*  last_dlg;
 DWORD  flags;
 DWORD  state;
 DWORD  x;
 DWORD  y;
 DWORD  width;
 DWORD  height;
 H3DlgItem* first_item;
 H3DlgItem* last_item;
 _ptr_  items_list;
 _ptr_  items_first;
 _ptr_  items_last;
 _ptr_  items_mem_end;
 DWORD  focused_item_id;
 void* screenshot_pcx16;
 _ptr_ field_48;
 _ptr_ field_4C;
 _ptr_ field_50;
 _ptr_ field_54;
 _ptr_ field_58;
 _ptr_ field_5C;
 _ptr_ field_60;
 DWORD field_64;
 DWORD field_68;
};

struct H3DlgItem
{
 _ptr_*  v_table;
 H3Dlg*  parent;
 H3DlgItem* prev_item;
 H3DlgItem* next_item;
 WORD  id;
 WORD  z_order;
 WORD  flags;
 WORD  state;
 WORD  x;
 WORD  y;
 WORD  width;
 WORD  height;
 PCHAR  short_tip_text; // shown in status bar
 PCHAR  full_tip_text; // shown in RMC Message Box
 BYTE  field_28[4];
 DWORD  field_2C; 
};
