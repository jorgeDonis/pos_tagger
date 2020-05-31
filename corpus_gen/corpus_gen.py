import nltk

tagged_corpus_file = "./corpus_train_ingles.txt"

file = open(tagged_corpus_file, 'w', encoding='iso-8859-1')
i = 1
num_words = 0
for sentence in nltk.corpus.brown.tagged_sents(tagset = 'universal'):
    out_str = str(encoding='iso-8859-1')
    for word in sentence:
        out_str += word[0] + chr(30) + word[1] + " "
        num_words += 1
    out_str += "\n"
    file.write(out_str)
    i += 1
file.close()