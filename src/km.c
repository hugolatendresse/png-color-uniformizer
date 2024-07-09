#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>
#include "../include/km.h"
#include "../include/uniformizer.h"

#define ERR_NO_NUM -1
#define ERR_NO_MEM -2
#define FREED_RAND -3


int *clusters_sizes;

void print_vector(double *vector, int vector_size) {
	printf("(");
	
	for (int i = 0; i < vector_size; ++i) {
		if (i > 0)
			printf(", ");
		
		printf("%.2f", vector[i]);
	}
	
	printf(")");
}

void print_rgba_pixel(RGBA_Pixel_Pos_Double *pixel) {
	char *format = "(%.2f, %.2f, %.2f, %.2f, %u)";
	printf(format, pixel->r, pixel->g, pixel->b, pixel->a, pixel->pos);
}

void print_rgba_pixels(RGBA_Pixel_Pos_Double *pixels, unsigned int pixel_cnt) {
	printf("[");

	for (int i = 0; i < pixel_cnt; ++i) {
		if (i > 0)
			printf(", ");

		print_rgba_pixel(&(pixels[i]));
	}

	printf("]");
}

void print_observations(double **observations, int observations_size, int vector_size) {
	printf("[");
	
	for (int i = 0; i < observations_size; ++i) {
		if (i > 0)
			printf(", ");
		
		print_vector(observations[i], vector_size);
	}
	
	printf("]");
}

void print_clusters(double ***clusters, int k, int observations_size, int vector_size) {
	printf("{");
	
	for (int cluster_idx = 0; cluster_idx < k; ++cluster_idx) {
		if (cluster_idx > 0)
			printf(", ");

		print_observations(clusters[cluster_idx], clusters_sizes[cluster_idx], vector_size);
		printf("\n");
	}
	
	free(clusters_sizes);
	printf("}");
}

int compare_clusters(const int *clusters_map1, const int *clusters_map2, int clusters_size) {
	int i = 0;
	
	while (i < clusters_size) {
		if (clusters_map1[i] != clusters_map2[i])
			return 0;
		
		++i;
	}
	
	return 1;
}

// TODO create a generic version that allows to pass a distance metric. Will be able to handle km_rgba and anything else
double ***km(double **observations, int k, int observations_size, int vector_size) {
	clusters_sizes = (int *) calloc(k, sizeof(int));
	int *clusters_map = (int *) calloc(observations_size, sizeof(int));
	double **centroids = initialize(observations, k, observations_size, vector_size);

	if (observations_size < k) {
		printf("Could not compute clusters.");

		for (int i = 0; i < k; ++i)
			free(centroids[i]);
		free(centroids);
		free(clusters_map);
		free(clusters_sizes);

		exit(1);
	}

	while (1) {
		int *new_clusters_map = partition(observations, centroids, k, observations_size, vector_size);

		if (compare_clusters(clusters_map, new_clusters_map, observations_size)) {
			double ***clusters = map_clusters(clusters_map, observations, k, observations_size, vector_size);

			for (int i = 0; i < k; ++i)
				free(centroids[i]);
			free(centroids);
			free(clusters_map);
			free(new_clusters_map);

			return clusters;
		}

		for (int i = 0; i < k; ++i)
			free(centroids[i]);
		free(centroids);
		free(clusters_map);
		clusters_map = new_clusters_map;
		centroids = re_centroids(clusters_map, observations, k, observations_size, vector_size);
	}
}

