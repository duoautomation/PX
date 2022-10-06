#include "CLP.h"

int CLP::pneumaticoAntes = 0;
int CLP::eletrodosQueimados = 0;
int CLP::counter = 0;
bool CLP::first = false;
unsigned long CLP::tFim = 0;
unsigned long CLP::tInicio = 0;

struct CLP::Dados
{
    int ano;
    int mes;
    int dia;
    int hora;
    int minuto;
    int segundo;
    int min_t;
    int vale;
    int direcao;
    int e_especial;
    int receita;
    int ver_se_parou;
    int num_serie;
    int simulacao;
    int n_vales_esp;
    int t_vales;
    int especial;
    int operando;
    int pneumatico;
    int num_e;
};

struct CLP::Dados *znap = NULL;

void CLP::Break(String texto)
{

    for(int i=0; i<100-texto.length(); i++)
    {

        if(i==(int)(100-texto.length())/2)
        {
            Serial.print(texto);
            continue;
        }
        Serial.print("-");

    }

    Serial.print("\n");

}

bool CLP::especial(struct CLP::Dados *dados)
{

    if((dados->vale>(dados->n_vales_esp-1)) || dados->vale<(dados->t_vales-dados->n_vales_esp))
    {
        return false;
    }
    else
        return true;

}

void CLP::printar_dados(CLP::Dados *dados)
{

    Serial.print("num_E: ");
    Serial.print(dados->num_e);
    Serial.print("\n");
    Serial.print("Vale: ");
    Serial.print(dados->vale);
    Serial.print("\n");
    Serial.print("Ano: ");
    Serial.print(dados->ano);
    Serial.print("\n");
    Serial.print("Mes: ");
    Serial.print(dados->mes);
    Serial.print("\n");
    Serial.print("Dia: ");
    Serial.print(dados->dia);
    Serial.print("\n");
    Serial.print("Hora: ");
    Serial.print(dados->hora);
    Serial.print("\n");
    Serial.print("Minuto: ");
    Serial.print(dados->minuto);
    Serial.print("\n");
    Serial.print("Segundo: ");
    Serial.print(dados->segundo);
    Serial.print("\n");
    Serial.print("MINT: ");
    Serial.print(dados->min_t);
    Serial.print("\n");
    Serial.print("Direcao: ");
    Serial.print(dados->direcao);
    Serial.print("\n");
    Serial.print("EEspecial: ");
    Serial.print(dados->e_especial);
    Serial.print("\n");
    Serial.print("Receita: ");
    Serial.print(dados->receita);
    Serial.print("\n");
    Serial.print("VSP: ");
    Serial.print(dados->ver_se_parou);
    Serial.print("\n");
    Serial.print("NumSerie: ");
    Serial.print(dados->num_serie);
    Serial.print("\n");
    Serial.print("Pneumatico: ");
    Serial.print(dados->pneumatico);
    Serial.print("\n");

}

void dados_cp(struct CLP::Dados* origem, struct CLP::Dados* destino){
    
    destino->vale = origem->vale;
    destino->n_vales_esp = origem->n_vales_esp; 
    destino->t_vales = origem->t_vales;

    destino->ver_se_parou = origem->ver_se_parou;
    destino->direcao = origem->direcao;
    destino->simulacao = origem->simulacao;
    destino->operando = origem->operando;
    destino->pneumatico = origem->pneumatico;

    destino->e_especial = origem->e_especial;
    destino->dia = origem->dia;
    destino->mes = origem->mes;
    destino->ano = origem->ano;
    destino->hora = origem->hora;
    destino->minuto = origem->minuto;
    destino->segundo = origem->segundo;
    destino->receita = origem->receita;
    destino->num_e = origem->num_e;

}

