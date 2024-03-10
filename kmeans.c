#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "linker.h"

void update_centroid(double **, int, int);
void vector_calculation(double *, double *, int, int);
double euclidian_dist(double *, double *, int);
int find_min_distance_cluster(double *, double **, int, int);
void assign_vector_to_cluster(double *, int, double **, int);
double *get_new_centroid(double **, int, int, double *);
double max(double, double);
double** kmeans(double, int, int, int, int, double **, double **);
void free_vector_cluster_lists(double **, double **, int, int);

int main(){
    return 0;
}

double** kmeans(double EPSILON, int k, int d, int n, int iter, double **vector_list, double **cluster_list)
{
    int new_cluster_id = -1;
    double *old_mean;
    double *new_mean = NULL;
    double max_mean_delta = 0;
    double *curr_mean;
    int i;
    int index_of_vector;
    int p;
    int index_of_cluster;
    int cluster;
    int val;
    double **final_centroids;
    double *product;
    int curr_cluster;

    old_mean = (double *)malloc(d * sizeof(double));

    /* check memory allocation */
    if (old_mean == NULL){
        free(old_mean);
        free_vector_cluster_lists(vector_list, cluster_list, k, n);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    product = (double *)malloc(d * sizeof(double));

    /* check memory allocation */
    if (product == NULL){
        free(old_mean);
        free(product);
        free_vector_cluster_lists(vector_list, cluster_list, k, n);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    for (i = 0; i < iter; i++)
    {
        for (index_of_vector = 0; index_of_vector < n; index_of_vector++)
        {
            new_cluster_id = find_min_distance_cluster(vector_list[index_of_vector], cluster_list, d, k);
            assign_vector_to_cluster(vector_list[index_of_vector], new_cluster_id, cluster_list, d);
            for (p = 0; p < d; p++)
            {
                /* copy old mean to compare with new mean later */
                old_mean[p] = cluster_list[new_cluster_id][d + 1 + p];
            }
            new_mean = get_new_centroid(cluster_list, new_cluster_id, d, product);
            max_mean_delta = max(euclidian_dist(old_mean, new_mean, d), max_mean_delta);
        }
        for (index_of_cluster = 0; index_of_cluster < k; index_of_cluster++)
        {
            update_centroid(cluster_list, index_of_cluster, d);
        }
        if (max_mean_delta < EPSILON)
        {
            break;
        }
        max_mean_delta = 0;
    }
    free(old_mean);
    if (new_mean == NULL){
        free(product);
    }
    else{
        free(new_mean);
    }
    

    final_centroids = (double **)malloc(k * sizeof(double*));

    /* check memory allocation */
    if (final_centroids == NULL){
        free(final_centroids);
        free_vector_cluster_lists(vector_list, cluster_list, k, n);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    for (cluster = 0; cluster < k; cluster++)
    {   
        final_centroids[cluster] = (double *)malloc(d * sizeof(double));

        /* check memory allocation */
        if (final_centroids[cluster] == NULL){
            for(curr_cluster = 0; curr_cluster < cluster; curr_cluster++){
                free(final_centroids[curr_cluster]);
            }
            free_vector_cluster_lists(vector_list, cluster_list, k, n);
            free(final_centroids);
            printf("An Error Has Occurred\n");
            return NULL;
        }
        
        curr_mean = &cluster_list[cluster][d + 1];
        for (val = 0; val < d; val++)
        {
            final_centroids[cluster][val] = curr_mean[val];
        }
    }

    return final_centroids;
}

void update_centroid(double **cluster_list, int index_of_cluster, int d)
{
    double size;
    double num_mean = 0;
    int i;
    size = cluster_list[index_of_cluster][0];
    for (i = 1; i <= d; i++)
    {
        num_mean = cluster_list[index_of_cluster][i] / size;
        cluster_list[index_of_cluster][i + d] = num_mean;
    }
}

void vector_calculation(double *vector1, double *vector2, int add_or_sub, int d)
{
    int i;
    int j;
    if (add_or_sub == 0)
    {
        /* this means we use addition */
        for (i = 0; i < d; i++)
        {
            vector1[i] += vector2[i];
        }
    }
    else
    {
        /* this means we substract*/
        for (j = 0; j < d; j++)
        {
            vector1[j] -= vector2[j];
        }
    }
}

double euclidian_dist(double *vector1, double *vector2, int d)
{
    double sum = 0;
    int i;
    for (i = 0; i < d; i++)
    {
        sum += pow((vector1[i] - vector2[i]), 2);
    }
    return sqrt(sum);
}

int find_min_distance_cluster(double *vector_x, double **cluster_list, int d, int k)
{
    double *cluster_mean_pointer;
    double min_distance;
    int min_cluster_id = 0;
    double curr_distance;
    int i;
    cluster_mean_pointer = &cluster_list[0][d + 1];
    min_distance = euclidian_dist(vector_x, cluster_mean_pointer, d);
    curr_distance = min_distance;
    for (i = 1; i < k; i++)
    {
        cluster_mean_pointer = &cluster_list[i][d + 1];
        curr_distance = euclidian_dist(vector_x, cluster_mean_pointer, d);
        if (curr_distance < min_distance)
        {
            min_distance = curr_distance;
            min_cluster_id = i;
        }
    }
    return min_cluster_id;
}

void assign_vector_to_cluster(double *vector, int new_cluster_id, double **cluster_list, int d)
{
    int old_cluster_id;
    double *sum_of_new_cluster_p;
    double *sum_of_old_cluster_p;
    old_cluster_id = vector[d];
    if (old_cluster_id == new_cluster_id)
    {
        return;
    }
    if (old_cluster_id != -1)
    {
        /* updates old cluster's size and sum, don't need to if vector wasn't in cluster */
        cluster_list[old_cluster_id][0] -= 1;
        sum_of_old_cluster_p = &cluster_list[old_cluster_id][1];
        vector_calculation(sum_of_old_cluster_p, vector, 1, d);
    }
    /* updates new cluster */
    cluster_list[new_cluster_id][0] += 1;
    sum_of_new_cluster_p = &cluster_list[new_cluster_id][1];
    vector_calculation(sum_of_new_cluster_p, vector, 0, d);
    /* update vector */
    vector[d] = new_cluster_id;
}

double max(double a, double b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

double *get_new_centroid(double **cluster_list, int index_of_cluster, int d, double *product)
{
    double size;
    double num_mean = 0;
    int i;
    size = cluster_list[index_of_cluster][0];
    for (i = 1; i <= d; i++)
    {
        num_mean = cluster_list[index_of_cluster][i] / size;
        product[i - 1] = num_mean;
    }
    return product;
}

void free_vector_cluster_lists(double **vector_list, double **cluster_list, int k, int n){
    int cluster;
    int num_of_vector;
    /* Free the allocated memory for cluster_list */
    for (cluster = 0; cluster < k; cluster++)
    {   
        free(cluster_list[cluster]);
    }
    free(cluster_list);

    /* Free the allocated memory for vector_list */
    for (num_of_vector = 0; num_of_vector < n; num_of_vector++)
    {
        free(vector_list[num_of_vector]);
    }
    free(vector_list);
}
