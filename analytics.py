import pandas as pd
import matplotlib.pyplot as plt

# Load tournament data
try:
    df = pd.read_csv('match_stats.csv')
except FileNotFoundError:
    print("Error: match_stats.csv not found. Run the C++ tournament executable first.")
    exit()

# 1. Calculate General Aggregates
total_games = len(df)
win_counts = df['Winner'].value_counts()
reason_counts = df['Reason'].value_counts()

print("--- TOURNAMENT TELEMETRY SUMMARY ---")
print(f"Total Games Played: {total_games}")
print("\nWin Distribution:")
print(win_counts)
print("\nTermination Reasons:")
print(reason_counts)

# 2. Generate Plots
# Increased figure width slightly to accommodate the legend
plt.figure(figsize=(14, 6))

# Plot 1: Match Outcomes (Pie Chart)
plt.subplot(1, 2, 1)
counts = df['Winner'].value_counts()

# Plot pie without labels. Hide percentages for slices smaller than 2% to prevent text overlap.
counts.plot(
    kind='pie', 
    labels=None, 
    autopct=lambda p: f'{p:.1f}%' if p > 2 else '', 
    startangle=90
)

plt.title('Match Outcome Distribution')
plt.ylabel('')

# Add a legend outside the pie chart with calculated percentages
plt.legend(
    labels=[f"{label} ({(val/total_games)*100:.1f}%)" for label, val in counts.items()],
    loc="center left",
    bbox_to_anchor=(0.9, 0.5)
)

# Plot 2: Game Length Distribution (Histogram)
plt.subplot(1, 2, 2)
plt.hist(df['TotalMoves'], bins=20, edgecolor='black', alpha=0.7)
plt.axvline(df['TotalMoves'].mean(), color='red', linestyle='dashed', linewidth=1, label=f"Mean: {df['TotalMoves'].mean():.1f}")
plt.title('Game Length Distribution (Moves)')
plt.xlabel('Number of Moves')
plt.ylabel('Frequency')
plt.legend()

plt.tight_layout()
plt.savefig('tournament_analytics.png')
plt.show()
