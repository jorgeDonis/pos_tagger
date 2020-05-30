#pragma once

#include <string>

class MapaMemoria
{
    private:
        void copy_vals(const MapaMemoria&);
    public:
        size_t file_size;
        char* mapa;
        MapaMemoria(const std::string&);
        MapaMemoria() {;}
        ~MapaMemoria();
        MapaMemoria(const MapaMemoria& foo) {copy_vals(foo);}
        MapaMemoria& operator=(const MapaMemoria&);
        void cargar_fichero(const std::string&);
};