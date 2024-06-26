#pragma once
#include <Ethernet.h>
#include <Arduino.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include<SD.h>
#include "csv.h"

class CLP
{

    public:

        static int gNumE,counter, pneumaticoAntes,eletrodosQueimados;
        static bool first;
        static int chapiscando;
        struct Dados;
        static unsigned long tInicio;//
        static unsigned long tFim;//
        static int vEspecial;//
        static char *formatar_dados(struct Dados *dados);
        static char *contar(ModbusTCPClient *modbusTCPClient,char *nomeCSV,int *passo,int numSerie);
        static void Break(String texto);
        static bool especial(struct Dados *dados);
        static String comporTimeStamp(int dia,int mes,int ano,int hora,int minuto,int segundo);
        static void printar_dados(struct Dados *dados);
        static void ler_clp(struct Dados *dados, ModbusTCPClient *modbusTCPClient);

};
