#include "CLP.h"

int CLP::pneumaticoAntes=0;
int CLP::eletrodosQueimados=0;
int CLP::counter=0;
unsigned long CLP::tFim=0;
unsigned long CLP::tInicio=0;

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
};

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

void CLP::ler_clp(struct Dados *dados, ModbusTCPClient *modbusTCPClient)
{

    //Leitura das Memorias de interesse
    dados->vale=modbusTCPClient->holdingRegisterRead(0x987a);//D6266
    dados->n_vales_esp=modbusTCPClient->holdingRegisterRead(0x97a0);//D6048
    dados->t_vales=modbusTCPClient->holdingRegisterRead(0x979a);//D6042

    dados->ver_se_parou=modbusTCPClient->coilRead(0x0834);
    dados->direcao=modbusTCPClient->coilRead(0x0c1d);//M1053
    dados->simulacao=modbusTCPClient->coilRead(0xb5cd);//M3021
    dados->operando=modbusTCPClient->coilRead(0xbe8);
    dados->pneumatico=modbusTCPClient->coilRead(0x0c33);

    dados->e_especial= especial(dados);//1digito
    dados->dia=modbusTCPClient->holdingRegisterRead(0x11F4);//2digitos
    dados->mes=modbusTCPClient->holdingRegisterRead(0x11F5);//2 digitos
    dados->ano=modbusTCPClient->holdingRegisterRead(0x11F6);//4 digitos
    dados->hora=modbusTCPClient->holdingRegisterRead(0x11F7);//2 digitos
    dados->minuto=modbusTCPClient->holdingRegisterRead(0x11F8);//2 digitos
    dados->segundo=modbusTCPClient->holdingRegisterRead(0x11F9);//2 digitos
    dados->receita=modbusTCPClient->holdingRegisterRead(0x11FA);//1digito

}

char *CLP::formatar_dados(struct Dados *dados)
{

    char *retorneMe = (char *) malloc(sizeof(struct Dados));
    sprintf(retorneMe,"%04d;%02d;%02d;%02d;%02d;%02d;%d;%02d;%d;%d;%d;%d;%d;%d",dados->ano,dados->mes,dados->dia,dados->hora,dados->minuto,dados->segundo,dados->min_t,dados->vale,dados->direcao,dados->especial,dados->receita,dados->ver_se_parou,dados->num_serie,dados->simulacao);
    Serial.println(retorneMe);

    return retorneMe;
}

int compararDados(struct CLP::Dados *velho, struct CLP::Dados *novo)
{

    //Usuário parou a máquina mas não trocou de eletrodo
    if(velho->operando==0 && novo->operando==1)
    {
        return 0;
        //pausar
    }

    //Houve troca de eletrodo entre um snap e outro
    if(velho->vale!=novo->vale)
    {
        return 2;
    }

}

char *CLP::contar(ModbusTCPClient *modbusTCPClient,char *nomeCSV,int *passo,int numSerie, struct CLP::Dados *snap)
{

    struct CLP::Dados dados;

    dados.num_serie = numSerie;
    Serial.print("----->");
    Serial.print(dados.num_serie);
    Serial.print("\n");

    IPAddress server(192, 168, 1, 3);

    modbusTCPClient->begin(server,502);
    ler_clp(&dados,modbusTCPClient);
    modbusTCPClient->stop();

    printar_dados(&dados);

    //Vemos se estamos em basico
    //Se sim ignoramos
    if(dados.operando==1)
    {
        return NULL;
    }

    //Vemos se estamos no ultimo
    //ou primeiro vale
    if((dados.vale==0 || dados.vale==dados.t_vales) && dados.pneumatico==1)
    {
        tInicio=millis();
        *passo=3;
    }

    Serial.println("==================");
    Serial.println(dados.pneumatico);
    Serial.println("==================");

    switch(*passo)
    {

    case -1:
        /*Primeiro Ciclo*/
        *passo=1;
        break;

    case 1:
        Serial.println("-->Passo 1");
        Serial.println("==================");
        Serial.println(dados.pneumatico);
        Serial.println("==================");

        if(dados.pneumatico==1)
        {
            Serial.println("Proximo passo...");
            *passo=2;
            tInicio=millis();
        }

        break;

    case 2:
        Serial.println("-->Passo 2");

        if(dados.pneumatico==0)
        {
            tFim=millis()-tInicio;
            dados.min_t = tFim/1000;
            *passo=1;
            return formatar_dados(&dados);
        }

        break;

    case 3:
        Serial.println("-->Passo 3");
        if(dados.vale!=0 && dados.vale!=dados.t_vales)
        {
            tFim=millis()-tInicio;
            dados.min_t = tFim/1000;
            *passo=1;
            return formatar_dados(&dados);
        }

        break;

    }
    return NULL;
}

