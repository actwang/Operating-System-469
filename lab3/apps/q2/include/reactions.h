#ifndef __USERPROG__
#define __USERPROG__


typedef struct _mole_struct{
    int numCO;
    int numS2;
    int numReact1;
    int numReact2;
    int numReact3;
    int numS;
    int numO2;
    int numC2;
    int numSO4;
}mole_struct;

#define FILE_CO "injectCO.dlx.obj"
#define FILE_S2  "injectS2.dlx.obj"
#define FILE_REACT1 "react1.dlx.obj"
#define FILE_REACT2 "react2.dlx.obj"
#define FILE_REACT3 "react3.dlx.obj"

#endif