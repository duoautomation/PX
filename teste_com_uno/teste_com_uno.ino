#include <SoftwareSerial.h>
#include<WiFiClient.h>
#include<WiFi.h>
SoftwareSerial mega(2,3);
WiFiClient client;
const char *ssid = "StarDuo";
const char *password = "$Mc 7352";
int step = 0;
int tentativa = 0;
char *p;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mega.begin(115200);
    
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

}

char *ler_sel(SoftwareSerial *p_serial){
  int hBytes = p_serial->available();
  char *retorneMe;

  int stringSize = 1;
  retorneMe=(char *) malloc(sizeof(char));
  retorneMe[0]='\0';
  delay(100);
  
  if(p_serial->available() == 0 ){
    return NULL;
  }

  while(p_serial->available()>0){

    char caractere = p_serial->read();
    stringSize++;
    retorneMe=(char *)realloc(retorneMe,stringSize*sizeof(char));
    retorneMe[stringSize-1]='\0';
    retorneMe[stringSize-2]=caractere;

    delay(10);
  }

  return retorneMe;

}

char * ler_serial(SoftwareSerial *p_serial){
    delay(1000);
    int hBytes = p_serial->available();
    char *input;
    if(hBytes>50){
        input = (char *) malloc(hBytes*(sizeof(char)+1));
        memset(p,(int) '\0', sizeof(*p));
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

void loop() {
  Serial.print("Passo: ");
  Serial.println(step);
  int resultado = -1;

  
  switch(step){
    case 0:
            mega.print('1');
            step++;
            tentativa = 0;
            free(p);
    break;
    case 1:
            tentativa++;
            if(tentativa > 10){
              step = 0;
            }
            
            p = ler_sel(&mega);
            if(strlen(p)>50){
              tentativa = 0;
              step++;
            }
    break;
    case 2:
          Serial.println("Caso 2");
          Serial.print("Pronto para enviar:"); Serial.println(p);
          resultado = sendGET(p, &client);

          if(resultado == 1){
            step++;

          }
          if(tentativa == 10){
            step = 0;
          }
          tentativa++;
    break;
    case 3:
          mega.print('2');
          step = 0;

    break;
  }
}
