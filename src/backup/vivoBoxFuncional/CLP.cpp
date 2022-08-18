#include "CLP.h"

int CLP::gNumE=0;
int CLP::pneumaticoAntes=0;
int CLP::eletrodosQueimados=0;
int CLP::counter=0;
unsigned long CLP::tFim=0;
unsigned long CLP::tInicio=0;

void CLP::Break(String texto){

    for(int i=0;i<100-texto.length();i++)
    {

      if(i==(int)(100-texto.length())/2){
        Serial.print(texto);
        continue;
      }
      Serial.print("-");
      
    }
    Serial.print("\n");

}

    bool CLP::especial(int valeAtual,int nValesEsp,int nVales){
        if((valeAtual>(nValesEsp-1)) || valeAtual<(nVales-nValesEsp)){
          return false;
        }
        else
          return true;
    }
   String CLP::comporTimeStamp(int dia,int mes,int ano,int hora,int minuto,int segundo){

  return (String(ano)+'-'+String(mes)+'-'+String(dia)+' '+String(hora)+':'+String(minuto)+':'+String(segundo));
} 


char *CLP::contar(ModbusTCPClient *modbusTCPClient,char *nomeCSV,int *passo,int numSerie) {
    char *retorno;
    IPAddress server(192, 168, 1, 3);
    int numE; //D6278
    int vale; //D6266
    int nValesEsp;//D6048
    int direcao;//M1053
    int simulacao;//M3021
    int operando;//M1000 -->1 Básico 0 Operando
    int pneumatico;//M1075
    int receita;
    bool eEspecial;//lógica interna
    int dia,ano,mes,hora,minuto,segundo;
    int verSeParou;
    
    int tVales;//D6042
    long minT;

    //Lógica
    //DO:O programa inicia e pega this.numE e coloca numa variável global.
    //DO:verificar se está em operacao e se o pneumático avançou
    //DO:se sim iniciar a contagem
    //DO:Verificar se o braço subiu
    //DO:Se sim parar contagem
    //Verificar se num numE mudou
    //Se sim esperamos a ativação do pneumático
    //Se sim começamos a contagem
    //Verificamos se o pneumático subiu
    //Se sim paramos a contagem.
    modbusTCPClient->begin(server,502);
    //Leitura das Memorias de interesse
    numE=modbusTCPClient->holdingRegisterRead(0x9886);
    vale=modbusTCPClient->holdingRegisterRead(0x987a);
    nValesEsp=modbusTCPClient->holdingRegisterRead(0x97a0);
    direcao=modbusTCPClient->coilRead(0x0c1d);//M1053
    simulacao=modbusTCPClient->coilRead(0xb5cd);//M3021
    operando=modbusTCPClient->coilRead(0xbe8);
    pneumatico=modbusTCPClient->coilRead(0x0c33);
    verSeParou=modbusTCPClient->holdingRegisterRead(0x0834);
    tVales=modbusTCPClient->holdingRegisterRead(0x979a);
    eEspecial = especial(vale,nValesEsp,tVales);//1digito
    dia=modbusTCPClient->holdingRegisterRead(0x11F4);//2digitos
    mes=modbusTCPClient->holdingRegisterRead(0x11F5);//2 digitos
    ano=modbusTCPClient->holdingRegisterRead(0x11F6);//4 digitos
    hora=modbusTCPClient->holdingRegisterRead(0x11F7);//2 digitos
    minuto=modbusTCPClient->holdingRegisterRead(0x11F8);//2 digitos
    segundo=modbusTCPClient->holdingRegisterRead(0x11F9);//2 digitos 
    receita=modbusTCPClient->holdingRegisterRead(0x11FA);//1digito
    
   
    if(dia==-1 || mes==-1){
        return NULL;
    }
    sprintf(nomeCSV,"L%02d-%02d.S",10,10);

 
    Serial.print("NumSerie-->");Serial.print(numSerie);Serial.print("\n");
    Serial.print("Eletrodos-->");Serial.print(eletrodosQueimados);Serial.print("\n");
    Serial.print("Pneumatico-->");Serial.print(pneumatico);Serial.print("\n");
    Serial.print("Vale:");Serial.print(vale);Serial.print("\n");
    Serial.print("Direcao-->");Serial.print(direcao);Serial.print("\n");
    Serial.print("Simulacao-->");Serial.print(simulacao);Serial.print("\n");
    Serial.print("Terno-->");Serial.print(receita);Serial.print("\n");
    Serial.print("Eespecial-->");Serial.print(eEspecial);Serial.print("\n");
    Serial.print("Data-->");Serial.print(dia);Serial.print(mes);;Serial.print("\n");
    Serial.print("tVales-->");Serial.print(vale);Serial.print("\n");
    Serial.print("T-Inicio-->");Serial.print(tInicio);Serial.print(mes);;Serial.print("\n");
    Serial.print("T-Fim-->");Serial.print(tFim);Serial.print(mes);;Serial.print("\n");
    char *testVar=(char *) malloc(60*sizeof(char));

    minT=17000;
    sprintf(testVar,"%04d;%02d;%02d;%02d;%02d;%02d;%ld;%02d;%d;%d;%d;%d;%d;%d;%d",ano,mes,dia,hora,minuto,segundo,minT,vale,direcao,eEspecial,receita,verSeParou,numSerie,simulacao);
    
    Serial.println(testVar);
    free(testVar);
    
    switch (*passo)
    {
    case -1:
      /*Primeiro ciclo*/
      gNumE=numE;
      *passo=1;
      break;

      case 1:
      if(operando==1){
        break;
      }else if(operando==0 && pneumatico==1){
      
        tInicio=millis();
        if(vale!=0){
          *passo=2;
          break;
        }
        else if(vale==0||vale==(tVales-1)){
          *passo=3;
          counter=0;
        }
      }
      break;
    case 2:
      if(pneumatico==0 && operando==0){
        tFim=millis();
        minT=(tFim-tInicio);
        *passo=1;
     
        
      eletrodosQueimados++;

    retorno=(char *) malloc(47*sizeof(char));

    sprintf(retorno,"%04d;%02d;%02d;%02d;%02d;%02d;%ld;%02d;%d;%d;%d;%d;%d;%d",ano,mes,dia,hora,minuto,segundo,minT,vale,direcao,eEspecial,receita,verSeParou,numSerie,simulacao);
    return retorno;
      }
      break;
    case 3:

        if(pneumatico==0 && operando==0 && counter==2){
        tFim=millis();
        minT=(tFim-tInicio); 
        *passo=1;  
        eletrodosQueimados++;
    
    retorno=(char *) malloc(47*sizeof(char));
    sprintf(retorno,"%04d-%02d-%02d;%02d:%02d:%02d;%d;%02d;%d;%d;%d;%d;%d;%d",ano,mes,dia,hora,minuto,segundo,minT,vale,direcao,eEspecial,receita,verSeParou,simulacao,numSerie); 
    
    return retorno;
      } if(pneumatico==0 && operando==0 && counter==0){
        counter++;
      } 
      if(pneumatico==1 && operando==0 && counter==1){
        counter++;
      }
    default:
      break;
    }
    modbusTCPClient->stop();
    return NULL;
  
}
