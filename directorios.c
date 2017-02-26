// Librerias
#include "directorios.h"
#include <errno.h> //ELIMINAR

// Funciones

/* Da la ruta absoluta del directorio donde se está ejecutando el programa*/
char *obtenerDirectorioRaiz(){
  char *abspath = (char *) malloc(sizeof(char) * PATH_MAX);

  if (getcwd(abspath, PATH_MAX) != NULL) {

    printf("Directorio raíz donde se ejecutará el comando: %s\n", abspath);
  } else {
    perror("getcwd() error");
  }

  return abspath;
};

/* Cuenta cuantos archivos y sub-directorios posee un directorio y cuantos
    bytes ocupan dichos archivos (sin contar los directorios)*/
void informacionArchivos(Directorio *directorio) {
  // Variables
  struct dirent *fd;
  struct stat info;
  directorio->bytes = 0;
  directorio->cantArchivos = 0;


  // Cuenta solamente los archivos
  while ((fd = readdir(directorio->dir)) != NULL) {
    stat(fd->d_name, &info);
    // No cuenta los archivos ocultos
    if (fd->d_name[0] == '.') {
      continue;
    }
    if (S_ISDIR(info.st_mode)) {
      (directorio->cantArchivos)++;
    } else {
      (directorio->cantArchivos)++;
      directorio->bytes = directorio->bytes + info.st_size;
    }
  }
}

void informacionArchivosRecursivo(Directorio *directorio) {
  // Variables
  struct dirent *fd;
  struct stat info;
  directorio->bytes = 0;
  directorio->cantArchivos = 0;


  // Cuenta solamente los archivos
  while ((fd = readdir(directorio->dir)) != NULL) {
    // No cuenta los archivos ocultos
    if (fd->d_name[0] == '.') {
      continue;
    }

    if (stat(fd->d_name, &info) == -1) {
      char abs[PATH_MAX];
      getcwd(abs, PATH_MAX);
      strcat(abs, "/");
      strcat(abs, directorio->path);
      strcat(abs, "/");
      strcat(abs, fd->d_name);
      //printf("Path: %s\n", directorio->path);

      stat(abs, &info);

      /*if (stat(abs, &info) == -1) {
        printf("%s\n", abs);
        printf("MAMALO MUCHO\n");
      };*/
    };

    if (S_ISDIR(info.st_mode)) {
      Directorio *recursivo = (Directorio *) malloc(sizeof(Directorio));
      
      // Concateno el padre a la primera recursion
      strcpy(recursivo->path, "");
      strcat(recursivo->path, directorio->path);
      strcat(recursivo->path, "/");
      strcat(recursivo->path, fd->d_name);

      // Concatena el nombre
      char abs[PATH_MAX];
      getcwd(abs, PATH_MAX);
      strcat(abs, "/");
      strcat(abs, directorio->path);
      strcat(abs, "/");
      strcat(abs, fd->d_name);
      // Crea el DIR
      recursivo->dir = opendir(abs);

      informacionArchivosRecursivo(recursivo);
      (directorio->cantArchivos) += recursivo->cantArchivos + 1;
      (directorio->bytes) += recursivo->bytes;
      //printf("Sume dos veces\n");
      // Elimina el Directorio de recursión.
      free(recursivo);
      //printf("Elimino recursivo\n");
    } else {
      (directorio->cantArchivos)++;
      directorio->bytes = directorio->bytes + info.st_size;
    }
  }
}


/* Busca los permisos de un directorio en específico. */
void permisosDirectorio(struct stat informacion){
    printf( (S_ISDIR(informacion.st_mode)) ? "d" : "-");
    printf( (informacion.st_mode & S_IRUSR) ? "r" : "-");
    printf( (informacion.st_mode & S_IWUSR) ? "w" : "-");
    printf( (informacion.st_mode & S_IXUSR) ? "x" : "-");
    printf( (informacion.st_mode & S_IRGRP) ? "r" : "-");
    printf( (informacion.st_mode & S_IWGRP) ? "w" : "-");
    printf( (informacion.st_mode & S_IXGRP) ? "x" : "-");
    printf( (informacion.st_mode & S_IROTH) ? "r" : "-");
    printf( (informacion.st_mode & S_IWOTH) ? "w" : "-");
    printf( (informacion.st_mode & S_IXOTH) ? "x" : "-");
};


/* Obtiene toda la información del directorio desde donde se está ejecutando el programa. */
Directorio *datosDirectorioActual(char *path){
  // Inicializa el directorio
  Directorio *directorio = (Directorio *) malloc(sizeof(Directorio));
  // Inicializa las variables
  strcpy((directorio->path), path);
  directorio->dir = opendir(directorio->path);
  informacionArchivos(directorio);
  stat(".", &(directorio->informacion));
  directorio->grupo = *getgrgid(directorio->informacion.st_gid);
  directorio->usuario = *getpwuid(directorio->informacion.st_uid);
  strftime(directorio->fechaMod, sizeof(directorio->fechaMod), FORMATO_FECHA, localtime(&directorio->informacion.st_mtime));
  strftime(directorio->fechaAcc, sizeof(directorio->fechaAcc), FORMATO_FECHA, localtime(&directorio->informacion.st_atime));

  // Retorna
  return directorio;
}

/* Obtiene toda la información del directorio desde donde se está ejecutando el programa. */
Directorio *datosDirectorioRecursivo(char *path){
  // Inicializa el directorio
  Directorio *directorio = (Directorio *) malloc(sizeof(Directorio));
  // Inicializa las variables
  strcpy((directorio->path), path);
  directorio->dir = opendir(directorio->path);
  informacionArchivosRecursivo(directorio);
  stat(".", &(directorio->informacion));
  directorio->grupo = *getgrgid(directorio->informacion.st_gid);
  directorio->usuario = *getpwuid(directorio->informacion.st_uid);
  strftime(directorio->fechaMod, sizeof(directorio->fechaMod), FORMATO_FECHA, localtime(&directorio->informacion.st_mtime));
  strftime(directorio->fechaAcc, sizeof(directorio->fechaAcc), FORMATO_FECHA, localtime(&directorio->informacion.st_atime));

  // Retorno
  return directorio;
}

void imprimeDirectorio(char *abspath, Directorio directorio){
  // Imprime de la información del directorio
  if (strcmp(abspath, "") == 0) {
    printf("%s/ ", directorio.path);
  } else {
    printf("%s/%s/ ", abspath, directorio.path); // Dirección del directorio
  }
  printf("| ");
  permisosDirectorio(directorio.informacion); // Permisos del directorio
  printf(" ");
  printf("| %s ", (directorio.usuario).pw_name); // User-ID
  printf("| %s ", (directorio.grupo).gr_name); // Group-ID
  printf("| %s ", directorio.fechaMod); // Fecha de Modificación
  printf("| %s ", directorio.fechaAcc); // Fecha de Acceso
  printf("| %d ", directorio.cantArchivos); // Cantidad de archivos
  printf("| %d ", directorio.bytes);
  printf("\n");
}