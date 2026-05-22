"""
plot_graphs.py  –  called automatically by the C++ toolkit after sorting evaluation.
Usage: python3 plot_graphs.py sort_results.csv
Reads the CSV written by C++, produces one PNG per input pattern.
"""

import sys
import csv
import os
from collections import defaultdict

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# config 

COLORS  = {"Bubble": "#e74c3c", "Insertion": "#e67e22",
           "Merge":  "#2980b9", "Quick":     "#27ae60"}
MARKERS = {"Bubble": "o", "Insertion": "s", "Merge": "^", "Quick": "D"}
ALGOS   = ["Bubble", "Insertion", "Merge", "Quick"]

# load CSV 

def load(path):
    """Return  { pattern: { algo: [times...] }, sizes: [100,1000,...] }"""
    patterns = defaultdict(lambda: defaultdict(list))
    sizes_per_pattern = defaultdict(list)

    with open(path, newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            p = row['pattern']
            n = int(row['n'])
            sizes_per_pattern[p].append(n)
            for algo in ALGOS:
                patterns[p][algo].append(float(row[algo]))

    return patterns, sizes_per_pattern

# plot one pattern 

def plot_pattern(label, algo_times, sizes, out_dir):
    fig, ax = plt.subplots(figsize=(8, 4.5))
    fig.patch.set_facecolor("#335D99")
    ax.set_facecolor("#39448C")

    for algo in ALGOS:
        ax.plot(sizes, algo_times[algo],
                marker=MARKERS[algo], color=COLORS[algo],
                linewidth=2.2, markersize=7, label=algo)

    ax.set_xlabel("Input Size (n)", color='white', fontsize=12)
    ax.set_ylabel("Runtime (ms)",   color='white', fontsize=12)
    ax.set_title(f"Sorting Performance – {label}", color='white',
                 fontsize=13, fontweight='bold')
    ax.tick_params(colors='white')
    for spine in ax.spines.values():
        spine.set_color('#334155')
    ax.grid(True, linestyle='--', alpha=0.3, color='white')
    ax.legend(facecolor='#1a1a2e', edgecolor='#334155',
              labelcolor='white', fontsize=10)
    ax.set_xticks(sizes)
    ax.set_xticklabels([str(s) for s in sizes], color='white')

    plt.tight_layout()
    safe = label.replace(" ", "_").lower()
    out  = os.path.join(out_dir, f"graph_{safe}.png")
    plt.savefig(out, dpi=150, bbox_inches='tight',
                facecolor=fig.get_facecolor())
    plt.close()
    print(f"  Saved: {out}")


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 plot_graphs.py sort_results.csv")
        sys.exit(1)

    csv_path = sys.argv[1]
    if not os.path.exists(csv_path):
        print(f"Error: file not found: {csv_path}")
        sys.exit(1)

    out_dir = os.path.dirname(os.path.abspath(csv_path))
    patterns, sizes_map = load(csv_path)

    print(f"Generating {len(patterns)} graph(s)...")
    for label, algo_times in patterns.items():
        plot_pattern(label, algo_times, sizes_map[label], out_dir)

    print("Done.")

if __name__ == "__main__":
    main()