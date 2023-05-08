#include "cloud.h"

int cloud::numero_falhas = 0;

int cloud::tamanho_pacote(int linhas_csv, int linhas_ponteiro){

   // Se o número de falhas for maior que 5 retornamos -1
   if(cloud::numero_falhas > 5){
    return -1;
   }

   int falta_enviar = linhas_csv - linhas_ponteiro; 


   // Se o pacote é muito pequeno, 
   // Tentamos 2 vezes
   if(falta_enviar < 10 && numero_falhas > 2){
    return -1;
   }

   // Se o número de falhas for menor que 3, mandamos o máximo número de linhas possível
   // com um máximo de 100
   if(cloud::numero_falhas < 3 ){
     if(falta_enviar < 100){
        return falta_enviar; 
     }else{
        return 100;
     }
   }

   // Se o número de falhas for maior que 3
   // Limitamos a 10
    return 10;
}

String cloud::formatarJSON(String data)
{
    String retorneMe="{\"raw\":\""+data+"\"}";
    return retorneMe;
}

String cloud::formatarListaJSON(String data)
{
    String retorneMe="{\"raw\":\""+data+"\"}";
    return retorneMe;
}

String cloud::linhaAguardando="";
static char serverName[]="adm.duo.com.br";

bool cloud::verificar_arquivos(){
    int count = 0;
    File file;
    if(!SD.exists(cloud::nome_ponteiro))
    {
        Serial.println("Criando arquivo de registro...");
        file= SD.open(cloud::nome_ponteiro,FILE_WRITE);
        if(file)
        {
            file.close();
        }
        file.close();
    }

    if(!SD.exists(cloud::arquivo_enviando)){
        Serial.println("Criando arquivo de armazenamento...");
        file = SD.open(cloud::arquivo_enviando, FILE_WRITE);
        if(file){
            file.close();
        }

        file.close();
    }

    if(!SD.exists("UA.S")){

         csv::escrever("UA.S", "2023;04;29;15;00;00");
         Serial.println("Criado.");
        file.close();
    }

    if(SD.exists("UA.S") && SD.exists(cloud::nome_ponteiro) && SD.exists(cloud::arquivo_enviando)){
        return true;
    }else{
        return false;
    }
}

struct tm cloud::ler_ua(){
    File file;
    file = SD.open("UA.S", FILE_READ); 
    char *dado_cru = csv::lerLinhaN(&file, 0);
    file.close();


    if(strlen(dado_cru)<10){
        csv::escrever("UA.S", "2023;04;29;15;00;00");
        file = SD.open("UA.S", FILE_READ); 
        csv::ler(&file);
        Serial.println("Criado.");
        return;
     }

     char *tratamento = strtok(dado_cru, ";");
     int parsed[7];
     int i = 0;

    while (tratamento !=NULL){
        parsed[i] = atoi(tratamento);
        tratamento = strtok(NULL, ";");
        i++;
     }

     struct tm retorne_me = {0};


    retorne_me.tm_year = parsed[0]-1900;
    retorne_me.tm_mon = parsed[1]-1;
    retorne_me.tm_mday = parsed[2];

    retorne_me.tm_hour = parsed[3];
    retorne_me.tm_min = parsed[4];
    retorne_me.tm_sec = parsed[5];

    return retorne_me;
}

bool cloud::esta_na_hora(){ 
    time_t t1 = mktime(&cloud::ua);
    time_t t2 = mktime(&CLP::ua);

    double tempo_passado = difftime(t2, t1);
    Serial.print("==========> tp: "); Serial.print(tempo_passado);Serial.print("\n");
    Serial.print("==========> intervalo: "); Serial.print(cloud::intervalo_segundos);Serial.print("\n");

    if(tempo_passado >= cloud::intervalo_segundos){
        return true;
    }

    return false;
}

