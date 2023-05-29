#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <stack>


struct NoHuffman {
    std::string c;
    float frequencia;
    NoHuffman *lde, *ldo;
};


NoHuffman *gerar_arvore(std::vector<NoHuffman *> &simbolos) {
    NoHuffman *raiz = new NoHuffman();
    int countNull = 0;
    while (simbolos.size() > 1) {
        NoHuffman *novoNo = new NoHuffman();
        novoNo->frequencia = simbolos[simbolos.size() - 1]->frequencia + simbolos[simbolos.size() - 2]->frequencia;
        novoNo->c = "NULL " + std::to_string(countNull);
        countNull++;
        novoNo->ldo = simbolos[simbolos.size() - 1];
        novoNo->lde = simbolos[simbolos.size() - 2];
        simbolos.pop_back(); simbolos.pop_back();

        int size = simbolos.size();
        for (int i = 0; i < size; i++) {
            if (simbolos[i]->frequencia <= novoNo->frequencia) {
                simbolos.insert(simbolos.begin() + i, novoNo);
                break;
            }
            else if (size == i + 1) {
                simbolos.push_back(novoNo);
            }
        }

        raiz = novoNo;
    }

    return raiz;
}

void getPre(NoHuffman* raiz, std::vector<std::string> &preOrderVector) {
    if (raiz == NULL) {
        return;
    }

    preOrderVector.push_back(raiz->c);
    getPre(raiz->lde, preOrderVector);
    getPre(raiz->ldo, preOrderVector);
}

void getSim(NoHuffman* raiz, std::vector<std::string> &simOrderVector) {
    if (raiz == NULL) {
        return;
    }

    getSim(raiz->lde, simOrderVector);
    simOrderVector.push_back(raiz->c);
    getSim(raiz->ldo, simOrderVector);
}

void writeOrder(NoHuffman *&raiz) {
    std::vector<std::string> pre;
    getPre(raiz, pre);

    std::ofstream preOrderFile("order/pre.txt");
    if (preOrderFile.is_open()) {
        preOrderFile << '[';
        for (std::string el : pre) {
            preOrderFile << '\'' << el << '\'' << ' ';
        }
        preOrderFile << ']';
    }
    preOrderFile.close();

    std::vector<std::string> sim;
    getSim(raiz, sim);
    std::ofstream simOrderFile("order/sim.txt");
    if (simOrderFile.is_open()) {
        simOrderFile << '[';
        for (std::string el : sim) {
            simOrderFile << '\'' << el << '\'' << ' ';
        }
        simOrderFile << ']';
    }
    simOrderFile.close();
}

void gerarMap(NoHuffman* raiz, const std::string& codigo, std::map<std::string, std::string>& map) {
    if (raiz->lde == NULL && raiz->ldo == NULL) {
        map[raiz->c] = codigo;
        return;
    }

    gerarMap(raiz->lde, codigo + "1", map);
    gerarMap(raiz->ldo, codigo + "0", map);
}

void encode(NoHuffman *&raiz) {
    std::map<std::string, std::string> map;
    std::string codigo;
    gerarMap(raiz, codigo, map);
    std::ifstream toEncode("text/to_encode.txt");
    std::ofstream encodedFile("text/to_decode.bin", std::ios::binary);
    if (toEncode.is_open() && encodedFile.is_open()) {
        std::string line;
        while (std::getline(toEncode, line)) {
            for (char c : line) {
                std::string str(1, c);
                std::string::size_type len = map[str].length();
                const char* data = map[str].data();
                encodedFile.write(data, len);
            }
            encodedFile << "\n";
        }
    }
}

int main() {
    std::vector<NoHuffman *> simbolos;
    float total = 0;

    std::ifstream toEncode("text/to_encode.txt");
    if (toEncode.is_open()) {
        std::string line;
        while (std::getline(toEncode, line)) {
            for (int i = 0; i < line.length(); i++) {
                int j;
                std::string s(1, line[i]);
                for (j = 0; j < simbolos.size(); j++) {
                    if (simbolos[j]->c == s) {
                        simbolos[j]->frequencia += 1;
                        total += 1;
                        break;
                    }
                }

                if (j == simbolos.size()) {
                    NoHuffman *novoNo = new NoHuffman();
                    novoNo->c = s;
                    novoNo->frequencia = 1;
                    simbolos.push_back(novoNo);
                    total += 1;
                }
            }
        }
    }
    else {
        return 1;
    }
    toEncode.close();

    for (int i = 0; i < simbolos.size(); i++) {
        simbolos[i]->frequencia /= total;
    }

    std::sort(simbolos.begin(), simbolos.end(), [](NoHuffman* &le, NoHuffman* &ld){ return le->frequencia > ld->frequencia; });
    NoHuffman *raiz = gerar_arvore(simbolos);
    writeOrder(raiz);
    encode(raiz);

    return 0;
}