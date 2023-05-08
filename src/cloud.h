#pragma once
#include <Arduino.h>
#include <SD.h>
#include "csv.h"
#include <Ethernet.h>
#include <stdio.h>
#include <time.h>
#include "CLP.h"

class cloud
{
public:
    static int numero_falhas;
    static String linhaAguardando;
    static char *nomeCSV;
    static char serverName[];
    static String formatarJSON(String data);
    static String formatarListaJSON(String data);
    static bool marcarPonteiro(SDLib::File *ponteiro);
    static void logicaNuvem(EthernetClient *ethClient,int numSerie);
    static boolean sendPOST(char *data,EthernetClient *ethClient);
    static boolean sendGET(char *data, EthernetClient *client);
    static int tamanho_pacote(int linhas_csv, int linhas_ponteiro);
    //Algar
    static inline int pacote_fechado = 0;
    static inline EthernetClient *ethClient;
    static inline bool arquivos_ok = false;
    static inline char *arquivo_enviando ="L10-10.S";
    static inline char *nome_ponteiro ="XL10-10.S";
    static inline struct tm ua = {0};
    static inline int linhas_enviando = 0;
    static inline double intervalo_segundos= 10*60.0d; //De quanto em quanto tempo tentar atualizar
    static inline int tamanho_fila = 0;
    static bool verificar_arquivos();
    static struct tm ler_ua();
    static bool esta_na_hora();
    static void atualizar();
    static char *preparar_pacote(int inicio, int t_linhas);
    static bool ticar_linhas(int num_linhas);

};
