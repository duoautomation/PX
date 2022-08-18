#include "CLP.h"
#include "cloud.h"
#include "csv.h"
#include <Ethernet.h>
#include <ArduinoRS485.h> 
#include <ArduinoModbus.h>
//Variáveis Globais
char *arquivoEnviando;
char *nomeCSV;
int passo;
bool shielded;
bool precisaOutroArquivo;
EthernetClient ethClient;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

ModbusTCPClient modbusTCPClient(ethClient);
int counter;
char *numSerie="317";
void iniciarEthernet(){

  Ethernet.init(10);
  delay(1000);
  Ethernet.begin(mac, ip);
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
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

    CLP::Break(numSerie); 
    nomeCSV=(char *) malloc(9*sizeof(char));
    char *contagem;
    contagem=CLP::contar(&modbusTCPClient,nomeCSV,&passo,numSerie);

    if(strlen(contagem)>1){
        Serial.print("Contagem------>");Serial.print(contagem);Serial.print("\n");
        csv::escrever(nomeCSV,contagem);
    }
    free(contagem);
    Serial.print("M1emoria------>");Serial.print(FreeRam());Serial.print("\n");
    Serial.print("PASSO------>");Serial.print(passo);Serial.print("\n");
    Serial.print("NOMECSV------>");Serial.print(nomeCSV);Serial.print("\n");
   if(precisaOutroArquivo){
        arquivoEnviando = csv::arquivoAguardando("/",nomeCSV);
       precisaOutroArquivo=false;
      cloud::nomeCSV=nomeCSV;
    }
    
    cloud::logicaNuvem(arquivoEnviando,&ethClient,&precisaOutroArquivo); 
    csv::ls("/");
    counter++;
    if(counter==10){
        counter=0;
        pinMode(52,HIGH);
        delay(2000); 
    }
    
    Serial.print("NomeCSV------>");Serial.print(nomeCSV);Serial.print("\n");
    if(nomeCSV!=NULL){
        free(nomeCSV);
    }
    CLP::Break("FIM");
}



/* void loop2(){ */ 
/*     CLP::Break(numSerie); */ 
/*     nomeCSV=(char *) malloc(9*sizeof(char)); */
/*     strcpy(nomeCSV,"TESTE.TXT"); */
/*     Serial.print("M1emoria------>");Serial.print(FreeRam());Serial.print("\n"); */
/*     Serial.print("NomeCSV------>");Serial.print(nomeCSV);Serial.print("\n"); */

/*    /1* if(precisaOutroArquivo){ *1/ */
/*        arquivoEnviando = csv::arquivoAguardando("/",nomeCSV); */
/*        /1* precisaOutroArquivo=false; *1/ */
/*     /1* } *1/ */
    
/*     Serial.print("Arquivo Enviando LOOP------>");Serial.print(arquivoEnviando);Serial.print("\n"); */
/*     cloud::nomeCSV=nomeCSV; */
/*     cloud::logicaNuvem(arquivoEnviando,&ethClient,&precisaOutroArquivo); */ 


/*     counter++; */
/*     free(nomeCSV); */ 
/*     free(arquivoEnviando); */ 
/*     CLP::Break("FIM"); */
/* } */

/* void loop(){ */
    
/*     Serial.print("Memoria------>");Serial.print(FreeRam());Serial.print("\n"); */
/*     CLP::Break(numSerie); */ 
    
/*     nomeCSV=(char *) malloc(9*sizeof(char)); */
/*     char *contagem; */
/*     //contagem=CLP::contar(&modbusTCPClient,nomeCSV,&passo,numSerie); */

/*     if(strlen(contagem)>1){ */
/*         Serial.print("STRLEN------>");Serial.print(strlen(contagem));Serial.print("\n"); */
/*         Serial.print("Contagem------>");Serial.print(contagem);Serial.print("\n"); */
/*         csv::escrever(nomeCSV,contagem); */
/*     } */
/*         free(contagem); */
    
/*     //csv::rm("*"); */
/*     if(SD.exists(nomeCSV)){ */
/*     arquivoEnviando = csv::arquivoAguardando("/",nomeCSV); */
/*     if(arquivoEnviando==NULL){ */
/*         Serial.print("NULL"); */
/*         arquivoEnviando=nomeCSV; */
/*     } */
/*     Serial.print("Arquivo Aguardando------>");Serial.print(arquivoEnviando);Serial.print("<--\n"); */
     
/*     Serial.print("NOMECSV------>");Serial.print(nomeCSV);Serial.print("<--\n"); */
/*     cloud::nomeCSV=nomeCSV; */
/*     cloud::logicaNuvem(arquivoEnviando,&ethClient,&precisaOutroArquivo); */ 

/*     free(arquivoEnviando); */ 
/*     }else{ */
/*         File temp= SD.open(nomeCSV,FILE_WRITE); */
/*         temp.close(); */
/*     } */
/*     if(nomeCSV!=NULL){ */
/*         free(nomeCSV); */
/*     } */
/*     csv::ls("/"); */
/*     CLP::Break("FIM"); */
/* } */
