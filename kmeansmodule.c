#define PY_SSIZE_T_CLEAN
#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double **read_from_input(int, int);
double **init_k_clusters(double **, int, int);
void update_centroid(double **, int, int);
void vector_calculation(double *, double *, int, int);
double euclidian_dist(double *, double *, int);
int find_min_distance_cluster(double *, double **, int, int);
void assign_vector_to_cluster(double *, int, double **, int);
double *get_new_centroid(double **, int, int);
double max(double, double);
void check_memory_alloc(double *);
void check_memory_alloc_2d(double **);


int main(int argc, char *argv[])
{
    const double EPSILON = 0.001;
    int k;
    int d;
    int n;
    int iter = 0;
    double **vector_list;
    double **cluster_list;
    int new_cluster_id = -1;
    double *old_mean;
    double *new_mean;
    double max_mean_delta = 0;
    double *curr_mean;
    int i;
    int index_of_vector;
    int p;
    int index_of_cluster;
    int cluster;
    int val;
    int num_of_vector;

    if (argc != 5)
    {
        if (argc == 4)
        {
            iter = 200;
        }
        else
        {
            printf("An Error Has Occured");
            return 1;
        }
    }
    else
    {
        iter = atoi(argv[4]);
    }
    k = atoi(argv[1]);
    n = atoi(argv[2]);
    d = atoi(argv[3]);
    vector_list  
    cluster_list 
    old_mean = (double *)malloc(d * sizeof(double));
    check_memory_alloc(old_mean);
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
            new_mean = get_new_centroid(cluster_list, new_cluster_id, d);
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
    free(new_mean);

    for (cluster = 0; cluster < k; cluster++)
    {
        curr_mean = &cluster_list[cluster][d + 1];
        for (val = 0; val < d-1; val++)
        {
            printf("%.4f,", curr_mean[val]);
        }
        printf("%.4f", curr_mean[d-1]);
        printf("\n");
    }

    /* Free the allocated memory for vector_list */
    for (num_of_vector = 0; num_of_vector < n; num_of_vector++)
    {
        free(vector_list[num_of_vector]);
    }
    free(vector_list);

    return 0;
}

/*we need iter, k, n, d, vectorlist = [vector, cluster_of_vector], clusterlist = initialised_clusters = [size_of_cluster, vector_of_sums, mean_vector]*/
static PyObject* fit(PyObject *self, PyObject *args)
{
    double EPSILON;
    int k;
    int d;
    int n;
    int iter;
    PyObject *vector_list_pyobj;
    PyObject *cluster_list_pyobj;
    /* vector is [vector,cluster]*/
    /* cluster is [size,sum_of_cluster,mean_of_cluster]*/
    PyObject *vector;
    PyObject *elem_in_vector;
    PyObject *cluster;
    PyObject *elem_in_cluster;
    double num;
    int i;
    int j;
    double **vector_list;
    double **cluster_list;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if(!PyArg_ParseTuple(args, "diiiiOO", &EPSILON, &k, &d, &n, &iter, &vector_list_pyobj, &cluster_list_pyobj)) {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }
    /* THIS PROGRAM ASSUMES THAT vectorlist is [n][d+1] and clusterlist is [k][2d+1]
    n, d, k, are all values we send from python!*/
    if (PyObject_Length(vector_list_pyobj != n)){
        return NULL;
    }
    
    vector_list = (double **)malloc(n * sizeof(double *));
    /* memory alloc check */
    if (vector_list == NULL) {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    /* this parses pyobject to 2d array of vectors.*/
    for (i = 0; i < n; i++)
    {
        vector = PyList_GetItem(vector_list_pyobj, i);
        if (PyObject_Length(vector != d+1)){
            return NULL;
        }
        vector_list[i] = (double *)malloc((d + 1) * sizeof(double));
        /* memory alloc check */
        if (vector_list[i] == NULL) {
            printf("An Error Has Occurred\n");
            return NULL;
        }
        for (j = 0; j < d+1; j++){
            elem_in_vector = PyList_GetItem(vector, j);
            num = PyFloat_AsDouble(elem_in_vector);
            vector_list[i][j] = num;
        }
    }





/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("d", geo_c(z, n)); /*  Py_BuildValue(...) returns a PyObject*  */
}

double **read_from_input(int n, int d)
{
    /* allocate memory for vector list, each vector is represtented by an array of [vector, cluster_of_vector] */
    double **vector_list;
    int vector_index = 0;
    char *line = NULL;
    size_t len = 0;
    char *ptr;
    int i;
    int j;

    vector_list = (double **)malloc(n * sizeof(double *));
    check_memory_alloc_2d(vector_list);

    for (i = 0; i < n; i++)
    {
        vector_list[i] = (double *)malloc((d + 1) * sizeof(double));
        check_memory_alloc(vector_list[i]);
    }

    while ((getline(&line, &len, stdin)) != -1)
    {
        vector_list[vector_index][0] = strtod(line, &ptr);
        for (j = 1; j < d; j++)
        {
            vector_list[vector_index][j] = strtod(ptr + 1, &ptr);
        }
        /* initialise each vectors cluster to -1 */
        vector_list[vector_index][d] = -1;
        vector_index++;
    }

    free(line);

    return vector_list;
}

double **init_k_clusters(double **vector_list, int k, int d)
{
    /* allocate memory for cluster list,each cluster is represented by
       an array of [size_of_cluster, vector_of_sums, mean_vector] */
    double **cluster_list;
    int index_of_cluster;
    int j;
    cluster_list = (double **)malloc(k * sizeof(double *));
    check_memory_alloc_2d(cluster_list);

    for (index_of_cluster = 0; index_of_cluster < k; index_of_cluster++)
    {
        cluster_list[index_of_cluster] = (double *)malloc(((2 * d) + 1) * sizeof(double));
        check_memory_alloc(cluster_list[index_of_cluster]);
        cluster_list[index_of_cluster][0] = 1.0;
        for (j = 0; j < d; j++)
        {
            cluster_list[index_of_cluster][j + 1] = vector_list[index_of_cluster][j];
            cluster_list[index_of_cluster][j + 1 + d] = vector_list[index_of_cluster][j];
        }
        vector_list[index_of_cluster][d] = index_of_cluster;
    }

    return cluster_list;
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

double *get_new_centroid(double **cluster_list, int index_of_cluster, int d)
{
    double size;
    double *product;
    double num_mean = 0;
    int i;
    size = cluster_list[index_of_cluster][0];
    product = (double *)(malloc(d * sizeof(double)));
    check_memory_alloc(product);
    for (i = 1; i <= d; i++)
    {
        num_mean = cluster_list[index_of_cluster][i] / size;
        product[i-1] = num_mean;
    }
    return product;
}

void check_memory_alloc(double *p)
{
    if (p == NULL)
    {
        printf("An Error Has Occurred\n");
        exit(1);
    }
}

void check_memory_alloc_2d(double **p)
{
    if (p == NULL)
    {
        printf("An Error Has Occurred\n");
        exit(1);
    }
}

