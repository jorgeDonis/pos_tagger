#include "mapa_memoria.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>

using namespace std;

void MapaMemoria::copy_vals(const MapaMemoria& foo)
{
    this->mapa = foo.mapa;
}

MapaMemoria::MapaMemoria(const string& ruta_fichero)
{
    cargar_fichero(ruta_fichero);
}

MapaMemoria::~MapaMemoria()
{
    munmap((char *) this->mapa, file_size);
}

MapaMemoria& MapaMemoria::operator=(const MapaMemoria& foo)
{
    if (&foo != this)
    {
        this->~MapaMemoria();
        copy_vals(foo);
    }
    return *this;
}

void MapaMemoria::cargar_fichero(const string& ruta_fichero)
{
    int ifd = open(ruta_fichero.c_str(), O_RDONLY, (mode_t)0600);
    if (ifd == -1)
        cerr << "ERROR: error al leer el fichero" << ruta_fichero << endl;
    struct stat fileInfo = {0};
    fstat(ifd, &fileInfo);
    this->file_size = fileInfo.st_size;
    this->mapa = (char *)mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, ifd, 0);
}