import serial
import numpy as np
import matplotlib.pyplot as plt
import re
import time
import msvcrt   # <-- necessário no Windows

# CONFIGURAÇÃO DA SERIAL
ser = serial.Serial('COM3', 115200)

plt.ion()
fig, ax = plt.subplots()

img = ax.imshow(np.zeros((6,6)), cmap='inferno', vmin=0, vmax=2000)
plt.colorbar(img)

# -----------------------------------------------------------
# FUNÇÃO PARA LER MATRIZ ENVIADA PELO ESP32
# -----------------------------------------------------------
def ler_matriz():
    matriz = []
    while True:
        raw = ser.readline().decode(errors="ignore").strip()

        if "----- MATRIZ -----" in raw:
            matriz = []
            continue

        if "------------------" in raw:
            if len(matriz) == 6:
                return np.array(matriz)

        if re.match(r'^\d+', raw):
            valores = list(map(int, raw.split()))
            if len(valores) == 6:
                matriz.append(valores)

# -----------------------------------------------------------
# CAPTURA BASELINE AUTOMÁTICO
# -----------------------------------------------------------
print("\n>>> Aguarde, calibrando baseline...")
time.sleep(1)

baseline = ler_matriz().astype(float)

print("\n>>> Baseline capturado!")
print(baseline)
print("\nPressione ENTER a qualquer momento para recalibrar.\n")

# -----------------------------------------------------------
# LOOP PRINCIPAL
# -----------------------------------------------------------
while True:

    # 🔥 DETECTA ENTER NO WINDOWS (sem travar o loop)
    if msvcrt.kbhit():           # se tem tecla pressionada
        key = msvcrt.getch()
        if key == b'\r':         # ENTER
            print("\n>>> Recalibrando baseline...\n")
            baseline = ler_matriz().astype(float)
            print(">>> Novo baseline:")
            print(baseline)

    # lê matriz atual
    M = ler_matriz().astype(float)

    # subtrai baseline
    delta = M - baseline

    # evita negativo
    delta = np.clip(delta, 0, None)

    # imprime matriz delta no terminal
    print("\n===== MATRIZ DELTA =====")
    print(delta.astype(int))
    print("========================\n")

    # atualiza heatmap
    img.set_clim(0, 1500)
    img.set_data(delta)

    plt.draw()
    plt.pause(0.01)
