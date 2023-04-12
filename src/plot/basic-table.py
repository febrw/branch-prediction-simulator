import pandas as pd
import matplotlib.pyplot as plt
import json

f = open("../jsons/basic-stats.json")

d = json.loads(f.read())

df = pd.DataFrame(d)
df.set_index("file_name",inplace=True)
df.sort_index(inplace=True)


t1 = df[['total_branches', 'unique_branches', 'branch_bias_prop.95', 'branch_bias_prop.99']].copy()
t1['hapax_proportion_unique'] = 100 * df['hapax_branches'] / df['unique_branches']
t1['hapax_proportion_total'] = 100 * df['hapax_branches'] / df['total_branches']

t2 = df[['total_branches']].copy()
t2['forward_prop'] = 100 * df['forward_branches'] / (df['forward_branches'] + df['backward_branches'])
t2['forward_taken_prop'] = 100 * df['forward_taken'] / df['forward_branches']
t2['backward_taken_prop'] = 100 * df['backward_taken'] / df['backward_branches']


t3 = t1[['total_branches', 'unique_branches', 'hapax_proportion_unique','hapax_proportion_total']].copy()


t4 = df[['branch_bias_prop.95', 'branch_bias_prop.99', 'branch_bias_prop.999']].copy()
print(t4)