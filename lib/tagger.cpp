#include "tagger.h"
#include <iostream>

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
        A[pareja_tags.first] = (float) pareja_tags.second / tag_prev_f[pareja_tags.first.izquierda];
    
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
    tag_prev = tag;
}
