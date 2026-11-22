import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("benchmark.csv")

fig, axes = plt.subplots(1, 3, figsize=(15, 4.8), dpi=150)

operations = [("AVL_Insert_ms", "StdMap_Insert_ms", "Вставка (Insert)"), ("AVL_Search_ms", "StdMap_Search_ms", "Поиск (Search)"),("AVL_Remove_ms", "StdMap_Remove_ms", "Удаление (Remove)")]

for ax, (avl_col, std_col, title) in zip(axes, operations):
    ax.plot(df["N"], df[avl_col], marker="o", linewidth=2, markersize=5, label="AVLTree")
    ax.plot(df["N"], df[std_col], marker="s", linewidth=2, markersize=5, linestyle="--", label="std::map")

    ax.set_title(title, fontsize=11, pad=10)
    ax.set_xlabel("Количество элементов N", fontsize=10)
    ax.set_ylabel("Время (мс)", fontsize=10)
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(frameon=False, fontsize=9)

plt.tight_layout()
plt.show()