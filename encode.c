#include <stdio.h>
#include <string.h>
#include "TAD.h"

struct arvorehuffman {
	struct arvorehuffman *esq, *dir;
	int simbolo, frequencia;
};
typedef struct arvorehuffman ArvoreHuffman;

struct floresta {
	ArvoreHuffman *raiz;
	struct floresta *prox;
};
typedef struct floresta Floresta;

Lista * CriaCaixa(char palavra[], int simbolo){
	Lista *novaCaixa = (Lista *)malloc(sizeof(Lista));
	novaCaixa -> D = (Dados*)malloc(sizeof(Dados));
	
	strcpy(novaCaixa -> D -> palavra, palavra);
	novaCaixa -> D -> simbolo = simbolo;
	novaCaixa -> D -> frequencia = 1;
	strcpy(novaCaixa -> D -> codigoHuffman, "0");
	novaCaixa -> prox = NULL;
	return novaCaixa;
}

Floresta * CriaCaixaFloresta(ArvoreHuffman *no){
	Floresta *F = (Floresta*)malloc(sizeof(Floresta));
	F -> raiz = no;
	F -> prox = NULL;
	return F;
}

void ExibeArvore(ArvoreHuffman *raiz)
{
	if(raiz != NULL){
		printf("Frequencia: %d\t Simbolo: %d\n", raiz -> frequencia, raiz -> simbolo);
		ExibeArvore(raiz -> esq);
		ExibeArvore(raiz -> dir);
	}
}

ArvoreHuffman *CriaNoArvore(int simbolo, int frequencia, ArvoreHuffman *esq, ArvoreHuffman *dir) {
    ArvoreHuffman *novoNo = (ArvoreHuffman *)malloc(sizeof(ArvoreHuffman));
    novoNo -> simbolo = simbolo;
    novoNo -> frequencia = frequencia;
    novoNo -> esq = esq;
    novoNo -> dir = dir;
    return novoNo;
}

void BuscaCodigo(Lista * L, char palavra[], char codigo[]){
	while (L != NULL && strcmp(L -> D -> palavra, palavra) != 0) {
        L = L -> prox;
    }
    if(L != NULL){
    	strcpy(codigo, L -> D -> codigoHuffman);
    }
    else codigo[0] = '\0';
}

void InsereCodigoByteByte(char codigo[], FILE *ptr){
	union byte Un;
	int i = 0;   	
	char *cod = codigo;
    while (i < strlen(cod)) {
        
        Un.num = 0;

        Un.bi.b7 = cod[i++] - '0';
        Un.bi.b6 = cod[i++] - '0';
        Un.bi.b5 = cod[i++] - '0';
        Un.bi.b4 = cod[i++] - '0';
        Un.bi.b3 = cod[i++] - '0';
        Un.bi.b2 = cod[i++] - '0';
        Un.bi.b1 = cod[i++] - '0';
        Un.bi.b0 = cod[i++] - '0';

        fprintf(ptr, "%d%d%d%d%d%d%d%d",
            Un.bi.b7, Un.bi.b6, Un.bi.b5, Un.bi.b4,
            Un.bi.b3, Un.bi.b2, Un.bi.b1, Un.bi.b0);
    }
}

void BuscaInsereCodigo(Lista * L, char palavra[], char codigo[], FILE *ptr){
	BuscaCodigo(L, palavra, codigo);
    InsereCodigoByteByte(codigo, ptr);
    palavra[0] = ' ';
    palavra[1] = '\0';
    BuscaCodigo(L, palavra, codigo);
    InsereCodigoByteByte(codigo, ptr);
    codigo[0] = '\0';
    palavra[0] = '\0';
}

void PercorreSegundaFrase(Lista *L, FILE *arquivo) {
    FILE *ptr = fopen("textoCodificado.txt", "w");
    char c[2], string[50], codigo[50];
    c[1] = '\0';
    string[0] = '\0';
    
    c[0] = fgetc(arquivo);
    while(!feof(arquivo)){
    	if(c[0] != ' ')
    		strcat(string, c);
    	if(c[0] == ' ')
    		BuscaInsereCodigo(L, string, codigo, ptr);
    	c[0] = fgetc(arquivo);
    	while(c[0] == ',' || c[0] == '.'){
    		c[0] = fgetc(arquivo);
    	}
    	if(feof(ptr))
			BuscaInsereCodigo(L, string, codigo, ptr);		
    }
    if(feof(arquivo)){
    	string[strlen(string)-1] = '\0';
    	BuscaInsereCodigo(L, string, codigo, ptr);
    }
		
    
    printf("Frase codificada com sucesso!\n");
    fclose(ptr);
}

void InsereNoArquivoBinario(Lista * L){
	FILE *ptr = fopen("binario.dat", "wb");
	
	if(ptr != NULL){
		Lista *aux = L;
		
		while(aux != NULL){
			fwrite(aux -> D, sizeof(Dados), 1, ptr);
			aux = aux -> prox;
		}
	}
	
	fclose(ptr);
}

