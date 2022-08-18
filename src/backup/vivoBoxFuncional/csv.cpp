
#include "csv.h"

int csv::posicaoMudar=0;

void csv::iSD(){
    digitalWrite(10,HIGH);
    digitalWrite(4,LOW);
}

void csv::fSD(){
    digitalWrite(10,LOW);
    digitalWrite(4,HIGH);
}

void csv::ls(char *diretorio) {
    iSD();
      File dir = SD.open(diretorio);
      Serial.println("----LS----");

  while (true) {

    File entry =  dir.openNextFile();

    if (! entry) {

      break;

    }



    Serial.print(entry.name());


    Serial.print("\t\t");

    Serial.println(entry.size(), DEC);

    entry.close();

  }
  dir.close();
  fSD();
}


void csv::escrever(char *nomeArquivo, char *conteudo){   
  iSD();
    Serial.println(nomeArquivo);
  File  myFile = SD.open(nomeArquivo, FILE_WRITE);
  if (myFile) {
    Serial.print("Escrevendo...");
    Serial.print(nomeArquivo);
    Serial.print("\n");
    myFile.println(conteudo);
    myFile.close();
    Serial.println("Escrito!");
  } else {
    Serial.println("Erro-->");Serial.print(nomeArquivo);Serial.print("<-\n");
    myFile.close();

  }
  fSD();
}

void csv::ler(File *file){
    while (file->available()) {
      Serial.write(file->read());
    }
    file->close();
    Serial.print("\n");
  }



char *csv::arquivoAguardando(char *diretorio,char *nomeArquivo) {
    iSD(); 
  File dir = SD.open(diretorio);
  while (dir) {
    Serial.println(">1");
 
    File entry =  dir.openNextFile();

    Serial.println(">2");

    if (! entry) {
    Serial.println(">3");
      break;
    }
    
    if(strcmp(entry.name(),nomeArquivo)==0){
        entry.close();
    Serial.println(">4");
        continue;

    }
    
    else if(entry.size()==0){
        entry.close();
    Serial.println(">5");
        continue;
    }

    else if(entry.isDirectory()){

        entry.close();
    Serial.println(">6");
        continue;
    }
    
    if(entry.name()[0]=='X'){
    Serial.println(">7");
        continue;
    }
    if(entry.name()[0]=='Y'){
    Serial.println(">8");
        continue;
    }

    if(entry.name()[0]=='L'){
        char *retorneMe=(char *) malloc(sizeof(entry.name()));
        strcpy(retorneMe,entry.name());
        entry.close();
        dir.close();
    Serial.println(">9");
        return retorneMe;
    }
    entry.close();
    Serial.println(">10");


  }
  dir.close();
    Serial.println(">11");
  fSD();

  return nomeArquivo;
}

//Tenta ler a linha N de um arquivo já aberto
//Caso não consiga retorna um ponteiro que aponta para NULL
char *csv::lerLinhaN(SDLib::File *file,int lineNumber){ 
    iSD();
    char *retorneMe;
    int n=0;
    char caractere;
    while (file->available()) { 
      caractere =(char) file->read(); 
      if(n==lineNumber){
        retorneMe=(char *) malloc(2*sizeof(char)); 
        retorneMe[0]=caractere;
        retorneMe[1]='\0';
        int stringSize=2;
        while((caractere=(char) file->read())!='\n')
        { 
            if((int)caractere==13){break;}
            stringSize++;     
            retorneMe=(char *)realloc(retorneMe,stringSize*sizeof(char));
            retorneMe[stringSize-1]='\0';
            retorneMe[stringSize-2]=caractere;
        
        } 
        fSD();
          return retorneMe;
      }
      if(caractere=='\n'){
        n++;
      } 
    }
    fSD();
    return NULL;
}

uint8_t csv::rm(String file){
    iSD();
  uint8_t n=0;
  if(file.equals("*")){
    //delete all files
    File dir = SD.open("/");
    while(dir){
      while(true){
        File entry = dir.openNextFile();
        SD.remove(String(entry.name()));
        n++;
        entry.close();
      }
    }
    dir.close();
    fSD();
    return n;
  }else{
    SD.remove(file);
    fSD();
    return 1;
  }
}

int csv::contarLinhas(File *myFile){
    iSD();
  int n=0; 
  if (myFile) { 
    char caractere;
    while (myFile->available()) { 
      caractere=myFile->read();
      if(caractere=='\n'){
        n++;
      }
    }
    fSD();
    return n;
  } else {
    Serial.println("Erro ao abrir o arquivo ");
    fSD();
    return -1;

  }
} 

//Copia o arquivo origem para o destino
//sobreescrevendo automaticamente o destino
//com uma cópia da origem, sem nenhuma confirmação.
void csv::copy(char *origem,char *destino){
    
  iSD();
    
  if(!SD.exists(origem)){
    Serial.println("ORG inexistente.");
    fSD();
    return;
  }
  File fOrigem = SD.open(origem,FILE_READ);
  File fDestino= SD.open(destino,FILE_WRITE);

  while(fOrigem.available()){
   uint8_t c = fOrigem.read();
   fDestino.write(c);
  }

  fOrigem.close();
  fDestino.close();
  fSD();
}


//Move o arquivo para o destino especificado
//Ou renomeia o arquivo se não houver subdiretório
void csv::move(char *origem,char *destino){
    if(strcmp(origem,destino)==0){
      return;
    }
    csv::copy(origem,destino);
    iSD();
    SD.remove(origem);
    fSD();
}






