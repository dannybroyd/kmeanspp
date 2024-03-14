from sklearn.cluster import KMeans
from sklearn import datasets
import matplotlib.pyplot as plt
import numpy as np

iris = datasets.load_iris()
data = iris['data']
inertia_list = []
k_list = []

for k in range(1, 11):
    model = KMeans(n_clusters=k, init='k-means++', random_state=0).fit(data)
    inertia_list.append(model.inertia_)
    k_list.append(k)

max_slope_proportion = 0
chosen_k = 0
for i in range(8):
    slope_1 = (inertia_list[i+1] - inertia_list[i])
    slope_2 = (inertia_list[i+2] - inertia_list[i+1])
    if slope_1/slope_2 > max_slope_proportion:
        max_slope_proportion = slope_1/slope_2
        chosen_k = i+2

print(chosen_k)
plt.plot(k_list, inertia_list)
plt.xticks(k_list)
plt.xlabel('-----> k')
plt.show()

