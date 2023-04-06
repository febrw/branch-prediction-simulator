import pandas as pd
import matplotlib.pyplot as plt
import sys
import json


f = open("../jsons/two-bit-table-results.json")


d = json.loads(f.read())


df = pd.json_normalize(d, record_path=['outputs'], meta="file_name")

df.set_index("file_name", inplace=True)
df.groupby('table_size')['missprediction_rate'].plot(legend=True)
plt.xticks(rotation="vertical")
plt.show()

