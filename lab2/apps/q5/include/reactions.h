#ifndef __USERPROG__
#define __USERPROG__


typedef struct _sem_struct{
    int numH2O;
    int numSO4;
    int numReact1;
    int numReact2;
    int numReact3;
    int numH2;
    int numO2;
    int numSO2;
    int numH2SO4;
}sem_struct;

#define FILE_H2O "injectH2O.dlx.obj"
#define FILE_SO4 "injectSO4.dlx.obj"
#define FILE_REACT1 "react1.dlx.obj"
#define FILE_REACT2 "react2.dlx.obj"
#define FILE_REACT3 "react3.dlx.obj"

#endif
