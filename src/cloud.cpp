#include "cloud.h"


String cloud::formatarJSON(String data)
{
    String retorneMe="{\"raw\":\""+data+"\"}";
    return retorneMe;
}

String cloud::linhaAguardando="";
static char serverName[]="adm.duo.com.br";

void cloud::logicaNuvem(char *arquivoEnviando,EthernetClient *ethClient,bool *precisaOutroArquivo,char *nomeCSV,int numSerie)
{
    Serial.println("Procurando dados para enviar para a DUO...");
    char *nomePonteiro=(char *) malloc(13*sizeof(char));
    snprintf(nomePonteiro,13*sizeof(char),"X%s",arquivoEnviando);
    Serial.print("Arquivo enviando ");
    Serial.print(arquivoEnviando);
    Serial.print("\n");
    Serial.print("Arquivo ponteiro ");
    Serial.print(nomePonteiro);
    Serial.print("\n");

    File file;
    if(!SD.exists(nomePonteiro))
    {
        Serial.println(nomePonteiro);
        file= SD.open(nomePonteiro,FILE_WRITE);
        if(file)
        {
            file.close();
        }
        file.close();
    }

    file=SD.open(arquivoEnviando,FILE_READ);
    int linhasCSV = csv::contarLinhas(&file);
    file.close();

    file=SD.open(nomePonteiro,FILE_READ);
    int linhasPonteiro = csv::contarLinhas(&file);
    file.close();

    Serial.print("Linhas_P---------->");
    Serial.print(linhasPonteiro);
    Serial.print("\n");
    Serial.print("Linhas_CSV---------->");
    Serial.print(linhasCSV);
    Serial.print("\n");

    Serial.print("|---->");
    Serial.print(strcmp(nomeCSV,arquivoEnviando));
    Serial.print("\n");

    if(linhasCSV!=-1 || linhasPonteiro!=-1)
    {
        if(linhasCSV>linhasPonteiro)
        {
            //Pegamos no ponteiro a linha a enviar
            file= SD.open(arquivoEnviando,FILE_READ);
            char *linha = csv::lerLinhaN(&file,linhasPonteiro);
            file.close();//Fechar arquivo
            Serial.print("Linha enviando--------->");
            Serial.print(linha);
            Serial.print("\n");
            int size=strlen(linha);
            if(size>0)
            {
                size=size+12;
                char *json =(char *) malloc(size*sizeof(char));
                snprintf(json,size*sizeof(char),"{\"raw\":\"%s\"}",linha);
                Serial.println(json);
                Serial.print("Ponteiro---->");
                Serial.print(nomePonteiro);
                Serial.print("\n");
                bool result = sendPOST(json,ethClient);
                free(json);

                if(result)
                {
                    Serial.println("Enviado");
                    csv::escrever(nomePonteiro,"P");
                }
                else
                {
                    Serial.println("Erro HTTP");
                }

            }
            free(linha);
        }
        else
        {
            //Fazemos um POST simples só para informar ao servidor que está conectado
            int size=16;
            char *json=(char *) malloc(size*sizeof(char));
            snprintf(json,size*sizeof(char),"{\"raw\":\"%d\"}",numSerie);
            sendPOST(json,ethClient);
            free(json);

        }
    }

    free(nomePonteiro);
}

bool cloud::marcarPonteiro(SDLib::File *ponteiro)
{
    bool retorneMe=false;
    if(*ponteiro)
    {
        ponteiro->println("P");
        retorneMe=true;
    }

    return retorneMe;
}


boolean cloud::sendPOST(char *data,EthernetClient *ethClient)
{
    //  char serverName[] = "adm.duo.com.br";
    ethClient->setTimeout(15000);
    Serial.print("6Memoria------>");
    Serial.print(FreeRam());
    Serial.print("\n");
    char serverName[]="23.239.10.90";
    char *readString;
    int size=1;
    if (ethClient->connect(serverName,8080))
    {
        Serial.println("connected");
        ethClient->println("POST / HTTP/1.1");
        ethClient->println("Host: adm.duo.com.br");
        ethClient->println("Content-Type:application/json");
        ethClient->print("Content-Length:");
        ethClient->print(strlen(data));
        ethClient->print("\n");
        ethClient->println("");
        ethClient->println(data);
        ethClient->println("Connection: close");
        ethClient->println();
        Serial.print("7Memoria------>");
        Serial.print(FreeRam());
        Serial.print("\n");
    }
    else
    {
        Serial.println("Sem Internet.");
        Serial.println();
    }
    readString=(char *) malloc(sizeof(char));
    char compareMe[9]="INSERIDO\0";
    bool returnMe=false;;
    while(ethClient->connected() && !ethClient->available()) delay(1);
    while (ethClient->connected() || ethClient->available())
    {
        char c = ethClient->read();
        size++;
        if(size<30)
        {
            readString=(char *) realloc(readString,size*sizeof(char));
            readString[size-1]='\0';
            readString[size-2]=c;

        }
        if(c==compareMe[0])
        {
            for(int i=1; i<strlen(compareMe); i++)
            {
                c=ethClient->read();
                if(c==compareMe[i])
                {
                    returnMe=true;
                    Serial.println("*****************************");
                    Serial.print("c->");
                    Serial.print(c);
                    Serial.print("comparado com ");
                    Serial.print(compareMe[i]);
                    Serial.print("\n");
                    continue;
                }
                else
                    returnMe=false;
                break;
            }
        }


    }


    ethClient->flush();
    ethClient->stop();
    Serial.println(readString);
    Serial.println("cliente desconectado.");
    /* Serial.println(strstr(readString,"200")!=NULL); */
    /* if(strstr(readString,"200")!=NULL){ */
    /* returnMe=true; */
    /* } */


    free(readString);
    return returnMe;
}
