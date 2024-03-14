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

elbow_point = 3
plt.scatter(elbow_point, inertia_list[elbow_point-1], color='none', s=1000, zorder=3, edgecolor='r', linestyle='--')
plt.plot(k_list, inertia_list)
plt.xticks(k_list)
plt.xlabel('-----> k')
plt.ylabel('Inertia')
plt.arrow(elbow_point+2, inertia_list[elbow_point-1]+200, -1.5, -150, head_width=0.2, head_length=15)
plt.annotate('Elbow Point', xy=(elbow_point+1.5, inertia_list[elbow_point-1]+210))
plt.savefig('elbow.png')

