import os
import PySimpleGUI as sg

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


porta = encontrar_placas()
formatar_sketch(porta)
layout = [[sg.Text("Por favor, insira o Arduino para começar")], [sg.Button("OK")]]
layout = [[sg.Text("Arduino Encontrado")], [sg.Button("OK")]]
window = sg.Window("PX", layout)
# Create an event loop
while True:
    event, values = window.read()
    
   
    if event == "OK" or event == sg.WIN_CLOSED:
        break

window.close()