void CLP::ler_clp(struct Dados *dados, ModbusTCPClient *modbusTCPClient)
{

    //Leitura das Memorias de interesse
    dados->vale = modbusTCPClient->holdingRegisterRead(0x987a);//D6266
    dados->n_vales_esp = modbusTCPClient->holdingRegisterRead(0x97a0);//D6048
    dados->t_vales = modbusTCPClient->holdingRegisterRead(0x979a);//D6042
    dados->ver_se_parou = modbusTCPClient->coilRead(0x0834);
    dados->direcao = modbusTCPClient->coilRead(0x0c1d);//M1053
    dados->simulacao = modbusTCPClient->coilRead(0xb5cd);//M3021
    dados->operando = modbusTCPClient->coilRead(0xbe8);
    dados->pneumatico = modbusTCPClient->coilRead(0x0c33);

    dados->e_especial = especial(dados);//1digito
    dados->dia = modbusTCPClient->holdingRegisterRead(0x11F4);//2digitos
    dados->mes = modbusTCPClient->holdingRegisterRead(0x11F5);//2 digitos
    dados->ano = modbusTCPClient->holdingRegisterRead(0x11F6);//4 digitos
    dados->hora = modbusTCPClient->holdingRegisterRead(0x11F7);//2 digitos
    dados->minuto = modbusTCPClient->holdingRegisterRead(0x11F8);//2 digitos
    dados->segundo = modbusTCPClient->holdingRegisterRead(0x11F9);//2 digitos
    dados->receita = modbusTCPClient->holdingRegisterRead(0x11FA);//1digito
    //dados->num_e = modbusTCPClient->holdingRegisterRead(0x11FC);
    dados->num_e = modbusTCPClient->holdingRegisterRead(0x9772); //----------------> Descomentar essa linha e comentar a superior.

    if(first==false){
        znap = malloc(sizeof(struct Dados));
        dados_cp(dados, znap);
        first = true;
        Serial.println("CLP inicializado!");
        delay(5000);
    }

}

char *CLP::formatar_dados(struct Dados *dados)
{

    char *retorneMe = (char *) malloc(52*sizeof(char));
    sprintf(retorneMe,"%04d;%02d;%02d;%02d;%02d;%02d;%d;%02d;%d;%d;%d;%d;%d;%d",dados->ano,dados->mes,dados->dia,dados->hora,dados->minuto,dados->segundo,dados->min_t,dados->vale,dados->direcao,dados->especial,dados->receita,dados->ver_se_parou,dados->num_serie,dados->simulacao);
    Serial.println(retorneMe);

    return retorneMe;
}

int comparar_dados(struct CLP::Dados *velho, struct CLP::Dados *novo,int *passo)
{
    //Retornamos -2 quando não temos certeza se o CLP está conectado. --> Implementado
    //Retornamos -1 se está em basico
    //Retornamos 0 se nada ocorreu ---> Implementado
    //Retornamos 1 se houve queima de eletrodo. --> Implementado

    //Vemos se estamos conectados
    if(novo->operando==-1) {
        Serial.println("1");
        return -2;
    }

    if(novo->operando == 1){
            CLP::tInicio == 0;
            return -1;
    }


    if(novo->num_e != velho->num_e && velho->pneumatico == 1){
            return 1;
    }

    if(novo->num_e == velho->num_e && novo->pneumatico == 1){
            return 0;
    }


}


char *CLP::contar(ModbusTCPClient *modbusTCPClient,char *nomeCSV,int *passo,int numSerie)
{

    struct CLP::Dados dados;

    IPAddress server(192, 168, 1, 3);

    modbusTCPClient->begin(server,502);
 
    ler_clp(&dados,modbusTCPClient);

    modbusTCPClient->stop();

    int resultado = comparar_dados(znap, &dados, passo);
    dados_cp(&dados, znap);
    printar_dados(znap);
    

    if(resultado == -2 || resultado == -1){
        return NULL;
    }

    else if (resultado == 1){
            dados.min_t = tFim/1000;
            dados.num_serie = numSerie;
            Serial.println("Novo Eletrodo");
            Serial.println(formatar_dados(&dados));
            return formatar_dados(&dados);
    }

    else if(resultado == 0){

            if(CLP::tInicio == 0){
                    CLP::tInicio =millis();
            }

        CLP::tFim += millis() - CLP::tInicio;
        CLP::tInicio=millis();
        return NULL;
    }

    return NULL; //------------------------------------------------------> Remova me!

}

