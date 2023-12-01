#include "cloud.h"


String cloud::formatarJSON(String data)
{
    String retorneMe="{\"raw\":\""+data+"\"}";
    return retorneMe;
}

String cloud::linhaAguardando="";
static char serverName[]="adm.duo.com.br";

boolean cloud::verificar_arquivos(){
    int count = 0;
    if(!SD.exists(cloud::nome_ponteiro))
    {
        csv::escrever(cloud::nome_ponteiro, "X");
        Serial.println("Criando arquivo de registro...");
    }

    if(!SD.exists(cloud::arquivo_enviando)){
        Serial.println("Criando arquivo de armazenamento...");
        csv::escrever(cloud::arquivo_enviando, "2023;05;09;15;25;2;25;16;0;-1;1;0;120;1");
    }

    if(!SD.exists("UA.S")){

         csv::escrever("UA.S", "2023;04;29;15;00;00");
         Serial.println("Criado."); 
    }

    if(SD.exists("UA.S") && SD.exists(cloud::nome_ponteiro) && SD.exists(cloud::arquivo_enviando)){
        return true;
    }else{
        return false;
    }
}

void limpar_buffer(){
    while(Serial2.available()>0){
        Serial2.read();
    }
}

void enviar_serial(char *json, char *nomePonteiro){
              //Codigo Novo
            //Vemos quantos caracteres precisam ser tratados no buffer
            //Leitura. Todas as leitura são códigos
            Serial.println("Envio serialllll");
            int number_char_read = Serial2.available();
            byte code = 0;
            
            if(number_char_read > 0 ){
                Serial.print("Reading:");Serial.print(number_char_read);Serial.print('\n');
                code = Serial2.read()-48; 
            }

            if(code>10){
                Serial.println("Limpando o buffer");
                limpar_buffer();
                delay(5000);
                Serial.println("Buffer vazio");
                code = 0;
            }

            Serial.print("--CODE:");Serial.print(code);Serial.print('\n');
            Serial.print("--ACODE:");Serial.print((char) (code + 48));Serial.print('\n');

            //TODO: Tratar o código.
            switch(code){
                case 1: 
                Serial.println("Mandando novamente.");
                Serial.print("Json enviado "); Serial.println(json);
                Serial2.print(json);
                code = 0;
                //Mandar novamente a linha atual
                break;
                case 2:
                //Ticar a linha 
                Serial.println("Ticando a linha.");
                //Serial.println("Enviado");
                csv::escrever(nomePonteiro,"P");
                cloud::linhasPonteiro++;
                //cloud::linhasPonteiro++; 
                //Mandar a próxima linha
                break;
                case 0:
                Serial.println("Nada para enviar.");
                //Não fazer nada
                break;
                default:
                break;
            }
}
void cloud::logicaNuvem(char *arquivoEnviando,EthernetClient *ethClient,bool *precisaOutroArquivo,char *nomeCSV,int numSerie)
{
    bool arquivos = cloud::verificar_arquivos();

    if(!arquivos){
        Serial.println("Problema no cartão SD");
    }
    //Serial.println("Procurando dados para enviar para a DUO...");
    char *nomePonteiro=(char *) malloc(13*sizeof(char));
    snprintf(nomePonteiro,13*sizeof(char),"X%s",arquivoEnviando);
    /* Serial.print("Arquivo enviando "); */
    /* Serial.print(arquivoEnviando); */
    /* Serial.print("\n"); */
    /* Serial.print("Arquivo ponteiro "); */
    /* Serial.print(nomePonteiro); */
    /* Serial.print("\n"); */

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


    if(cloud::linhasCSV == cloud::linhasPonteiro){
        cloud::first = false;
    }
    if(cloud::first == false){
        
    file=SD.open(arquivoEnviando,FILE_READ);
    cloud::linhasCSV = csv::contarLinhas(&file);
    file.close();

    file=SD.open(nomePonteiro,FILE_READ);
    cloud::linhasPonteiro = csv::contarLinhas(&file);
    file.close();
    
    cloud::first = true;

    /*         //Fazemos um POST simples só para informar ao servidor que está conectado */
    /*         int size=24; */
    /*         char *json=(char *) malloc(size*sizeof(char)); */
    /*         snprintf(json,size*sizeof(char),"GET /test?raw=%d&SD=%d",numSerie,linhasCSV); */
    /*         Serial.println(numSerie); */
    /*         Serial.println("Atualizando a hora"); */
    /*         sendGET(json,ethClient); */
    /*         free(json); */
    /*         cloud::first = true; */
    /*         return; */
    }

    Serial.print("Linhas_P---------->");
    Serial.print(cloud::linhasPonteiro);
    Serial.print("\n");
    Serial.print("Linhas_CSV---------->");
    Serial.print(cloud::linhasCSV);
    Serial.print("\n");

    if(linhasCSV >= 0){
        cloud::sd_ok = true;
    }else{
        cloud::sd_ok = false;
    }

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
                size=size+20;
                char *json =(char *) malloc(size*sizeof(char));
                /* snprintf(json,size*sizeof(char),"{\"raw\":\"%s\"}",linha); */
                snprintf(json,size*sizeof(char),"GET /test?raw=%s",linha);

                Serial.println("************************************************");
                Serial.println(json);
                Serial.println("************************************************");
                Serial.print("Ponteiro---->");
                Serial.print(nomePonteiro);
                Serial.print("\n");

                enviar_serial(json, nomePonteiro);
                //Fim do Código novo
                //bool result = sendGET(json, ethClient);
                free(json);


            }
            free(linha);
        }
        else
        {
            Serial.println("Sem dados para enviar.");
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


boolean cloud::sendGET(char *data, EthernetClient *client){

    int timer;
    char compareMe[9]="INSERIDO\0";
    int match = 0;
    char servidor[]="23.239.10.90";

    if (client->connect(servidor,8080))
    { 
        client->setTimeout(10000);
        Serial.println("Conectado");
        client->println(data);
        Serial.println(data);
        client->println("Host: adm.duo.com.br");
        Serial.println("Host: adm.duo.com.br"); 
        client->println("Connection: close");
        Serial.println("Connection: close");
         
        client->println();
        Serial.println("Enviado, aguardando resposta.");
    }else{
            return false;
    }

        timer = 0;

        while(!client->available()){
                Serial.print(".");
                delay(100);
                timer++;
                if(timer == 50){
                        return false;
                }
        }
        
        /* Serial.print("\n"); */
        

        while(client->available() && client->connected() && match<8){

                char c = client->read(); 
                Serial.print(c); 
                
                if(compareMe[match] == c){
                        match++;
                }else{
                        match = 0;
                }
        }

        client->flush();
        client->stop();
        
        Serial.println("MATCH");
        /* Serial.println(match); */
        if(match == 8){
                cloud::internet_ok = true;
                return true;
        }else{
                return false;
        }
}

boolean cloud::sendPOST(char *data,EthernetClient *ethClient)
{
    ethClient->setTimeout(15000);
    Serial.print("Memoria------>");
    Serial.print(FreeRam());
    Serial.print("\n");
    char serverName[]="23.239.10.90";
    char *readString;
    int size=1;

    if (ethClient->connect(serverName,8080))
    {
        Serial.println("connected");
        ethClient->println("POST/ HTTP/1.1");
        ethClient->println("Host: 23.239.10.90");
        ethClient->println("Content-Type: application/json");
        ethClient->print("Content-Length:"); 
        ethClient->print(strlen(data));
        ethClient->print("\n");
        ethClient->println("");
        ethClient->println(data);
        ethClient->println("Connection: close");
        ethClient->println();
    }
    else
    {
        Serial.println("Sem Internet.");
        Serial.println();
        return false;
    }

    readString=(char *) malloc(sizeof(char));
    char compareMe[9]="INSERIDO\0";
    bool returnMe=false;;
    while(ethClient->connected() && !ethClient->available())
    {
            delay(1);
    }

    while (ethClient->connected() && ethClient->available())
    {
        Serial.println("Conectado Aguardado dados...");

        char c = ethClient->read();
        size++;
        if(size<300)
        {
            readString=(char *) realloc(readString,size*sizeof(char));
            Serial.println(c);
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


    free(readString);
    return returnMe;
}
