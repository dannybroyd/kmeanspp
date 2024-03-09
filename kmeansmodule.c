#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "linker.h"


/*we need iter, k, n, d, vectorlist = [vector, cluster_of_vector], clusterlist = initialised_clusters = [size_of_cluster, vector_of_sums, mean_vector]*/
static PyObject *fit(PyObject *self, PyObject *args)
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
    PyObject* python_double;
    PyObject* centroid_pyobj;
    PyObject* final_centroids_pyobj;
    double num;
    double num_in_cluster;
    int i;
    int j;
    int cluster;
    int num_of_vector;
    double **vector_list;
    double **cluster_list;
    double **final_centroids;

    if (!PyArg_ParseTuple(args, "diiiiOO", &EPSILON, &k, &d, &n, &iter, &vector_list_pyobj, &cluster_list_pyobj))
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }
    /* THIS PROGRAM ASSUMES THAT vectorlist is [n][d+1] and clusterlist is [k][2d+1]
    n, d, k, are all values we send from python!*/
    if (PyObject_Length(vector_list_pyobj) != n)
    {
        return NULL;
    }

    vector_list = (double **)malloc(n * sizeof(double *));
    /* memory alloc check */
    if (vector_list == NULL)
    {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    /* this parses pyobject to 2d array of vectors.*/
    for (i = 0; i < n; i++)
    {
        vector = PyList_GetItem(vector_list_pyobj, i);
        if (PyObject_Length(vector) != d + 1)
        {
            return NULL;
        }
        vector_list[i] = (double *)malloc((d + 1) * sizeof(double));
        /* memory alloc check */
        if (vector_list[i] == NULL)
        {
            printf("An Error Has Occurred\n");
            return NULL;
        }
        for (j = 0; j < d + 1; j++)
        {
            elem_in_vector = PyList_GetItem(vector, j);
            num = PyFloat_AsDouble(elem_in_vector);
            vector_list[i][j] = num;
        }
    }
    if (PyObject_Length(cluster_list_pyobj) != k)
    {
        return NULL;
    }
    cluster_list = (double **)malloc(k * sizeof(double *));
    /* memory alloc check */
    if (cluster_list == NULL)
    {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    /* this parses pyobject to 2d array of clusters.*/
    for (i = 0; i < n; i++)
    {
        cluster = PyList_GetItem(cluster_list_pyobj, i);
        if (PyObject_Length(cluster) != (2 * d + 1))
        {
            return NULL;
        }
        cluster_list[i] = (double *)malloc((2 * d + 1) * sizeof(double));
        /* memory alloc check */
        if (cluster_list[i] == NULL)
        {
            printf("An Error Has Occurred\n");
            return NULL;
        }
        for (j = 0; j < (2 * d + 1); j++)
        {
            elem_in_cluster = PyList_GetItem(cluster, j);
            num_in_cluster = PyFloat_AsDouble(elem_in_cluster);
            cluster_list[i][j] = num_in_cluster;
        }
    }

    /* run kmeans in C */
    final_centroids = kmeans(double EPSILON, int k, int d, int n, int iter, double **vector_list, double **cluster_list);

    /* build pyobject list from 2d array in C */
    final_centroids_pyobj = PyList_New(k);
    for (i = 0; i < k; i++){
        centroid_pyobj = PyList_New(d);
        for (j = 0; j < d; j++)
        {
            python_double = PyFloat_FromDouble(final_centroids[i][j]);
            PyList_SetItem(centroid_pyobj, j, python_double); 
        }
        PyList_SetItem(final_centroids_pyobj, i, centroid_pyobj);
    }

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

    /* Free the allocated memory for final_centroids */
    for (cluster = 0; cluster < k; cluster++)
    {   
        free(final_centroids[cluster]);
    }
    free(final_centroids);

    /* this returns a 2d list as a pyobj */
    return final_centroids_pyobj;
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                                                                                                                                                                                /* the Python method name that will be used */
     (PyCFunction)fit,                                                                                                                                                                     /* the C-function that implements the Python function and returns static PyObject*  */
     METH_VARARGS,                                                                                                                                                                         /* flags indicating parameters
                                                                                                                                                             accepted for this function */
     PyDoc_STR(" the kmeanspp algorithm recieves args: epsilon, iter, k, n, d, vectorlist = [vector, cluster_of_vector], clusterlist  = [size_of_cluster, vector_of_sums, mean_vector]")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}                                                                                                                                                                  /* The last entry must be all NULL as shown to act as a
                                                                                                                                                                                              sentinel. Python looks for this entry to know that all
                                                                                                                                                                                              of the functions for the module have been defined. */
};

static struct PyModuleDef mykmeanssp = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL,         /* module documentation, may be NULL */
    -1,           /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&mykmeanssp);
    if (!m)
    {
        return NULL;
    }
    return m;
}