#include "tagger.h"
#include "tokenizador.h"

int main()
{
    Tagger tagger;
    tagger.entrenar("./resources/corpus_train_ingles.txt", 100);
    return 0;
}