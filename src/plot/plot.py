import pandas as pd
import matplotlib.pyplot as plt

d = {"sell": [
           {
               "Rate": 0.001425,
               "Quantity": 537.27713514
           },
           {
               "Rate": 0.00142853,
               "Quantity": 6.59174681
           }
]}

df = pd.DataFrame(d['sell'])
print (df)

df.plot(x='Quantity', y='Rate')
plt.show()