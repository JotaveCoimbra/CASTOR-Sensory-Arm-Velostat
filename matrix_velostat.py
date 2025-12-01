import serial
import numpy as np
import matplotlib.pyplot as plt
import re

# CONFIGURAÇÃO DA SERIAL
ser = serial.Serial('COM5', 115200)  # coloque sua porta aqui

plt.ion()
fig, ax = plt.subplots()
img = ax.imshow(np.zeros((6,6)), cmap='inferno', vmin=0, vmax=4095)
plt.colorbar(img)

def ler_matriz():
    matriz = []
    linha_atual = []

    while True:
        raw = ser.readline().decode(errors="ignore").strip()

        if "----- MATRIZ -----" in raw:
            matriz = []
            continue

        if "------------------" in raw:
            if len(matriz) == 6:
                return np.array(matriz)

        # detecta linha em formato: num num num ...
        if re.match(r'^\d+', raw):
            valores = list(map(int, raw.split()))
            if len(valores) == 6:
                matriz.append(valores)

while True:
    M = ler_matriz()
    img.set_data(M)
    plt.draw()
    plt.pause(0.01)
