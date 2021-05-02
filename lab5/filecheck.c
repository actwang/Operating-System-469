//
//	filecheck.c
//
//
//

#include "filecheck.h"

void FilePermCheck(char* file_path, char* perm) {
  char* user_perm;
  int i;
  int missing = 0;
  int result = 1;

  user_perm = FileGetPerm(file_path);

  for (i = 0; i < strlen(perm); i++) {
    if (perm[i] == 'r') {
      if (user_perm[0] != perm[i]) {
        missing = 1;
        result = 0;
        break;
      }
    }
    if (perm[i] == 'w') {
      if (user_perm[1] != perm[i]) {
        missing = 2;
        result = 0;
        break;
      }
    }
    if (perm[i] == 'x') {
      if (user_perm[2] != perm[i]) {
        missing = 3;
        result = 0;
        break;
      }
    }
  }

  if (result != 1) {
    printf("Cannot %s access. Reason: The user does not have ", perm);
    switch (missing) {
      case 1 : printf("read access to directory \"%s\".\n", file_path); break;
      case 2 : printf("write access to directory \"%s\".\n", file_path); break;
      case 3 : printf("execute access to directory \"%s\".\n", file_path); break;
    }
  } else {
    printf("User has %s permission to directory \"%s\".\n", perm, file_path);
  }

  free(user_perm);
}

char* FileGetPerm(char* file_path) {
  struct stat statbuf;
  char* perm = malloc(9 * sizeof(char));

  stat(file_path, &statbuf);

  switch (statbuf.st_mode & 0x700) {
    case S_IRWXU: perm[0] = 'r';  perm[1] = 'w';  perm[2] = 'x';  break;
    case S_IRUSR: perm[0] = 'r';  perm[1] = '-';  perm[2] = '-';  break; //4
    case S_IWUSR: perm[0] = '-';  perm[1] = 'w';  perm[2] = '-';  break; //2
    case S_IXUSR: perm[0] = '-';  perm[1] = '-';  perm[2] = 'x';  break; //1
    case 0x00600: perm[0] = 'r';  perm[1] = 'w';  perm[2] = '-';  break; //6
    case 0x00500: perm[0] = 'r';  perm[1] = '-';  perm[2] = 'x';  break; //5
    case 0x00300: perm[0] = '-';  perm[1] = 'w';  perm[2] = 'x';  break; //3
    default:      perm[0] = '-';  perm[1] = '-';  perm[2] = '-';  break;
  }

  switch (statbuf.st_mode & 0x70) {
    case S_IRWXG: perm[3] = 'r';  perm[4] = 'w';  perm[5] = 'x';  break;
    case S_IRGRP: perm[3] = 'r';  perm[4] = '-';  perm[5] = '-';  break; //4
    case S_IWGRP: perm[3] = '-';  perm[4] = 'w';  perm[5] = '-';  break; //2
    case S_IXGRP: perm[3] = '-';  perm[4] = '-';  perm[5] = 'x';  break; //1
    case 0x00060: perm[3] = 'r';  perm[4] = 'w';  perm[5] = '-';  break; //6
    case 0x00050: perm[3] = 'r';  perm[4] = '-';  perm[5] = 'x';  break; //5
    case 0x00030: perm[3] = '-';  perm[4] = 'w';  perm[5] = 'x';  break; //3
    default:      perm[3] = '-';  perm[4] = '-';  perm[5] = '-';  break;
  }

  switch (statbuf.st_mode & 0x7) {
    case S_IRWXO: perm[6] = 'r';  perm[7] = 'w';  perm[8] = 'x';  break;
    case S_IROTH: perm[6] = 'r';  perm[7] = '-';  perm[8] = '-';  break; //4
    case S_IWOTH: perm[6] = '-';  perm[7] = 'w';  perm[8] = '-';  break; //2
    case S_IXOTH: perm[6] = '-';  perm[7] = '-';  perm[8] = 'x';  break; //1
    case 0x00006: perm[6] = 'r';  perm[7] = 'w';  perm[8] = '-';  break; //6
    case 0x00005: perm[6] = 'r';  perm[7] = '-';  perm[8] = 'x';  break; //5
    case 0x00003: perm[6] = '-';  perm[7] = 'w';  perm[8] = 'x';  break; //3
    default:      perm[6] = '-';  perm[7] = '-';  perm[8] = '-';  break;
  }

  return perm;
}

