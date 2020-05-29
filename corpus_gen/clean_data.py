RUTA_CORPUS = "../resources/lexesp.txt"
RUTA_SALIDA = "./corpus.txt"
DEBUG_MAX = 1000

in_file = open(RUTA_CORPUS, 'r')
out_file = open(RUTA_SALIDA, 'w')
for i in range(DEBUG_MAX):
    line = in_file.readline().replace("\n", "")
    line = in_file.readline().replace(chr(13), "")
    tokens = []
    for token in line.split(' '):
        if token:
            tokens.append(token)
    out_file.write(tokens[0] + " " + tokens[2][:2] + "\n")
in_file.close()
out_file.close()