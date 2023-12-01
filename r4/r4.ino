#include<SoftwareSerial.h>
#include<WiFi.h>
#include<HttpClient.h>
#include<WiFiClient.h>
const char *ssid = "StarDuo";
const char *password = "$Mc 7352";

WiFiClient client;

int rxPin = 2;
int txPin = 3;
int step = 0;

int tentativa =0;

char nome_servidor[]="23.239.10.90";
char *pacote;


SoftwareSerial mega(rxPin, txPin);

void setup(){
    Serial.begin(115200);
    mega.begin(115200);

    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

//Código para a leitura do serial
//Aloca memória dinamicamente para guardar o input
//A memória precisa ser desalocada
char * ler_serial(SoftwareSerial *p_serial){
    int hBytes = p_serial->available();
    char *input;
    if(hBytes>0){
        input = (char *) malloc(hBytes*sizeof(char));
        Serial.print("Bytes to read: ");Serial.print(hBytes);Serial.print("\n");
        p_serial->readBytes(input, hBytes);
        Serial.println(input);
    return (input);
    }else{
        return NULL;
    }
}

int sendGET(char *data, WiFiClient *client){

    int timer;
    char compareMe[]="INSERIDO\0";
    int match = 0;
    char servidor[]="23.239.10.90";

    if (client->connect(servidor,8080))
    { 
        client->setTimeout(10000);
        Serial.println("Conectado");
        client->print("GET /test?raw=");
        client->println(data);
        Serial.println(data);
        client->println("Host: adm.duo.com.br");
        Serial.println("Host: adm.duo.com.br"); 
        client->println("Connection: close");
        Serial.println("Connection: close"); 
        client->println();
        Serial.println("Enviado, aguardando resposta.");
    }else{
            return 1;
    }

        timer = 0;

        while(!client->available()){
                Serial.print(".");
                delay(100);
                timer++;
                if(timer == 50){
                        return 0;
                }
        }
        
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
        if(match == 8){
                return 1;
        }else{
                return 0;
        }
}

void logica() {
    int resultado = -1;
    int hBytes = Serial3.available();
    Serial.print("Bytes to read: ");Serial.print(hBytes);Serial.print("\n");

    switch(step){
        case 0:
            tentativa = 0;
            Serial.println("Caso 0");
            mega.write('1');
            step++;
            delay(5000);
        break;
        case 1:
            Serial.println("Caso 1");
            tentativa++;
            Serial.print("Disponível: ");
            Serial.println(mega.available());
            if(tentativa > 10){
                step = 0;
            }
            if(mega.available()>0){
                pacote = ler_serial(&mega);
                step++;
            }else{
                delay(1000);
            }
            
        break;
        case 2:
            Serial.println("Caso 3");
            Serial.println("Fazendo POST");
            resultado = sendGET(pacote, &client);
            Serial.print("Resultado:");
            Serial.println(resultado);
            if(resultado != 0){
                step = 0;
            }
        break;
        case 3:
            mega.write('2');
        break;
    }
}

void loop(){

    int resultado = -1;
    int hBytes = mega.available();
    Serial.print("Bytes to read: ");Serial.print(hBytes);Serial.print("\n");
    mega.write('17');
    delay(1000);

    
   
   //logica();
}