void GeraCodigoHuffman(ArvoreHuffman *raiz, char *codigo, int profundidade, Lista * L) {
    if (raiz -> esq == NULL && raiz -> dir == NULL) {
        Lista *temp = L;
        while (temp != NULL) {
            if (temp -> D -> simbolo == raiz -> simbolo) {
                codigo[profundidade] = '\0';
                strcpy(temp -> D -> codigoHuffman, codigo);
            }
            temp = temp -> prox;
        }
    }
    else
	{
    	if (raiz->esq != NULL) {
	        codigo[profundidade] = '0';
	        GeraCodigoHuffman(raiz -> esq, codigo, profundidade + 1, L);
	    }
	    
	    if (raiz->dir != NULL) {
	        codigo[profundidade] = '1';
	        GeraCodigoHuffman(raiz -> dir, codigo, profundidade + 1, L);
	    }
    }
}

void ConstroiCodigoHuffman(Lista * L, ArvoreHuffman *raiz) {
    char codigo[50];
    GeraCodigoHuffman(raiz, codigo, 0, L);
}

void InsereFlorestaDeArvores(Floresta **F, ArvoreHuffman *no) {
    Floresta *ant, *aux, *novaCaixa;

    novaCaixa = CriaCaixaFloresta(no);

    if (*F == NULL) {
        *F = novaCaixa;
    } 
	else {
        aux = *F;
        ant = NULL;
        while (aux != NULL && aux->raiz->frequencia < novaCaixa->raiz->frequencia) {
            ant = aux;
            aux = aux->prox;
        }

        if (ant == NULL) {
            novaCaixa->prox = *F;
            *F = novaCaixa;
        } 
		else {
            novaCaixa->prox = aux;
            ant->prox = novaCaixa;
        }
    }
}

void CriaArvoreHuffman(Floresta * * F) {
    ArvoreHuffman *novoNo;
    while (*F != NULL && (*F)->prox != NULL) {
        Floresta *primeiro = *F;
        Floresta *segundo = primeiro->prox;

        int somaFrequencias = primeiro->raiz->frequencia + segundo->raiz->frequencia;
        novoNo = CriaNoArvore(-1, somaFrequencias, primeiro->raiz, segundo->raiz);

        *F = segundo->prox;
        free(primeiro);
        free(segundo);

        InsereFlorestaDeArvores(F, novoNo);
    }
}

void PercorreLista(Lista *L, ArvoreHuffman * * raiz) {
    Floresta *F = NULL;

    while (L != NULL) {
        ArvoreHuffman *novoNo = CriaNoArvore(L->D->simbolo, L->D->frequencia, NULL, NULL);
        InsereFlorestaDeArvores(&F, novoNo);

        L = L->prox;
    }

    CriaArvoreHuffman(&F);
    *raiz = F -> raiz;
}

void InserePalavraNaLista(Lista * * L, char palavra[]){
	Lista *aux, *ant;
	int simbolo = 0;
	
	if(*L == NULL){
		*L = CriaCaixa(palavra, simbolo);
	}
	else{
		aux = *L;
		ant = NULL;
		while(aux != NULL && strcmp(aux -> D -> palavra, palavra) != 0){
			ant = aux;
			aux = aux -> prox;
		}
		
		if(aux == NULL){
			ant -> prox = CriaCaixa(palavra, ant -> D -> simbolo + 1);
		}
		else{
			aux -> D -> frequencia += 1;
		}
	}
}

void ConstroiLista(Lista * * L, FILE *ptr) {
    char c;
    char palavra[100];
    char caractere[2];
    int index = 0;

    if (ptr == NULL) {
        printf("Nao foi possivel abrir o arquivo\n");
    } 
	else {
        while ((c = fgetc(ptr)) != EOF) {
            if (c != ' ' && c != '.' && c != ',' && c != '\n') {
                palavra[index++] = c;
            } 
			else {
                if (index > 0) {
                    palavra[index] = '\0';
                    InserePalavraNaLista(&(*L), palavra);
                    index = 0;
                }
                if(c == ' '){
                	caractere[0] = ' ';
                	caractere[1] = '\0';
                	InserePalavraNaLista(&(*L), caractere);
                }
				else if (c == '.') {
					caractere[0] = '.';
                	caractere[1] = '\0';
                    InserePalavraNaLista(&(*L), caractere);
                } 
				else if (c == ',') {
					caractere[0] = ',';
                	caractere[1] = '\0';
                    InserePalavraNaLista(&(*L), caractere);
                }
            }
        }
        if (index > 0) {
            palavra[index] = '\0';
            InserePalavraNaLista(&(*L), palavra);
        }
    }
}

void LeArquivo(){
	ArvoreHuffman *raiz;
	FILE *ptr = fopen("frase.txt", "r");
	FILE *ptr2 = fopen("frase2.txt", "r");
	
	Lista *L;
	L = NULL;
	
	ConstroiLista(&L, ptr);
	PercorreLista(L, &raiz);
	ConstroiCodigoHuffman(L, raiz);
	InsereNoArquivoBinario(L);
	PercorreSegundaFrase(L, ptr2);
}

int main()
{
	LeArquivo();
}