// member_cnt is needed to know how many fields to use in the struct
int *km_rgba(RGBA_Pixel_Pos_Double *pixels, int k, unsigned int pixel_cnt, int member_cnt) {
	clusters_sizes = (int *) calloc(k, sizeof(int));
	int *clusters_map = (int *) calloc(pixel_cnt, sizeof(int));
	double **centroids = initialize_rgba(pixels, k, pixel_cnt, member_cnt);

	if (pixel_cnt < k) {
		fprintf(stderr, "Could not compute clusters.");

		for (int i = 0; i < k; ++i)
			free(centroids[i]);
		free(centroids);
		free(clusters_map);
		free(clusters_sizes);

		exit(1);
	}

	while (1) {
		int *new_clusters_map = partition_rgba(pixels, centroids, k, pixel_cnt, member_cnt);

		if (compare_clusters(clusters_map, new_clusters_map, pixel_cnt)) {
			// double ***clusters = map_clusters(clusters_map, pixels, k, pixel_cnt, member_cnt);

			for (int i = 0; i < k; ++i)
				free(centroids[i]);
			free(centroids);
			free(clusters_map);
			// free(new_clusters_map);

			// return clusters;
			return new_clusters_map;
		}

		for (int i = 0; i < k; ++i) {
			free(centroids[i]);
		}
		free(centroids);
		free(clusters_map);
		clusters_map = new_clusters_map;
		centroids = re_centroids_rgba(clusters_map, pixels, k, pixel_cnt, member_cnt);
	}
}



double *centroid(double **observations, int observations_size, int vector_size) {
	double *vector = (double *) calloc(vector_size, sizeof(double));
	
	for (int i = 0; i < observations_size; ++i) {
		double *temp = vsum(vector, observations[i], vector_size);
		free(vector);
		vector = temp;
	}
	
	for (int i = 0; i < vector_size; ++i)
		vector[i] /= observations_size;
	
	return vector;
}

double *vsum(const double *vector1, const double *vector2, int vector_size) {
	double *vector = (double *) malloc(sizeof(double) * vector_size);
	
	for (int i = 0; i < vector_size; ++i)
		vector[i] = vector1[i] + vector2[i];
	
	return vector;
}

// Vector substraction
double *vsub(const double *vector1, const double *vector2, int vector_size) {
	double *vector = (double *) malloc(sizeof(double) * vector_size);
	
	for (int i = 0; i < vector_size; ++i)
		vector[i] = vector1[i] - vector2[i];
	
	return vector;
}

double innerprod(const double *vector1, const double *vector2, int vector_size) {
	double prod = 0;
	
	for (int i = 0; i < vector_size; ++i)
		prod += vector1[i] * vector2[i];
	
	return prod;
}

double norm(const double *vector, int vector_size) {
	return sqrt(innerprod(vector, vector, vector_size));
}

/* Loved this shuffling random algorithm
 * Source: http://stackoverflow.com/a/5064432
 */
int rand_num(int size) {
	static int *numArr = NULL;
	static int numNums = 0;
	int i, n;
	
	if (size == -22) {
		free(numArr);
		return FREED_RAND;
	}
	
	if (size >= 0) {
		if (numArr != NULL)
			free(numArr);
		
		if ((numArr = (int *) malloc(sizeof(int) * size)) == NULL)
			return ERR_NO_MEM;
		
		for (i = 0; i < size; ++i)
			numArr[i] = i;
		
		numNums = size;
	}
	
	if (numNums == 0)
		return ERR_NO_NUM;
	
	n = rand() % numNums;
	i = numArr[n];
	numArr[n] = numArr[numNums - 1];
	numNums--;
	
	if (numNums == 0) {
		free(numArr);
		numArr = 0;
	}
	
	return i;
}

double **initialize(double **observations, int k, int observations_size, int vector_size) {
	double **centroids = (double **) malloc(sizeof(double *) * k);
	
	srand(time(NULL));
	int r = rand_num(observations_size);
	
	for (int cluster_idx = 0; cluster_idx < k; ++cluster_idx) {
		centroids[cluster_idx] = (double *) malloc(sizeof(double) * vector_size);
		for (int j = 0; j < vector_size; ++j) {
			centroids[cluster_idx][j] = observations[r][j];
			r = rand_num(-1);
		}
	}
	
	rand_num(-22);
	
	return centroids;
}

double **initialize_rgba(RGBA_Pixel_Pos_Double *pixels, int k, unsigned int pixel_cnt, int member_cnt) {
	double **centroids = (double **) malloc(sizeof(double *) * k);

	srand(time(NULL));
	int r = rand_num(pixel_cnt);

	// TODO an immediate and probably necessary improvement to this is to pick centroids at random until we have k unique ones
	//  One way would be to stratify by alpha (take min alpha, max alpha, and divisions in between)
	for (int cluster_idx = 0; cluster_idx < k; ++cluster_idx) {
		centroids[cluster_idx] = (double *) malloc(sizeof(double) * member_cnt);
		for (int j = 0; j < member_cnt; ++j) {
			double *pixel_d = (double *)&pixels[r];
			centroids[cluster_idx][j] = pixel_d[j];
			r = rand_num(-1);
		}
	}

	rand_num(-22);

	return centroids;
}

