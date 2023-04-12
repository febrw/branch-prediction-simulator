import pandas as pd
import matplotlib.pyplot as plt
import sys
import json


f = open("../jsons/always-taken-results.json")


d = json.loads(f.read())

df = pd.DataFrame(d)

#df = pd.json_normalize(d, record_path=['outputs'], meta="file_name")
#print (df)

#df.plot(x="file_name", y="missprediction_rate")
df.set_index("file_name", inplace=True)
#print (df)


plt.figure(figsize=(6,4))
plt.legend()
plt.ylabel("Missprediction Rate (%)", fontsize=16)
plt.xticks(rotation=30, fontsize=14)
plt.yticks(fontsize=14)
plt.plot(df, marker='o')
plt.title("Missprediction rate  - Always Taken Predictor", fontsize=26)
plt.grid()
plt.show()

