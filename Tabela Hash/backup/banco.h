#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Vitor Pinheiro David
//Douglas Custodio de Araujo
//Jose Lucas Alves gomes

typedef struct Cliente
{
	int codCliente;
	char nome[100];
}cliente;

//funções para inserção na tabela:
	cliente* cria_cliente();

	void adiciona_cliente(cliente*,FILE*);

//funções para remoção da tabela:
	void remove_cliente(FILE*);

//pesquisar:
	cliente* pesquisa_cliente();

//funções impressao:
	void ler_clientes(FILE*);

	cliente* le_cliente(FILE*);

	void imprime_cliente(cliente*);

//auxiliares:
	int tamanho_cliente();

	//int comparar_cliente(const void* ,const void* );


	//int achar_cod_cliente(char*,FILE*,FILE*);