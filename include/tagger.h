#pragma once

#include "mapa_memoria.h"
#include "lattice.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <list>

class ParejaString
{
    public:
        std::string izquierda;
        std::string derecha;
        ParejaString(std::string a, std::string b) {
            izquierda = a;
            derecha = b;
        }
        ParejaString(const ParejaString& foo) {
            izquierda = foo.izquierda;
            derecha = foo.derecha;
        }
        bool operator==(const ParejaString& foo) const {
            return (izquierda == foo.izquierda && derecha == foo.derecha);
        }
        bool operator<(const ParejaString& foo) const {
            return izquierda < foo.izquierda;
        }
        bool operator>(const ParejaString &foo) const
        {
            return izquierda < foo.izquierda;
        }
};

class TrioString
{
    private:
        void copy_vals(const TrioString& foo)
        {
            primero = foo.primero;
            segundo = foo.segundo;
            tercero = foo.tercero;
        }
    public:
        std::string primero, segundo, tercero;
        TrioString(const std::string & a, const std::string & b , const std::string &c)
        {
            primero = a;
            segundo = b;
            tercero = c;
        }
        TrioString(const TrioString& foo)
        {
            copy_vals(foo);
        }
        TrioString& operator=(const TrioString& foo)
        {
            if (this != &foo)
            {
                this->~TrioString();
                copy_vals(foo);
            }
            return *this;
        }
        bool operator==(const TrioString& foo) const
        {
            return (primero == foo.primero && segundo == foo.segundo && tercero == foo.tercero);
        }
        bool operator<(const TrioString& foo) const
        {
            return (primero < foo.primero);
        }
};

class Hasher
{
    private:
    public:
        inline static uint32_t fnv1a(const char *text, uint32_t hash = Seed)
        {
            const unsigned char *ptr = (const unsigned char *)text;
            while (*ptr)
                hash = (*ptr++ ^ hash) * Prime;
            return hash;
        }
        static const uint32_t Prime = 0x01000193;
        static const uint32_t Seed = 0x811C9DC5;
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

namespace std
{
    template <>
    struct hash<ParejaString>
    {
        size_t operator()(const ParejaString &clave) const
        {
            std::string combi = clave.izquierda + clave.derecha;
            return Hasher::fnv1a(combi.c_str());
        }
    };
    template <>
    struct hash<TrioString>
    {
        size_t operator()(const TrioString &clave) const
        {
            std::string combi = clave.primero + clave.segundo + clave.tercero;
            return Hasher::fnv1a(combi.c_str());
        }
    };
}
class Tagger
{
    private:
        static const unsigned L = 5;
        double alpha[L];
        std::unordered_map<std::string, std::unordered_set<std::string, Hasher>, Hasher> tag_tipos_palabra;
        std::unordered_map<ParejaString, double> A;
        std::unordered_map<ParejaString, double> B;
        std::unordered_map<ParejaString, size_t> transition_f;
        std::unordered_map<ParejaString, size_t> observed_f;
        std::unordered_map<std::string, size_t, Hasher> tag_prev_f;
        std::unordered_map<std::string, size_t, Hasher> tag_total_f;
        std::unordered_map<TrioString, double> c_w_end_t_res;
        size_t c_w_end_t(const std::string&, const std::string&, const size_t&);
        double p_e_wt_unknown(const std::string &, const std::string &);
        double p_e_wt(const std::string&, const std::string&);
        void lattice_trace(Lattice&, std::list<std::string>&);
        void lattice_ini(Lattice&, std::list<std::string>::const_iterator&);
        void lattice_fill(Lattice &, std::list<std::string>::const_iterator &);
        void calcular_matrices();
        void calcular_frecuencias(const std::string&, const unsigned int&);
        void registrar_token(const unsigned&, const unsigned&, const unsigned&, const MapaMemoria&, std::string&);
    public:
        Tagger() {
            for (unsigned i = 0; i < L; i++)
                alpha[i] = 0.00001;
        }
        void entrenar(const std::string&, const unsigned int& = 100);
        void etiquetar(const std::list<std::string> &, std::list<std::string>&);
};