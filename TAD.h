struct dados {
	int simbolo, frequencia;
	char palavra[50];
	char codigoHuffman[50];
};
typedef struct dados Dados;

struct lista {
	Dados *D;
	struct lista *prox;
};
typedef struct lista Lista;

struct bits{
	unsigned char b7:1;
	unsigned char b6:1;
	unsigned char b5:1;
	unsigned char b4:1;
	unsigned char b3:1;
	unsigned char b2:1;
	unsigned char b1:1;
	unsigned char b0:1;
};
typedef struct bits Bits;

union byte{
	struct bits bi;
 	unsigned char num;
};
