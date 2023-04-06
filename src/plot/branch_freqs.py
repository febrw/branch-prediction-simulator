import pandas as pd
import matplotlib.pyplot as plt
import sys
import json

f = open("../jsons/branch-freqs.json")


d = json.loads(f.read())


df = pd.DataFrame(d["branch_freqs"])

#print(df)
df.groupby(by="frequency").count().plot(legend=True)

print(df)
#df.rename(columns={"branch_address": "count of each branch in trace file"}, inplace=True)
#plt.show()