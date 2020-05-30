#include "lattice.h"

Lattice::Lattice(const unsigned & rows, const unsigned & cols)
{
    this->rows = rows;
    this->cols = cols;
    container = new NodoViterbi[rows * cols];
}

Lattice::~Lattice()
{
    delete[] container;
}

NodoViterbi& Lattice::getNodo(const unsigned & row, const unsigned & col)
{
    return container[row * cols + col];
}
