import os
import PySimpleGUI as sg

#trocar numero de serie
def trocar_num_serie(num_serie):
    if os.path.exists("dest.ino"):
        os.remove("./dest.ino")
    try:
        num_serie = int(num_serie)
    except:
        print("Número de série inválido")

    fonte = open('./src.ino', 'r')
    destino = open('./dest.ino', 'a')

    linha = 0

    for line in fonte.readlines():
        linha += 1
        if linha == 24 :
            destino.write('int numSerie = ' + str(num_serie)+ '\n'+';') 
        else:
            destino.write(line)
    fonte.close()
    destino.close()
    os.remove("./src.ino")
    os.rename('dest.ino', 'src.ino')

# Função para modificar o sketch.json
def formatar_sketch(porta):
    sketch = "{\n\"cpu\":\n {\n\"fqbn\": \"arduino:avr:mega\",\n\"name\": \"Arduino Mega or Mega 2560\",\n\"port\": \"serial://"+porta+"\"\n}\n}"
    print(sketch)
    f = open('sketch.json', 'w')
    f.write(sketch)


def encontrar_placas():
    stream = os.popen('arduino-cli board list')  
    #Tentamos encontrar uma placa no sistema
    #caso não dê certo retornamos -1
    #caso funcione retornamos o endereco da placa
    try:
        output = stream.read()
        indice_comeco = output.find('COM')
        indice_fim = output.find(' ', indice_comeco)

        porta = output[indice_comeco:indice_fim]
        print('Arduino encontrado na porta: ', porta)
        return porta



    except Exception as e:
         print(e)
         print('Nenhum arduino encontrado')
         return ""

def menu():
    while True:

        print('==============DUO AUTOMATION===============')
        print('\n')
        print('1 - Verificar Execução')
        print('2 - Passar o software')
        print('77 - Verificar bibliotecas')
        print('\n')
        print('Digite -1 para sair')

        
        opcao = input('Digite a opção: ')
        try:
            opcao = int(opcao)
        except:
            print('Opção inválida')
            opcao = 0
        if opcao == 77:
            _permitir = " Set-ExecutionPolicy RemoteSigned -Scope CurrentUser # Optional: Needed to run a remote script the first time"
            _instalar =  "irm get.scoop.sh | iex"
            _arduino_cli = "scoop install arduino-cli"
            _lib_1 = "arduino-cli lib install ArduinoModbusRS485"
            _lib_2 = "arduino-cli lib install ArduinoModbus"
            _lib_3 = "arduino-cli lib install SD"
            _lib_4 = "arduino-cli lib install Ethernet" 
            comandos = [_permitir, _instalar, _arduino_cli, _lib_1, _lib_2, _lib_3, _lib_4]
            
            for comando in comandos:
                os.system(comando)

        if opcao == 2:

            porta = encontrar_placas()

            num_serie = input('Digite o número de série: ')
            trocar_num_serie(num_serie)

            _compile = "arduino-cli compile --fqbn arduino:avr:mega"
            os.system(_compile)

            _upload = "arduino-cli upload -v --fqbn arduino:avr:mega -p " + porta
            os.system(_upload)

            
        elif opcao == 1:
            _run = "ComPrinter.exe /baudrate 9600"
            os.system(_run)

        elif opcao == -1:
            print("...")
            break
        



menu()

