#ifndef __USERPROG__
#define __USERPROG__


typedef struct _mole_struct{
    mbox_t S2_mbox;
    mbox_t S_mbox;
    mbox_t CO_mbox;
    mbox_t O2_mbox;
    mbox_t C2_mbox;
    mbox_t SO4_mbox;
}mole_struct;

#define FILE_CO "injectCO.dlx.obj"
#define FILE_S2  "injectS2.dlx.obj"
#define FILE_REACT1 "react1.dlx.obj"
#define FILE_REACT2 "react2.dlx.obj"
#define FILE_REACT3 "react3.dlx.obj"

#endif
