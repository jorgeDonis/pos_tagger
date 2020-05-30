#pragma once

#include "mapa_memoria.h"

#include <vector>
#include <unordered_map>
#include <string>

class ParejaString
{
    public:
        std::string izquierda;
        std::string derecha;
        ParejaString(std::string a, std::string b) {
            izquierda = a;
            derecha = b;
        }
        bool operator==(const ParejaString& foo) const {
            return (izquierda == foo.izquierda && derecha == foo.derecha);
        }
        bool operator<(const ParejaString& foo) const {
            return izquierda < foo.izquierda;
        }
};

class Hasher
{
    private:
        static const uint32_t Prime = 0x01000193;
        static const uint32_t Seed = 0x811C9DC5;
        inline uint32_t fnv1a(const char *text, uint32_t hash = Seed) const
        {
            const unsigned char *ptr = (const unsigned char *)text;
            while (*ptr)
                hash = (*ptr++ ^ hash) * Prime;
            return hash;
        }
    public:
        std::size_t operator() (const ParejaString& clave) const
        {
            std::string combi = clave.izquierda + clave.derecha;
            return fnv1a(combi.c_str());
        }
        std::size_t operator()(const std::string &clave) const
        {
            return fnv1a(clave.c_str());
        }
};

class Tagger
{
    private:
        std::unordered_map<ParejaString, float, Hasher> A;
        std::unordered_map<ParejaString, float, Hasher> B;
        std::unordered_map<ParejaString, size_t, Hasher> transition_f;
        std::unordered_map<ParejaString, size_t, Hasher> observed_f;
        std::unordered_map<std::string, size_t> tag_prev_f;
        std::unordered_map<std::string, size_t> tag_total_f;
        void calcular_matrices();
        void calcular_frecuencias(const std::string&, const unsigned int&);
        void registrar_token(const unsigned&, const unsigned&, const unsigned&, const MapaMemoria&, std::string&);
    public:
        void entrenar(const std::string&, const unsigned int& = 100);
};