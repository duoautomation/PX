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
int numSerie=25;

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

    digitalWrite(10,HIGH);
    digitalWrite(4,LOW);

    cloud::logicaNuvem(nomeCSV,&ethClient,&precisaOutroArquivo,nomeCSV,numSerie);

    counter++;

    delay(1000);
    CLP::Break("FIM");
}
void servir_post(EthernetClient client){

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    File webFile = SD.open(arquivoEnviando);        // open web page file
    if (webFile) {
    while(webFile.available()) {
        client.write(webFile.read()); // send web page to client
    }
    webFile.close();
}
}

void printarHTML(EthernetClient client){

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<center> DUO AUTOMATION</center>");
          client.println("Status SD: ");
          client.println(cloud::sd_ok);
          client.println("<br>");
          client.println("Status CLP: ");
          client.println(CLP::CLP_ok);
          client.println("<br>");
          client.println("Status Internet: ");
          client.println(cloud::internet_ok);
          client.println("<br>");
          client.println("<br>");
          client.println("</html>");
}