int *partition(double **observations, double **centroids, int k, int observations_size, int vector_size) {
	int *clusters_map = (int *) malloc(sizeof(int) * observations_size);
	float curr_distance;
	int centroid;
	
	for (int i = 0; i < observations_size; ++i) {
		float min_distance = DBL_MAX;
		
		for (int c = 0; c < k; c++) {
			double *temp = vsub(observations[i], centroids[c], vector_size);
			
			if ((curr_distance = norm(temp, vector_size)) < min_distance) {
				min_distance = curr_distance;
				centroid = c;
			}
			
			free(temp);
		}
		
		clusters_map[i] = centroid;
	}
	
	return clusters_map;
}

int *partition_rgba(RGBA_Pixel_Pos_Double *pixels, double **centroids, int k, unsigned int pixel_cnt, int member_cnt) {
	int *clusters_map = (int *) malloc(sizeof(int) * pixel_cnt);
	float curr_distance;
	int centroid;

	for (int i = 0; i < pixel_cnt; ++i) {
		float min_distance = DBL_MAX;

		for (int c = 0; c < k; c++) {
			double *temp = vsub((double *)(&pixels[i]), centroids[c], member_cnt);

			if ((curr_distance = norm(temp, member_cnt)) < min_distance) {
				min_distance = curr_distance;
				centroid = c;
			}

			free(temp);
		}

		clusters_map[i] = centroid;
	}

	return clusters_map;
}



double **re_centroids(int *clusters_map, double **observations, int k, int observations_size, int vector_size) {
	double **centroids = (double **) malloc(sizeof(double *) * k);
	double **temp_arr = (double **) malloc(sizeof(double *) * observations_size);
	
	for (int c = 0, count = 0; c < k; ++c) {
		for (int i = 0; i < observations_size; ++i) {
			int curr = clusters_map[i];
			
			if (curr == c) {
				temp_arr[count] = observations[i];
				++count;
			}
		}
		
		centroids[c] = centroid(temp_arr, count, vector_size);
		count = 0;
	}
	
	free(temp_arr);
	
	return centroids;
}


double **re_centroids_rgba(int *clusters_map, RGBA_Pixel_Pos_Double *pixels, int k, unsigned int pixel_cnt, int member_count) {
	double **centroids = (double **) malloc(sizeof(double *) * k);
	double **temp_arr = (double **) malloc(sizeof(double *) * pixel_cnt);

	for (int cluster_idx = 0, count = 0; cluster_idx < k; ++cluster_idx) {
		for (int i = 0; i < pixel_cnt; ++i) {
			int curr = clusters_map[i];

			if (curr == cluster_idx) {
				temp_arr[count] = (double *)&(pixels[i]);
				count++;
			}
		}

		centroids[cluster_idx] = centroid(temp_arr, count, member_count);
		count = 0;
	}

	free(temp_arr);

	return centroids;
}

double ***map_clusters(int *clusters_map, double **observations, int k, int observations_size, int vector_size) {
	double ***clusters = (double ***) malloc(sizeof(double **) * k);
	
	for (int i = 0; i < k; ++i)
		clusters[i] = map_cluster(clusters_map, observations, i, observations_size, vector_size);
	
	return clusters;
}

double **map_cluster(const int *clusters_map, double **observations, int c, int observations_size, int vector_size) {
	int count = 0;
	int *temp_arr = (int *) malloc(sizeof(int) * observations_size);
	
	for (int i = 0; i < observations_size; ++i) {
		if (clusters_map[i] == c) {
			temp_arr[count] = i;
			++count;
		}
	}
	
	double **cluster = (double **) malloc(sizeof(double *) * count);
	
	for (int i = 0; i < count; ++i)
		cluster[i] = observations[temp_arr[i]];
	
	free(temp_arr);
	clusters_sizes[c] = count;
	
	return cluster;
}
