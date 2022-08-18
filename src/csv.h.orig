#pragma once
#include <stdlib.h>
#include<SD.h>
#include <SPI.h>

//AUTOR: PEDRO
//Esta classe cria e editar arquivos .csv
//O construtor cria um novo arquivo
//Um método retorna um arquivo existente
//O destrutor fecha o arquivo
//Um método escreve
//Um método lê arquivo inteiro
//Um método add linha
class csv{
    public:
    csv(String nome);
    ~csv();
    public:
    static int posicaoMudar;
    static int linhaAguardando;
    
    static void iSD();
    static void fSD();
    static void ls(char *dir);
    static void escrever(char *nomeArquivo,char *conteudo);
    static void ler(File *file); 
    static char *arquivoAguardando(char *diretorio,char *nomeArquivo);
    static char *lerLinhaN(SDLib::File *file,int lineNumber);
    static uint8_t rm(String file);
    static int contarLinhas(File *myFile);

    static void copy(char *origem, char *destino);    
    static void move(char *origem, char *destino);


};