//Lê até 100 linhas e retorna um JSON array
char *cloud::preparar_pacote(int inicio, int t_linhas){
    int limit = 10;
    int linha_atual = 0;
    int linhas_lidas = 0;

    char *retorne_me = (char*)malloc(2*sizeof(char));
    retorne_me[0] = '[';
    retorne_me[1] = '\0';

    bool tem_aspa = false;

    char atual;
    int t_json = 2;

    File myFile = SD.open(cloud::arquivo_enviando, FILE_READ);
    if (myFile)
    {
        char caractere;
        while (myFile.available())
        {
            caractere = myFile.read();
            if(caractere=='\n')
            {
                linha_atual++;
                continue;
            }

            if(linha_atual >= inicio){
                if(t_json==2){
                    t_json++;
                    retorne_me =(char *)realloc(retorne_me,t_json*sizeof(char));
                    retorne_me[t_json-1]='\0';
                    retorne_me[t_json-2]='"';
                }

             if(tem_aspa == true){
                     t_json++;
                    retorne_me =(char *)realloc(retorne_me,t_json*sizeof(char));
                    retorne_me[t_json-1]='\0';
                    retorne_me[t_json-2]='"';
                    tem_aspa = false;
                }

                 if((int)caractere==13 || caractere=='\n')
                {
                    caractere = ',';
                    linhas_lidas++;
                    if(linhas_lidas == limit || linha_atual == t_linhas-1){
                        break;
                    }

                    t_json++;
                    retorne_me =(char *)realloc(retorne_me,t_json*sizeof(char));
                    retorne_me[t_json-1]='\0';
                    retorne_me[t_json-2]='"';

                    tem_aspa = true;
                }

               
                t_json++;
                retorne_me =(char *)realloc(retorne_me,t_json*sizeof(char));
                retorne_me[t_json-1]='\0';
                retorne_me[t_json-2]=caractere;

            }
        }

        myFile.close();

    }
        cloud::linhas_enviando = linhas_lidas;
        t_json = t_json + 2;

        retorne_me =(char *)realloc(retorne_me,t_json*sizeof(char)); 
        retorne_me[t_json-1]='\0';
        retorne_me[t_json-2]=']';
        retorne_me[t_json-3]='"';

    return retorne_me;
}


bool ticar_linhas(int num_linhas){
    return false;
}

void cloud::atualizar(){
    //Determinar o tamanho atual da fila
    int db = csv::contarLinhas(cloud::arquivo_enviando);
    int ponteiro = csv::contarLinhas(cloud::nome_ponteiro);

    if(cloud::pacote_fechado==0){
        cloud::pacote_fechado = tamanho_pacote(db, ponteiro); 
    }

    Serial.print("Tamanho do pacote a enviar: ");Serial.print(cloud::pacote_fechado);Serial.print("\n");
    if(cloud::pacote_fechado== 0){
        // Atualizar o arquivo UA.S com o timestamp atual não tem nada para fazer
        SD.remove("UA.S");
        File file = SD.open("UA.S", FILE_WRITE);
        char t_atual[22];
        const *n_ua = strftime(t_atual, 22,"%Y;%m;%d;%H;%M;%S\n", &CLP::ua);
        csv::escrever("UA.S",*n_ua);
        cloud::ua = CLP::ua;
        file.close();
        return;
    }

    Serial.println("Preparando para atualizar...");
    char *json = cloud::preparar_pacote(ponteiro, db); 
    bool enviou = sendPOST(json,cloud::ethClient);

    if(enviou){
        Serial.print("Linhas enviando-->");Serial.print(cloud::linhas_enviando);Serial.print("\n");
        while(cloud::linhas_enviando != 0){
            csv::escrever(cloud::nome_ponteiro, "X");
            cloud::linhas_enviando--;
        }
        cloud::pacote_fechado = 0;
    }

    free(json);
    Serial.print("Dados: ");Serial.print(db);Serial.print("\n");
    Serial.print("Registro: ");Serial.print(ponteiro);Serial.print("\n");

    //preparar o pacote com as linhas a enviar
    //converter o pacote para json - gravar em variavel global
}

void cloud::logicaNuvem(EthernetClient *ethClient,int numSerie){
    if(!cloud::arquivos_ok){
        cloud::arquivos_ok = cloud::verificar_arquivos();
        Serial.print("Status arquivos: ");
        Serial.print(cloud::arquivos_ok);
        Serial.println("\n");

        return;
    }

    if(cloud::ua.tm_year == 0){
        cloud::ua = cloud::ler_ua();
        Serial.println("Lendo a ultima atualizacao."); 
        Serial.println("Vou retornar!");
        return;
    }

    if(cloud::esta_na_hora()){
        cloud::atualizar();
        return;
    }else{
        Serial.println("Nao esta na hora de atualizar");
        return;
    }
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
    ethClient->setTimeout(15000);
    Serial.print("Memoria------>");
    Serial.print(FreeRam());
    Serial.print("\n");
    char serverName[]="23.239.10.90";
    char *readString;
    int size=1;

    if (ethClient->connect(serverName,8080))
    {
        Serial.println("Conectado !!");
        ethClient->println("POST /test HTTP/1.1");
        ethClient->println("Host: 23.239.10.90");
        ethClient->println("Content-Type: application/json");
        ethClient->print("Content-Length: "); 
        ethClient->println(strlen(data)+2);
        ethClient->println();
        ethClient->println(data);
        ethClient->println();
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

    Serial.println("Conectado Aguardado dados...");
    while (ethClient->connected() && ethClient->available())
    {

        char c = ethClient->read();
        size++;
        if(size<300)
        {
            readString=(char *) realloc(readString,size*sizeof(char));
            //Serial.println(c);
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