int FileExistCheck(char* file_path) {
  struct stat statbuf;
  // succeed if 0 returned
  if (stat(file_path, &statbuf) == -1) {
    return 0;
  } else {
    return 1;
  }
}

int FileHardLinkCheck(char* file_path) {
  struct stat statbuf;

  stat(file_path, &statbuf);

  if (statbuf.st_nlink != 0) {
    printf("Yes, hard links exist.\n");
    return 1;
  } else {
    printf("No hard links exists.\n");
    return 0;
  }
}

void FileInfoCheck(char* file_path) {
  struct stat statbuf;
  char parent[256];
  int len, i;

  char* perm;

  stat(file_path, &statbuf);

  printf("Info for path: %s\n", file_path);
  printf("{");
  switch (statbuf.st_mode & S_IFMT) {
    case S_IFCHR:  printf("Character device, ");        break;
    case S_IFDIR:  printf("Directory, ");               break;
    case S_IFIFO:  printf("FIFO/pipe, ");               break;
    case S_IFLNK:  printf("Symlink, ");                 break;
    case S_IFREG:  printf("Regular file, ");            break;
    case S_IFSOCK: printf("Socket, ");                  break;
    default:       printf("Unknown?, ");                break;
  }

  FileGetDevice(file_path);

  printf("%ju, ", (uintmax_t) (statbuf.st_ino));

  // permission
  perm = FileGetPerm(file_path);
  printf("%s, ", perm);
  free(perm);

  printf("%jd bytes, ", (intmax_t) (statbuf.st_size));

  // parent directory
  len = strlen(file_path);
  for (i = len-1; ; i--){
    if (file_path[i] == '/'){
      break;
    }
  }
  strncpy(parent, file_path, i);
  printf("%s ",parent);
  printf("}\n");
}

void FileGetDevice(char* file_path) {
  struct stat statbuf;
  FILE* f;
  char sline[256];
  char minmaj[128];

  stat(file_path, &statbuf);

  sprintf(minmaj, "%d:%d ", (int) statbuf.st_dev >> 8, (int) statbuf.st_dev & 0xff);

  f = fopen("/proc/self/mountinfo", "r");
  if (f == NULL) {
    fprintf(stderr, "Failed to open /proc/self/mountinfo\n");
    return;
  }

  while (fgets(sline, 256, f)) {
    char *token;
    char *where;

    token = strtok(sline, "-");
    where = strstr(token, minmaj);
    if (where) {
      token = strtok(NULL, " -:");
      token = strtok(NULL, " -:");
      printf("%s, ", token);
      break;
    }
  }
  fclose(f);
}

void FileSearchDir(char* file_path, int** inode_arr, int* i, int* ct) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(file_path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_DIR) {

        char new_path[1024];
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)continue;
        snprintf(new_path, sizeof(new_path), "%s/%s", file_path, entry->d_name);
        if (0 == binarySearch(*inode_arr, 0, *i, entry->d_ino)) {   // if it's not stored in the array
          (*inode_arr)[(*i)++] = entry->d_ino;
        }else{    // if it is already in the array
          (*ct) ++;
          printf("%s is a link to %s\n", file_path, entry->d_name);
        }
        FileSearchDir(new_path, inode_arr, i, ct);
      }
    }
    printf("i = %d\n",*i);
    closedir(dir);
}

void FileCyclicCheck(char* file_path) {
  int i;
  int search_i = 0;
  int counter = 0;

  int* inode_arr = malloc(2048 * sizeof(int));
  //char** dname_arr = malloc(256 * sizeof(char*));
  FileSearchDir(file_path, &inode_arr, &search_i, &counter);

//   for (i = 0; i < search_i; i++) {
//     printf("this dir = %d\n", inode_arr[i]);
//   }
  printf("%d circular links found.\n", counter);
  free(inode_arr);
  //free(dname_arr);
}

