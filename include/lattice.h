#pragma once

#include <string>

class NodoViterbi
{
    public:
        double p;
        NodoViterbi* puntero_prev;
        std::string tag;
        NodoViterbi()
        {
            p = 0;
            puntero_prev = nullptr;
        }
        NodoViterbi(const NodoViterbi &foo)
        {
            p = foo.p;
            puntero_prev = foo.puntero_prev;
            tag = foo.tag;
        }
};

class Lattice
{
    private:
        NodoViterbi* container;
    public:
        unsigned rows;
        unsigned cols;
        Lattice() {
            rows = cols = 0,
            container = nullptr;
        }
        Lattice(const Lattice& foo) {
            rows = foo.rows;
            cols = foo.rows;
            container = foo.container;
        }
        Lattice(const unsigned&, const unsigned&);
        ~Lattice();
        NodoViterbi &getNodo(const unsigned &, const unsigned &);
};
