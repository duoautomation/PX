#pragma once
#include <Arduino.h>
#include <SD.h>
#include "csv.h"
#include <Ethernet.h>

class cloud
{
public:
    static inline bool internet_ok = false;
    static inline bool first = false;
    static inline bool sd_ok = false;
    static String linhaAguardando;
    static char *nomeCSV;
    static char serverName[];
    static String formatarJSON(String data);
    static bool marcarPonteiro(SDLib::File *ponteiro);
    static void logicaNuvem(char *arquivoEnviando,EthernetClient *ethClient,bool *precisaOutroArquivo,char *nomeCSV,int numSerie);
    static boolean sendPOST(char *data,EthernetClient *ethClient);
    static boolean sendGET(char *data, EthernetClient *client);
};
