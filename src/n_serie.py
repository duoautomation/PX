import os


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
            destino.write('int numSerie = ' + str(num_serie)+ '\n') 
        else:
            destino.write(line)
    fonte.close()
    destino.close()
    os.remove("./src.ino")
    os.rename('dest.ino', 'src.ino')

num_serie = input('Digite o número de série: ')

trocar_num_serie(num_serie)