//Returns 1 if found, 0 if not found.
int binarySearch(int arr[], int l, int r, int x)
{
  if (l == 0 && r == 0)return 0;  // i = 0 exit to prevent uninitialized access at mid
  if (r >= l) {
    int mid = l + (r - l) / 2;

    if (arr[mid] == x)
        return 1;

    if (arr[mid] > x)
        return binarySearch(arr, l, mid - 1, x);

    return binarySearch(arr, mid + 1, r, x);
  }
  return 0;
}

int isOption(char* string){
  if (!((strcmp(string, "-p") && strcmp(string,"--permcheck") && strcmp(string, "-i") &&
        strcmp(string, "--info") && strcmp(string, "-l") && strcmp(string, "--linkcheck")
        && strcmp(string, "-d") && strcmp(string, "--dispcycles")))){
    return 1;
  }
  return 0;
}

int isPerm(char* string){
  if (!(strcmp(string, "w")&&strcmp(string,"r")&&strcmp(string,"x")&&
      strcmp(string,"rw")&&strcmp(string,"rx")&&strcmp(string,"rwx")&&
      strcmp(string,"wx"))){
    return 1;
  }
  return 0;
}

int main(int argc, char** argv){
  int i;
  int p_flag = 0;
  int i_flag = 0;
  int l_flag = 0;
  int d_flag = 0;

  if (argc <= 1 || argc > 7){
    printf("Usage: ./filecheck [one or more options] <path>");
    return EXIT_FAILURE;
  }

  if (argc == 2){ //either lack options or path
    if (!isOption(argv[1])){
      printf("No options provided\n");
      return EXIT_FAILURE;
    }
    else{
      printf("No filepath provided.\n");
      return EXIT_FAILURE;
    }
  }
  // ./filecheck -p w nonsense    (last arg is invalid)
  if (!FileExistCheck(argv[argc-1])){
    printf("Invalid file path provided.\n");
    return EXIT_FAILURE;
  }
  // if we reached here, it means argv[argc-1] is valid file path
  for (i = 1; i < argc-1; i++){   // loop excluding filepath
    if (!isOption(argv[i]) && !isPerm(argv[i])){  // if it's not an option return False
      printf("Invalid option.\n");
      return EXIT_FAILURE;
    }

    // -p --permcheck
    if (p_flag == 0 && (!(strcmp(argv[i], "-p") && strcmp(argv[i],"--permcheck")))){
      if (!isPerm(argv[i+1])){
        printf("No permissions provided for -p option.\n");
        return EXIT_FAILURE;
      }
      // valid -p permission request(rwx)
      FilePermCheck(argv[argc-1], argv[i+1]);
    }
    else{
      if (p_flag == 1) {printf("Duplicate -p flag.\n"); return EXIT_FAILURE;}
    }

    // -i --info
    if (i_flag == 0 && (!(strcmp(argv[i], "-i") && strcmp(argv[i], "--info")))){
      FileInfoCheck(argv[argc-1]);
    }else{
      if (i_flag == 1){printf("Duplicate -i flag.\n"); return EXIT_FAILURE;}
    }

    // -l --linkcheck
    if (l_flag == 0 && (!(strcmp(argv[i], "-l") && strcmp(argv[i], "--linkcheck")))){
      FileHardLinkCheck(argv[argc-1]);
    }else{
      if (l_flag == 1){printf("Duplicate -l flag.\n"); return EXIT_FAILURE;}
    }

    // -d --dispcycles
    if (d_flag == 0 && (!(strcmp(argv[i], "-d") && strcmp(argv[i], "--dispcycles")))){
      FileCyclicCheck(argv[argc-1]);
    }else{
      if (d_flag == 1){printf("Duplicate -d flag.\n"); return EXIT_FAILURE;}
    }
  }

  return EXIT_SUCCESS;
}
