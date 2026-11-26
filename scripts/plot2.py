import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("benchmark_vfs.csv")

fig, axes = plt.subplots(1, 3, figsize=(15, 4.8), dpi=150)

operations = [
    ("VFS_Touch_ms", "StdMap_Touch_ms", "Создание файлов (touch)"),
    ("VFS_Ls_ms", "StdMap_Ls_ms", "Просмотр директории (ls)"),
    ("VFS_Rm_ms", "StdMap_Rm_ms", "Удаление файлов (rm)")
]

for ax, (vfs_col, std_col, title) in zip(axes, operations):
    ax.plot(df["N"], df[vfs_col], marker="o", linewidth=2, markersize=5, label="VirtualFS (AVL)")
    ax.plot(df["N"], df[std_col], marker="s", linewidth=2, markersize=5, linestyle="--", label="std::map")

    ax.set_title(title, fontsize=11, pad=10)
    ax.set_xlabel("Количество файлов N", fontsize=10)
    ax.set_ylabel("Время (мс)", fontsize=10)
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(frameon=False, fontsize=9)

plt.tight_layout()
plt.show()