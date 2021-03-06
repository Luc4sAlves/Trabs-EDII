#include "hash.h"

#define M 7 // define o tamanho da tabela

//Vitor Pinheiro David
//Douglas Custodio de Araujo
//Jose Lucas Alves gomes

cliente *interador;//percorre a lista para verificar o codigo e me indica o final da lista 

int hash(int codigo){

	return codigo % M;
}

void inicializa(FILE* hashFile){
	int menosUm = -1;

	for(int i=0;i<M;i++){
		fwrite(&menosUm, sizeof(int), 1, hashFile);
	}
	remove("clientes.dat");
	FILE* aux = fopen("clientes.dat","w+b");
	fclose(aux);
}
//insere o cliente na tabela hash resetando todos os valores nescessarios.
void insere(cliente* cliente,FILE* tabHash,FILE* clientesDat){

	int cursor; // cursor para posicionar na tabHash
	int listFirst; // para verificar se na posicao achada ja tem uma lista ou esta vazia
	int posicao = 0; // contabiliza quantos registros foram lidos para atualizar tabela Hash
	int temp = 0;//me diz o status do cliente lido

	cursor = sizeof(int)*hash(cliente->codCliente);//calcula qual local da hash vai ficar o cliente

	fseek(tabHash,cursor,SEEK_SET);//prepara o ponteiro para ler o que tem na hash

	fread(&listFirst,sizeof(int),1,tabHash);//le a hash

	rewind(clientesDat);//confirma que o ponteiro de clientes esta no começo

	if(listFirst == -1)//se nao houver colisao
	{
		temp = le_status(clientesDat);//pega o codigo do primeiro cliente

		if(temp == -1){//se o arquivo de clientes já esta vazio
			adiciona_cliente(cliente,clientesDat);
		}
		else//se existe cliente
		{
			while(temp == 0)//se o cliente tem status 0 ele ainda existe, logo procura o final do arquivou ou um cliente com 1.
			{
				temp = le_status(clientesDat);//le o status do proximo cliente
				posicao++;//contador para saber o endereço do cliente atual
			}
			if(temp == 1){//se eu achei um cara que pode ser reescrito, preciso voltar para escrever sobre
				fseek(clientesDat,-tamanho_cliente(),SEEK_CUR);
			}

			adiciona_cliente(cliente,clientesDat);//escrevo o cliente no final do arquivou ou em cima de um vazio.
		}

		fseek(tabHash,cursor,SEEK_SET);
		fwrite(&posicao,sizeof(int),1,tabHash);
	}
	else{//se ja tem um cara com esse codigo, logo, existe colisao
		
		fseek(clientesDat,tamanho_cliente()*listFirst,SEEK_SET);
		interador = le_cliente(clientesDat);
		if(interador->codCliente == cliente->codCliente)
		{
			printf("Erro na insercao o codigo: %d já existe\n",interador->codCliente);
			return;
		}
		int ultimo = listFirst;//me diz o ultimo cliente da lista
		while(interador->prox != -1)
		{
			ultimo = interador->prox;
			fseek(clientesDat,tamanho_cliente()*interador->prox,SEEK_SET);
			interador = le_cliente(clientesDat);

			if(interador->codCliente == cliente->codCliente)
			{
				printf("Erro na insercao o codigo: %d já existe\n",interador->codCliente);
				return;
			}
		}

		rewind(clientesDat);
		//o restante é igual ao caso acima.
		temp = le_status(clientesDat);

		if(temp == -1){
			
			adiciona_cliente(cliente,clientesDat);
		}
		else{
			while(temp == 0){
				temp = le_status(clientesDat);
				posicao++;
			}
			if(temp == 1) fseek(clientesDat,-tamanho_cliente(),SEEK_CUR);

			adiciona_cliente(cliente,clientesDat);
		}
		interador->prox = posicao;//coloca o ponteiro do ultimo para apontar para o novo vliente
		fseek(clientesDat,ultimo*tamanho_cliente(),SEEK_SET);//coloca o ponteiro do arquivo para o ultimo
		adiciona_cliente(interador,clientesDat);//atualiza o valor do ultimo no arquivo.
	}

	//fflush(tabHash);
	//fflush(clientesDat);
}

