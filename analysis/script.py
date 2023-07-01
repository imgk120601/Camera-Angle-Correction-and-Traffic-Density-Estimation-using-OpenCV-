import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

dataframe = pd.read_csv("/home/user/Downloads/2018CS10333_2019CS50430_ass1_part3/anal/MyTable4_4.csv",  )  # delimiter= ';'
x = dataframe.frameCount
y = dataframe.StaticqueueDensity
#print(dataframe.head(10))
plt.scatter(x, y )
plt.savefig("/home/user/Downloads/2018CS10333_2019CS50430_ass1_part3/anal/MyTable4_4.png")
#plt.show()