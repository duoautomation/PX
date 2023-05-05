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
//byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte mac[] = {0x70, 0xB3, 0xD5, 0x0A, 0xCB, 0x85};
IPAddress ip(192, 168, 1, 177);

ModbusTCPClient modbusTCPClient(ethClient);
int counter;
int numE;
int numSerie = 120;

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
    pinMode(35, INPUT); // Inserir registro qualquer
    pinMode(36, INPUT); // Ticar registro qualquer
    pinMode(10,OUTPUT);
    pinMode(4,OUTPUT);
    digitalWrite(10,HIGH);
    digitalWrite(4,HIGH);
    delay(5000);
    //pinMode(53,OUTPUT);
    SD.begin(4); 
    Serial.begin(9600);
    precisaOutroArquivo=true;
    iniciarEthernet();
    passo=-1;
    counter==0;
    numE=0;
    SD.remove("UA.S");
    cloud::ethClient = &ethClient;

}

void _loop(){
    Serial.println("TESTE");
    // char *json = cloud::preparar_pacote(10);
    // //char *json = "{\"pedro\":1}";
    // cloud::sendPOST(json, &ethClient);
    // free(json);
    delay(10000); 
}

void loop()
{
    CLP::Break("INICIO");
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
    Serial.print("--> Memoria: ");
    Serial.print(FreeRam());
    Serial.print("\n");

    // digitalWrite(10,HIGH);
    // digitalWrite(4,LOW);

    char ts[22]; 
    strftime(ts, 22,"%Y/%m/%d %H:%M:%S", &CLP::ua);
    Serial.print("Hora atual: ");Serial.print(ts);Serial.print("\n");

    cloud::logicaNuvem(&ethClient,&numSerie);
    counter++;

    //Opcoes

    int reg_false = digitalRead(35);
    int tic = digitalRead(36);

    if(reg_false == 1){
        Serial.print("Inserindo...");
        char t_falso[22]; 
        strftime(t_falso, 22,"%Y;%m;%d;%H;%M;%S;", &CLP::ua);
        char* d_falso = strcat(t_falso,"240;01;1;18913;6;1;120;1");
        Serial.println(d_falso);
        csv::escrever(cloud::arquivo_enviando, d_falso);
        numE++;
    }

    if(tic == 1){
        Serial.print("Ticando...");
        csv::escrever(cloud::nome_ponteiro, "X");
    }

}
