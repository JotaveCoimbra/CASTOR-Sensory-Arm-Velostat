import re
import time
import numpy as np
import serial
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

PORTA = "COM7"      # <- TROQUE
BAUD = 115200

VMAX1 = 1200        # máximo Matriz 1
VMAX2 = 2000      # máximo Matriz 2 (ajuste pro seu caso)

def extrair_6_ints(texto):
    nums = re.findall(r"-?\d+", texto)
    if len(nums) < 6:
        return None
    return list(map(int, nums[:6]))

ser = serial.Serial(PORTA, BAUD, timeout=0.05)
time.sleep(2)
ser.reset_input_buffer()

plt.ion()
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4))
ax1.set_title("Matriz 1")
ax2.set_title("Matriz 2")

m1 = np.zeros((6, 6), dtype=int)
m2 = np.zeros((6, 6), dtype=int)

im1 = ax1.imshow(m1, interpolation="nearest", vmin=0, vmax=VMAX1)
im2 = ax2.imshow(m2, interpolation="nearest", vmin=0, vmax=VMAX2)

# ---------- Colorbar fora de cada matriz ----------
div1 = make_axes_locatable(ax1)
cax1 = div1.append_axes("right", size="4%", pad=0.15)
cbar1 = fig.colorbar(im1, cax=cax1)
cbar1.set_label("Intensidade (Δ ADC)")
cbar1.set_ticks([0, VMAX1//2, VMAX1])

div2 = make_axes_locatable(ax2)
cax2 = div2.append_axes("right", size="4%", pad=0.15)
cbar2 = fig.colorbar(im2, cax=cax2)
cbar2.set_label("Intensidade (Δ ADC)")
cbar2.set_ticks([0, VMAX2//2, VMAX2])

# mais espaço entre os plots
fig.subplots_adjust(wspace=0.35)
fig.canvas.draw()

# fundo do blit (depois de desenhar TUDO, inclusive as colorbars)
bg = fig.canvas.copy_from_bbox(fig.bbox)

capturando = False
rows1, rows2 = [], []

print("Lendo serial... (Ctrl+C para parar)")

try:
    while True:
        linha = ser.readline().decode(errors="ignore").strip()
        if not linha:
            continue

        if ("MATRIZ 1" in linha) and ("MATRIZ 2" in linha):
            capturando = True
            rows1, rows2 = [], []
            continue

        if not capturando:
            continue

        if "|" not in linha:
            continue

        esq, dir = linha.split("|", 1)
        a = extrair_6_ints(esq)
        b = extrair_6_ints(dir)
        if a is None or b is None:
            continue

        rows1.append(a)
        rows2.append(b)

        if len(rows1) == 6:
            m1 = np.array(rows1, dtype=int)
            m2 = np.array(rows2, dtype=int)

            im1.set_data(m1)
            im2.set_data(m2)

            # blit (rápido)
            fig.canvas.restore_region(bg)
            ax1.draw_artist(im1)
            ax2.draw_artist(im2)
            fig.canvas.blit(fig.bbox)
            fig.canvas.flush_events()

            capturando = False

except KeyboardInterrupt:
    pass
finally:
    ser.close()
