#ifndef KM_H
#define KM_H

#include "uniformizer.h"
#define UNIFORMIZER_H

void print_vector(double *vector, int vector_size);
void print_observations(double **observations, int observations_size, int vector_size);
void print_rgba_pixel(RGBA_Pixel_Pos_Double *pixel);
void print_rgba_pixels(RGBA_Pixel_Pos_Double *pixels, unsigned int pixel_cnt);

void print_clusters(double ***clusters, int k, int observations_size, int vector_size);
int compare_clusters(const int *cluster_map1, const int *cluster_map2, int clusters_size);

double ***km(double **observations, int k, int observations_size, int vector_size);
int *km_rgba(double **centroids, double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
double *centroid(double **observations, int observations_size, int vector_size);
double *vsum(const double *vector1, const double *vector2, int vector_size);
double *vsub(const double *vector1, const double *vector2, int vector_size);
double innerprod(const double *vector1, const double *vector2, int vector_size);
double norm(const double *vector, int vector_size);

int rand_num(int size);

double **initialize(double **observations, int k, int observations_size, int vector_size);
double **create_centroids_rgba(double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
int *partition(double **observations, double **centroids, int k, int observations_size, int vector_size);
int *partition_rgba(double **pixels, double **centroids, int k, unsigned int pixel_cnt, int member_cnt);
double **re_centroids(int *clusters_map, double **observations, int k, int observations_size, int vector_size);
void re_centroids_rgba(double **centroids, int *clusters_map, double **pixels, int k, unsigned int pixel_cnt, int member_count);
double ***map_clusters(int *clusters_map, double **observations, int k, int observations_size, int vector_size);
double **map_cluster(const int *clusters_map, double **observations, int c, int observations_size, int vector_size);

#endif