int busca_cod(int codigo,FILE* tabHash,FILE* clientesDat)
{
	int cursor; // cursor para posicionar na tabHash
	int listFirst; //pega o endereço do primeiro cara.
	cliente* temp;//auxiliar para ter o cliente lido.

	cursor = sizeof(int)*hash(codigo);//calcula qual local da hash vai ficar o cliente

	fseek(tabHash,cursor,SEEK_SET);//prepara o ponteiro para ler o que tem na hash
	fread(&listFirst,sizeof(int),1,tabHash);//le a hash

	if(listFirst == -1) return -1;//o cliente não existe pq a lista esta vazia.
	else
	{
		fseek(clientesDat,listFirst*tamanho_cliente(),SEEK_SET);//vai para o primeiro da lista.
		temp = le_cliente(clientesDat);//le o primeiro cliente da lista.

		if(temp->codCliente == codigo)//o primeiro cliente da lista é o cliente com o codigo
		{
			return listFirst;
		}
		else
		{
			while(temp->prox != -1)
			{
				listFirst = temp->prox;//recebe o proximo da lista.
				fseek(clientesDat,listFirst*tamanho_cliente(),SEEK_SET);//vai para o proximo da lista.
				temp = le_cliente(clientesDat);//le o proximo cliente da lista.
				if(temp->codCliente == codigo) return listFirst;//encontrei no meio da lista.
			}
		}
	}
	return -1;//se tiver um problema eu saio retornando que não existe.
}

void busca(int codigo,FILE* tabHash,FILE* clientesDat)
{
	int onde = busca_cod(codigo,tabHash,clientesDat);

	if(onde != -1)
	{
		fseek(clientesDat,onde*tamanho_cliente(),SEEK_SET);//vai no cliente encontrado
		printf("O cliente econtrado é:\n\n");
		imprime_cliente(le_cliente(clientesDat));//imprime o cliente encontrado
	}
	else printf("\nO cliente procurado não existe\n\n");
}

void Remove(int codigo,FILE* tabHash,FILE* clientesDat)
{
	int onde = busca_cod(codigo,tabHash,clientesDat);//me pega o endereco de onde está o cliente a ser removido.
	int listFirst;//utilizado para saber se é o primeiro da lista
	cliente* temp;//temprario para pegar cliente e alterar os ponteiros
	cliente* remover;//o cliente que sera removido esta aqui.
	int sobreescrever;//me indica o endereco do cliente a ser reescrito pq os ponteiros foram alterados.]
	int cursor;//me diz onde pegar o valor na tabela hash;
	if(onde != -1)
	{
		cursor = sizeof(int)*hash(codigo);//calcula qual local da hash vai ficar o cliente
		fseek(tabHash,cursor,SEEK_SET);//prepara o ponteiro para ler o que tem na hash
		fread(&listFirst,sizeof(int),1,tabHash);//le a hash

		fseek(clientesDat,onde*tamanho_cliente(),SEEK_SET);//vai no cliente encontrado
		remove_cliente(clientesDat);//altera status para 1;

		fseek(clientesDat,onde*tamanho_cliente(),SEEK_SET);//vai no cliente encontrado
		remover = le_cliente(clientesDat);//pega o cliente que sera removido
		if(onde == listFirst)
		{
			listFirst = remover->prox;
			fseek(tabHash,cursor,SEEK_SET);//prepara o ponteiro para ler o que tem na hash
			fwrite(&listFirst,sizeof(int),1,tabHash);
		}
		else
		{
			fseek(clientesDat,listFirst*tamanho_cliente(),SEEK_SET);//vai no 1 da lista do removido
			temp = le_cliente(clientesDat);
			sobreescrever = listFirst;
			while(temp->prox != -1)
			{
				if(temp->prox == onde)
				{
					temp->prox = remover->prox;
					break;
				}
				else
				{
					fseek(clientesDat,temp->prox*tamanho_cliente(),SEEK_SET);//vai no 1 da lista do removido
					sobreescrever = temp->prox; 
					temp = le_cliente(clientesDat);
				}
			}
			fseek(clientesDat,sobreescrever*tamanho_cliente(),SEEK_SET);//vai no 1 da lista do removido
			adiciona_cliente(temp,clientesDat);
		}

	}
	else printf("\nO cliente procurado não existe\n\n");
}








































/*for(int i=0;i<M;i++){
		fwrite(&menosUm, sizeof(int), 1, hashFile);
		write('a', sizeof(char), sizeof(cliente->nome), hashFile);
		write(&menosUm, sizeof(int), sizeof(cliente*), hashFile);
	}
*/