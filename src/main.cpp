#include "tagger.h"
#include "tokenizador.h"

using namespace std;

int main()
{
    Tagger tagger;
    Tokenizador tok;
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(false);
    list<string> tokens;
    list<string> tags;
    tok.TokenizarString("Suddenly, the car stopped working.", tokens);
    tagger.entrenar("./resources/corpus_train_ingles.txt", 100);
    tagger.etiquetar(tokens, tags);
    list<string>::const_iterator it = tokens.begin();
    for (const string& tag : tags)
    {
        cout << *it << "\\" << tag << " ";
        it++;
    }
    cout << endl;
    return 0;
}