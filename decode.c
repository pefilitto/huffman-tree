#include <stdio.h>
#include <string.h>
#include "TAD.h"

struct arvorehuffman {
	struct arvorehuffman *esq, *dir;
	int simbolo;
};
typedef struct arvorehuffman ArvoreHuffman;

Lista* CriaCaixaLista(Dados *D) {
    Lista *nova = (Lista*)malloc(sizeof(Lista));
    nova->D = (Dados*)malloc(sizeof(Dados));
    *(nova->D) = *D;
    nova->prox = NULL;
    return nova;
}

ArvoreHuffman *CriaNoArvore(int simbolo){
	ArvoreHuffman *nova = (ArvoreHuffman *)malloc(sizeof(ArvoreHuffman));
	nova -> esq = nova -> dir = NULL;
	nova -> simbolo = simbolo;
	return nova;
}

Lista* CarregaLista() {
    FILE *ptr = fopen("binario.dat", "rb");
    Lista *primeira = NULL, *aux, *ant = NULL;
    Dados D;
    union byte Un;
    int i;
    char cod[50];

	if(ptr == NULL){
		printf("Por favor, gere o binario!\n");
	}
	else{
		while (fread(&D, sizeof(Dados), 1, ptr) == 1) {    	 
	        Lista *nova = CriaCaixaLista(&D);
	        
	        if (primeira == NULL) {
	            primeira = nova;
	        } 
			else {
	            aux = primeira;
	            while (aux != NULL) {
	            	ant = aux;
	                aux = aux->prox;
	            }
	            ant->prox = nova;
	            nova -> prox = aux;
        	}
    	}
    	fclose(ptr);
	}
    return primeira;
}

void InsereNaArvore(ArvoreHuffman * * Arvore, char *cod, int simbolo){
	ArvoreHuffman *aux;
	int i=0;
	
	if(*Arvore == NULL){
		*Arvore = CriaNoArvore(-1);
	}
	aux = *Arvore;
	while(cod[i] != '\0'){
		if(cod[i] == '0'){
			if(aux -> esq == NULL){
				aux -> esq = CriaNoArvore(-1);	
			}
			aux = aux -> esq;
		}
		else if(cod[i] == '1'){
			if(aux -> dir == NULL){
				aux -> dir = CriaNoArvore(-1);
			}
			aux = aux -> dir;
		}
		i++;
	}
	aux -> simbolo = simbolo;

}

ArvoreHuffman * ConstroiArvoreAPartirDosCodigos(Lista * L){
	ArvoreHuffman *Arvore = NULL;
	Lista *aux = L;
	while(aux != NULL){
		InsereNaArvore(&Arvore, aux -> D -> codigoHuffman, aux -> D -> simbolo);
		
		aux = aux -> prox;
	}
	return Arvore;
}

void BuscaPalavra(Lista * L, int simbolo){
	while(L != NULL && L -> D -> simbolo != simbolo){		
		L = L -> prox;
	}
	
	if(L == NULL)
		printf("palavra-desconhecida\n");
	else{
		printf("%s", L -> D -> palavra);
	}
}

void BuscaSimbolo(ArvoreHuffman *Arvore, Lista *L, char *nomeArq) {
    FILE *ptr = fopen(nomeArq, "r");
    ArvoreHuffman *aux, *pai;
    int bitsLidos = 0;
    char c;
    char str[50];
    int i = 0, j;

    if (ptr != NULL) {
        aux = Arvore;
        c = fgetc(ptr);
        while (!feof(ptr)) {
            str[i] = c;
            if (c == '0') {
            	pai = aux;
                aux = aux->esq;
            } 
			else if (c == '1') {
				pai = aux;
                aux = aux->dir;
            } 
			else {
                if (!feof(ptr)) {
                    printf("Caracter invalido\n");
                }
            }

            bitsLidos++;
            
            if (aux->dir == NULL && aux->esq == NULL) {
                BuscaPalavra(L, aux->simbolo);
                aux = Arvore;
                i = 0;
                str[1] = '\0';

                int bitsRestantes = 8 - (bitsLidos % 8);
                if (bitsRestantes < 8) {
                    for (j = 0; j < bitsRestantes; j++) {
                        c = fgetc(ptr);
                    }
                    bitsLidos = 0;
                }
            } 
			else {
                i++;
            }
            c = fgetc(ptr);
        }
    }
    fclose(ptr);
}

void ImprimeLista(Lista *lista) {
    while (lista != NULL) {
        printf("Simbolo: %d\tFrequencia: %d\tPalavra: %s\tCodigo Huffman: %s\n",
               lista->D->simbolo, lista->D->frequencia, lista->D->palavra, lista->D->codigoHuffman);
        lista = lista->prox;
    }
}

void ExibeArquivoBinario(const char *nomeArquivo) {
    FILE *ptr = fopen(nomeArquivo, "rb");
    if (ptr == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    Dados dados;

    printf("Dados do arquivo binário %s:\n", nomeArquivo);
    printf("-----------------------------------------------------\n");
    printf("| Simbolo | Frequencia | Palavra            | Codigo Huffman |\n");
    printf("-----------------------------------------------------\n");

    while (fread(&dados, sizeof(Dados), 1, ptr) == 1) {
        printf("| %-8d| %-11d| %-20s| %-15s|\n", dados.simbolo, dados.frequencia, dados.palavra, dados.codigoHuffman);
    }

    printf("-----------------------------------------------------\n");

    fclose(ptr);
}


int main() {
    Lista *Lista;
    ArvoreHuffman *Arvore;
	Lista = CarregaLista();
	Arvore = ConstroiArvoreAPartirDosCodigos(Lista);
	//ImprimeLista(Lista);
	//ExibeArquivoBinario("binario.dat");
	BuscaSimbolo(Arvore, Lista, "textoCodificado.txt");
}
