//
//	filecheck.h
//
//	Definitions for filecheck operation.  These include
//

#ifndef	__filecheck_h__
#define	__filecheck_h__

#include <stdio.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <dirent.h>

int FileExistCheck(char* file_path);
void FilePermCheck(char* file_path, char* perm);
char* FileGetPerm(char* file_path);
void FileInfoCheck(char* file_path);
int FileHardLinkCheck(char* file_path);
void FileCyclicCheck(char* file_path);
void FileGetDevice(char* file_path);

void FileSearchDir(char* file_path, int** inode_arr, int* i, int* ct) ;
int binarySearch(int arr[], int l, int r, int x);

int isOption(char* string);
int isPerm(char* string);

#endif	/* __process_h__ */
