from sklearn.cluster import KMeans
from sklearn import datasets
import matplotlib.pyplot as plt

iris = datasets.load_iris()
data = iris['data']
inertia_list = []
k_list = []

for k in range(1, 11):
    model = KMeans(n_clusters=k, init='k-means++', random_state=0).fit(data)
    inertia_list.append(model.inertia_)
    k_list.append(k)

plt.plot(k_list, inertia_list)
plt.xticks(k_list)
plt.xlabel('-----> k')
plt.show()
