import pandas as pd
import matplotlib.pyplot as plt
import json


f = open("../jsons/perceptron-comparison-results.json")


d = json.loads(f.read())


df = pd.json_normalize(d, record_path=['results'], meta="file_name")
df.set_index('file_name', inplace=True)
df.groupby('name')['missprediction_rate']

#print(df)

# Define the bar width
bar_width = 0.2

# Define the x-axis labels
x_labels = df.index.unique()

# Define the x-positions for each group of bars
x_pos = [i * 1.5 for i in range(len(x_labels))]

# Define the colors for each table size
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728']

# Create a figure and axis object
fig, ax = plt.subplots()

# Loop through each table size and create a group of bars for each file
for i, size in enumerate(df['name'].unique()):
    # Get the missprediction rates for the current table size
    y_values = df[df['name'] == size]['missprediction_rate'].values
    # Get the x-positions for the current group of bars
    current_x_pos = [x + (i * bar_width) for x in x_pos]
    # Create a group of bars for the current table size
    ax.bar(current_x_pos, y_values, color=colors[i], width=bar_width, label=size)

# Set the x-axis ticks and labels
ax.set_xticks(x_pos)
ax.set_xticklabels(x_labels, rotation=30, fontsize=14)

# Set the y-axis label
ax.set_ylabel('Missprediction Rate (%)', fontsize=16)

# Add a legend
ax.legend(title='Table Size', fontsize=14)
plt.title("Global, Local, and Gshare perceptron predictors", fontsize=26)
# Show the plot

plt.show()

