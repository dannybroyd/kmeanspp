import sys
import pandas as pd
import numpy as np
import mykmeanssp as km

np.random.seed(0)
chosen_centroid_indexes = []

def kmeans_pp(k, max_iter, epsilon, n, cluster_data):
    d = len(cluster_data[0])
    initialised_centroids = np.zeros((k, d))
    added_clusters_dict = {}
    random_value = np.random.choice(n)
    chosen_centroid_indexes.append(random_value)
    # choose first cluster at random
    initialised_centroids[0] = cluster_data[random_value]
    added_clusters_dict[random_value] = 1
    for cent_index in range(1, k):
        cluster_dists = get_cluster_dists(cluster_data, added_clusters_dict, initialised_centroids, n, cent_index)
        initialised_centroids = add_cluster_to_initialised(initialised_centroids, cluster_data, cluster_dists, added_clusters_dict, n, cent_index)
    cluster_data = cluster_data.tolist()
    initialised_centroids = initialised_centroids.tolist()
    initialised_centroids = format_lists_for_c(cluster_data, initialised_centroids, chosen_centroid_indexes)
    results = km.fit(epsilon, k, d, n, max_iter, cluster_data, initialised_centroids)
    print_output(results, chosen_centroid_indexes)



def get_cluster_dists(cluster_data, added_clusters_dict, initialised_centroids, n, cent_index):
    cluster_dists = np.zeros(n)
    for cluster_index in range(len(cluster_data)):
        if cluster_index in added_clusters_dict:
            continue
        cluster = cluster_data[cluster_index]
        cluster_dists[cluster_index] = dist_from_centroids(cluster, initialised_centroids, cent_index)
    return cluster_dists


def add_cluster_to_initialised(initialised_centroids, cluster_data, cluster_dists, added_clusters_dict, n, cent_index):
    dists_sum = np.sum(cluster_dists)
    cluster_probs = cluster_dists / dists_sum
    random_value = np.random.choice(n, p=cluster_probs)
    chosen_centroid_indexes.append(random_value)
    initialised_centroids[cent_index] = cluster_data[random_value]
    added_clusters_dict[random_value] = 1
    return initialised_centroids


def dist_from_centroids(cluster, initialised_centroids, cent_index):
    distances = []
    for index in range(cent_index):
        distances.append(np.linalg.norm(initialised_centroids[index] - cluster))
    return np.min(distances)


def main():
    k, max_iter, epsilon, file_path1, file_path2 = get_sys_arguments()
    df1 = pd.read_csv(file_path1, header=None)
    df2 = pd.read_csv(file_path2, header=None)
    n = len(df1)
    check_inputs(k, n, max_iter)
    data = df_to_nparray(df1, df2)
    kmeans_pp(int(k), int(max_iter), float(epsilon), n, data)


def df_to_nparray(df1, df2):
    df = pd.merge(df1, df2, left_on=0, right_on=0, how='inner')
    df = df.sort_values(by=0, ascending=True)
    df = df.iloc[:, 1:]
    data = df.values
    return data


def check_inputs(k, n, max_iter):
    if int(k) <= 1 or int(k) >= int(n) or not k.isdigit():
        print("Invalid number of clusters!")
        quit()
    if int(max_iter) <= 1 or int(max_iter) >= 1000 or not max_iter.isdigit():
        print("Invalid maximum iteration!")
        quit()


def get_sys_arguments():
    if len(sys.argv) != 6:
        if len(sys.argv) == 5:
            max_iter = '300'
        else:
            print("An Error Has Occurred")
            quit()
    else:
        max_iter = sys.argv[2]
    k = sys.argv[1]
    epsilon = sys.argv[len(sys.argv) - 3]
    file_path1 = sys.argv[len(sys.argv) - 2]
    file_path2 = sys.argv[len(sys.argv) - 1]
    return k, max_iter, epsilon, file_path1, file_path2


def format_lists_for_c(cluster_data, initialised_centroids, chosen_centroid_indexes):
    product_init_centroids = []
    # we assume cluster_data and initialised_centroids are a list - not numpy
    # we format like this: cluster_data = [[vector, cluster_which_it_belongs_to]] - done in-place
    # initialised_centroid = [[size_of_cluster, sum_of_vectors_in_cluster, cluster_mean]] - not in-place
    curr_cluster = 0
    # set cluster of each vector -1
    for cluster in cluster_data:
        cluster.append(-1)
    for index in chosen_centroid_indexes:
        # set cluster of each vector which has cluster 
        cluster_data[index][len(cluster)-1] = curr_cluster
        curr_cluster += 1
    for centroid in initialised_centroids:
        centroid = centroid + centroid
        centroid.insert(0, 1)
        product_init_centroids.append(centroid)
    return product_init_centroids


def print_output(results, chosen_centroid_indexes):
    # print chosen centroids indexes 
    for i in range(len(chosen_centroid_indexes)-1):
        print(chosen_centroid_indexes[i], end=",")
    print(chosen_centroid_indexes[len(chosen_centroid_indexes)-1])

    # print result of C algorithm
    for result in results:
        for i in range(len(result)-1):
            print('%.4f' % result[i], end=",")
        print('%.4f' % result[len(result)-1])

        


if __name__ == "__main__":
    main()
