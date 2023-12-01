#include "CLP.h"
#include "cloud.h"
#include "csv.h"
#include <Ethernet.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <utility/w5100.h>

//Variáveis Globais
char *arquivoEnviando="L10-10.S";
char *nomeCSV;
int passo;
bool shielded;
bool precisaOutroArquivo;

EthernetClient ethClient;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

EthernetServer server(80);
ModbusTCPClient modbusTCPClient(ethClient);
int counter;
int numE;
int numSerie = 4;

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
    delay(50);
    //pinMode(53,OUTPUT);
    SD.begin(4);
    W5100.setRetransmissionTime(0x07D0);
    W5100.setRetransmissionCount(3);

    Serial.begin(115200);
    Serial2.begin(115200);

    precisaOutroArquivo=true;
    iniciarEthernet();
    passo=-1;
    counter==0;
    numE=0;
    /* char *ankanis = "XL10-10.S"; */
    /* SD.remove(arquivoEnviando); */
    /* SD.remove(ankanis); */
}

void loop()
{
    CLP::Break("INICIO");
    Serial.println("DUO AUTOMATION");
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

    EthernetClient client = server.available();

    if(client){
        Serial.println("Novo CLiente");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean lido_primeiro = false;
    boolean e_post = false;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if(lido_primeiro == false && c == 'G'){
            Serial.println("GET");
            lido_primeiro = true;
        }else if(lido_primeiro == false && c == 'P'){
            Serial.println("POST");
            lido_primeiro = true;
            e_post = true;
        }
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
            if(e_post == false){
                printarHTML(client);
                break;
            }
                servir_post(client);
                break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    
}

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
