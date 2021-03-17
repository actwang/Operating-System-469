#ifndef __USERPROG__
#define __USERPROG__


typedef struct _sem_struct{
    sem_t numH2O;
    sem_t numSO4;
    sem_t numReact1;
    sem_t numReact2;
    sem_t numReact3;
    sem_t numH2;
    sem_t numO2;
    sem_t numSO2;
    sem_t numH2SO4;
}sem_struct;

#define FILE_H2O "injectH2O.dlx.obj"
#define FILE_SO4 "injectSO4.dlx.obj"
#define FILE_REACT1 "react1.dlx.obj"
#define FILE_REACT2 "react2.dlx.obj"
#define FILE_REACT3 "react3.dlx.obj"

#endif