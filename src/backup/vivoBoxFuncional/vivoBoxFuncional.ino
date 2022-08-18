#include "CLP.h"
#include "cloud.h"
#include "csv.h"
#include <Ethernet.h>
#include <ArduinoRS485.h> 
#include <ArduinoModbus.h>
//Variáveis Globais
char *arquivoEnviando="L10-10.S";
char *nomeCSV;
int passo;
bool shielded;
bool precisaOutroArquivo;
EthernetClient ethClient;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

ModbusTCPClient modbusTCPClient(ethClient);
int counter;
int numSerie=-1;
void iniciarEthernet(){

  Ethernet.init(10);
  delay(1000);
  Ethernet.begin(mac, ip);
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Shiled Ethernet não encontrado :(");
        shielded=false;
  }else{
      shielded=true;
  }
}


void setup(){
    pinMode(10,OUTPUT);
    pinMode(4,OUTPUT);
    SD.begin(4);

    digitalWrite(10,LOW);
    digitalWrite(4,HIGH);

    Serial.begin(9600);
    precisaOutroArquivo=true;
    iniciarEthernet();
    passo=-1;
    counter==0;
}


void loop(){ 
    CLP::Break("INICIO"); 
    nomeCSV=(char *) malloc(9*sizeof(char));
    char *contagem;
    contagem=CLP::contar(&modbusTCPClient,nomeCSV,&passo,numSerie);

    if(strlen(contagem)>1){
        Serial.print("Contagem------>");Serial.print(contagem);Serial.print("\n");
        csv::escrever(nomeCSV,contagem);
    }
    free(contagem);
    Serial.print("Memoria------>");Serial.print(FreeRam());Serial.print("\n");
    Serial.print("PASSO------>");Serial.print(passo);Serial.print("\n");
    Serial.print("NOMECSV------>");Serial.print(nomeCSV);Serial.print("\n");
   if(precisaOutroArquivo){
        Serial.println("Alocando memoria...");
       precisaOutroArquivo=false;

    }
    
    cloud::logicaNuvem(nomeCSV,&ethClient,&precisaOutroArquivo,nomeCSV,numSerie); 

    counter++;
    
    if(nomeCSV!=NULL){
        free(nomeCSV);
    }

    CLP::Break("FIM");
}


