#include "tagger.h"
#include "tokenizador.h"

using namespace std;

void imprimirResultado(const list<string>& tokens, const list<string>& tags)
{
    cout << "\033[2J" << "\033[1;1H";
    cout << "Oración: " << endl;
    if (tags.size())
    {
        list<string>::const_iterator it = tokens.begin();
        for (const string &tag : tags)
        {
            cout << *it << "\\" << tag << " ";
            it++;
        }
        cout << endl;
    }
    else
    {
        cout << endl;
    }
    cout << "\033[1;11H";
}

void debug()
{
    Tagger tagger;
    Tokenizador tok;
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(false);
    list<string> tokens;
    list<string> tags;
    tok.TokenizarString("Secretariat won the race", tokens);
    tagger.entrenar("./resources/corpus_train_ingles.txt", 100);
    tagger.etiquetar(tokens, tags);
    imprimirResultado(tokens, tags);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Uso: tagger ruta/corpus/aprendizaje" << endl;
        return 0;
    }
    if (!Tokenizador::file_exists(argv[1]))
    {
        cout << "El fichero " << argv[1] << " no existe" << endl;
        return 0;
    }
    cout << "\033[2J" << "\033[1;1H";
    Tagger tagger;
    Tokenizador tok;
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(false);
    tagger.entrenar(argv[1]);
    string oracion = "";
    list<string> tokens;
    list<string> tags;
    cout << "Oración: ";
    while (true)
    {
        getline(cin, oracion);
        tok.TokenizarString(oracion, tokens);
        tagger.etiquetar(tokens, tags);
        imprimirResultado(tokens, tags);
    }
    return 0;
}