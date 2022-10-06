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
int numE;
int numSerie=22;

void iniciarEthernet()
{

    Ethernet.init(10);
    delay(1000);
    Ethernet.begin(mac, ip);
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        Serial.println("Shiled Ethernet não encontrado :(");
        shielded=false;
    }
    else
    {
        shielded=true;
    }
}


void setup()
{
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
    numE=0;

}


void loop()
{
    CLP::Break("INICIO");
    //nomeCSV=(char *) malloc(9*sizeof(char));
    Serial.print("--> Eletrodos Queimados: ");
    Serial.print(numE);
    Serial.print("\n");

    nomeCSV="L10-10.S";
    char *contagem;
    contagem = CLP::contar(&modbusTCPClient,nomeCSV,&passo,numSerie);

    if(strlen(contagem)>2)
    {  
        csv::escrever(nomeCSV,contagem); 
        numE++;
    }

    free(contagem);
    Serial.print("Memoria------>");
    Serial.print(FreeRam());
    Serial.print("\n");


    cloud::logicaNuvem(nomeCSV,&ethClient,&precisaOutroArquivo,nomeCSV,numSerie);

    counter++;

    delay(1000);
    CLP::Break("FIM");
}
