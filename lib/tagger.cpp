#include "tagger.h"

#include <iostream>
#include <algorithm>

using namespace std;

/**
 * @brief Entrena el tagger. Rellena las matrices A y B.
 * 
 * @param ruta_fich Ruta al fichero de entrenamiento. Ha de estar en codificación
 * ISO-8859-1 (Western). Cada línea contiene una frase. La frase sigue el formato:
 * palabra + record separator + categoría. Record separator equivale a 30 en Western.
 * Las frases están separadas por un espacio + salto de línea.
 * @param porc_entrenamiento Porcentaje del fichero que se leerá.
 */
void Tagger::entrenar(const string& ruta_fich, const unsigned& porc_entrenamiento)
{
    calcular_frecuencias(ruta_fich, porc_entrenamiento);
    calcular_matrices();
}

void Tagger::calcular_matrices()
{
    for (const auto& pareja_tags : transition_f)
        A[pareja_tags.first] = (double) pareja_tags.second / tag_prev_f[pareja_tags.first.izquierda];
    for (const auto& pareja_etiquetada : observed_f)
        B[pareja_etiquetada.first] = (double) pareja_etiquetada.second / tag_total_f[pareja_etiquetada.first.derecha];

    // for (const auto& tag : tag_total_f)
    // {
    //     for (const auto& pareja : A)
    //     {
    //         if (pareja.first.izquierda == tag.first)
    //             cout << pareja.first.izquierda << " => " << pareja.first.derecha << " = " << pareja.second <<  endl;
    //     }
    // }

    // for (const auto &pareja : A)
    // {
    //     if (pareja.first.izquierda == "inicio")
    //         cout << pareja.first.izquierda << " => " << pareja.first.derecha << " = " << pareja.second << endl;
    // }
}

/**
 * @brief Abre el fichero ruta_fich como mapa dinámico y lee caracter a caracter,
 * extrayendo parejas de tokens y tags y actualizando las frecuencias.
 * 
 * @param ruta_fich 
 * @param porc_entrenamiento 
 */
void Tagger::calcular_frecuencias(const string& ruta_fich, const unsigned& porc_entrenamiento)
{
    MapaMemoria mapa_memoria(ruta_fich);
    unsigned bytes_entrenamiento = mapa_memoria.file_size * porc_entrenamiento / 100;
    unsigned it_fichero_entrada = 0;
    string tag_prev = "inicio";
    tag_prev_f["inicio"] = 0;
    while (it_fichero_entrada < mapa_memoria.file_size && it_fichero_entrada < bytes_entrenamiento)
    {
        tag_prev = "inicio";
        while (mapa_memoria.mapa[it_fichero_entrada] != '\n')
        {
            unsigned primer_izquierda = it_fichero_entrada;
            while ((unsigned char) mapa_memoria.mapa[it_fichero_entrada] != 30)
                it_fichero_entrada++;
            unsigned primer_derecha = it_fichero_entrada;
            while (mapa_memoria.mapa[it_fichero_entrada] != ' ')
                it_fichero_entrada++;
            registrar_token(primer_izquierda, primer_derecha, it_fichero_entrada, mapa_memoria, tag_prev);
            it_fichero_entrada++;
        }
        it_fichero_entrada++;
    }
}

/**
 * @brief Actualiza las frecuencias con la pareja token/tag
 * 
 * @param izquierda principio del token
 * @param medio caracter 'record separator' (30) que separa token/tag
 * @param derecha final del tag
 * @param mapa mapa dinámico
 * @param tag_prev necesario para el modelo oculto de markov
 */
void Tagger::registrar_token(const unsigned & izquierda, const unsigned & medio,
                             const unsigned & derecha, const MapaMemoria & mapa, string & tag_prev)
{
    string token(&mapa.mapa[izquierda], &mapa.mapa[medio]);
    string tag(&mapa.mapa[medio + 1], &mapa.mapa[derecha]);
    ParejaString pareja_tags(tag_prev, tag);
    ParejaString pareja_token_etiquetado(token, tag);
    transition_f[pareja_tags]++;
    tag_prev_f[tag_prev]++;
    observed_f[pareja_token_etiquetado]++;
    tag_total_f[tag]++;
    tag_tipos_palabra[tag].insert(token);
    tag_prev = tag;
}

