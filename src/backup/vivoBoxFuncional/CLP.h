#pragma once
#include <Ethernet.h>
#include <Arduino.h>
#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
#include<SD.h>
#include "csv.h"

class CLP{
    public:
    static int gNumE,counter, pneumaticoAntes,eletrodosQueimados;
    static unsigned long tInicio;//
    static unsigned long tFim;//
    static char *contar(ModbusTCPClient *modbusTCPClient,char *nomeCSV,int *passo,int numSerie);
    static void Break(String texto);
    static bool especial(int valeAtual,int nValesEsp,int nVales);
    static String comporTimeStamp(int dia,int mes,int ano,int hora,int minuto,int segundo);
};