void Tagger::lattice_ini(Lattice& lattice, list<string>::const_iterator& it_oracion)
{
    unsigned i = 0;
    for (const auto &tag : tag_total_f)
    {
        double f1 = A[ParejaString("inicio", tag.first)];
        double f2 = p_e_wt(*it_oracion, tag.first);
        lattice.getNodo(i, 0).p = f1 * f2;
        lattice.getNodo(i, 0).tag = tag.first;
        i++;
    }
    it_oracion++;
}

void Tagger::lattice_fill(Lattice &lattice, list<string>::const_iterator &it_oracion)
{
    for (unsigned j = 1; j < lattice.cols; j++)
    {
        unsigned i = 0;
        for (const auto &tag : tag_total_f)
        {
            NodoViterbi *prev_max = &lattice.getNodo(i, j - 1);
            double max_p = 0;
            unsigned k = 0;
            for (const auto &prev_tag : tag_total_f)
            {
                double f1 = lattice.getNodo(k, j - 1).p;
                double f2 = A[ParejaString(prev_tag.first, tag.first)];
                double prev_p = f1 * f2;
                if (prev_p > max_p)
                {
                    max_p = prev_p;
                    prev_max = &lattice.getNodo(k, j - 1);
                }
                k++;
            }
            double f = p_e_wt(*it_oracion, tag.first);
            lattice.getNodo(i, j).p = max_p * f;
            lattice.getNodo(i, j).puntero_prev = prev_max;
            lattice.getNodo(i, j).tag = tag.first;
            i++;
        }
        it_oracion++;
    }
}

void Tagger::lattice_trace(Lattice& lattice, list<string>& predicted_tags)
{
    NodoViterbi *nodo_ptr = &lattice.getNodo(0, lattice.cols - 1);
    double max_p = 0;
    for (unsigned i = 0; i < lattice.rows; i++)
    {
        double p = lattice.getNodo(i, lattice.cols - 1).p;
        if (p > max_p)
        {
            max_p = p;
            nodo_ptr = &lattice.getNodo(i, lattice.cols - 1);
        }
    }
    while (nodo_ptr != nullptr)
    {
        predicted_tags.push_front(nodo_ptr->tag);
        nodo_ptr = nodo_ptr->puntero_prev;
    }
}

size_t Tagger::c_w_end_t(const string& token, const string& tag, const size_t& l)
{
    TrioString clave = TrioString(token, tag, to_string(l));
    unordered_map<TrioString, double>::iterator it = c_w_end_t_res.find(clave);
    if (it == c_w_end_t_res.end())
    {
        size_t C = 0;
        unsigned tam_efectivo_comp = min(l, token.length());
        for (const string &palabra : tag_tipos_palabra[tag])
        {
            unsigned tam_efectivo = min(l, palabra.length());
            if (!token.compare(token.length() - tam_efectivo_comp, tam_efectivo_comp,
                            palabra, palabra.length() - tam_efectivo, tam_efectivo))
                C++;
        }
        it = c_w_end_t_res.emplace(piecewise_construct, forward_as_tuple(clave), forward_as_tuple(C)).first;
    }
    return it->second;
}

double Tagger::p_e_wt_unknown(const string &token, const string &tag)
{
    double sigma = 0;
    for (unsigned l = 1; l <= 5; l++)
    {
        size_t num = c_w_end_t(token, tag, l);
        size_t den = 0;
        for (const auto &tag : tag_tipos_palabra)
            den += c_w_end_t(token, tag.first, l);
        if (den != 0)
            sigma += alpha[l - 1] * (double)num / den;
    }
    return sigma;
}

double Tagger::p_e_wt(const string &token, const string &tag)
{
    unordered_map<ParejaString, double>::iterator it = B.find(ParejaString(token, tag));
    if (it != B.end())
        return it->second;
    return p_e_wt_unknown(token, tag);
}

void Tagger::etiquetar(const std::list<std::string> &oracion, std::list<std::string> &predicted_tags)
{
    if (oracion.size() < 2)
        cerr << "ERROR: La oración ha de contener al menos un bigrama" << endl;
    predicted_tags.clear();
    list<string>::const_iterator it_oracion = oracion.begin();
    Lattice lattice(tag_total_f.size(), oracion.size());
    lattice_ini(lattice, it_oracion);
    lattice_fill(lattice, it_oracion);
    lattice_trace(lattice, predicted_tags);
}