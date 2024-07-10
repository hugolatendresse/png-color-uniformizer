#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>
#include "../include/pcu.h"

int *clusters_sizes;

void print_rgba_pixel(RGBA_Pixel_Double *pixel) {
	char *format = "(%.2f, %.2f, %.2f, %.2f)";
	printf(format, pixel->r, pixel->g, pixel->b, pixel->a);
}

void print_rgba_pixels(RGBA_Pixel_Double *pixels, unsigned int pixel_cnt) {
	printf("[");

	for (int i = 0; i < pixel_cnt; i++) {
		if (i > 0)
			printf(", ");

		print_rgba_pixel(&(pixels[i]));
	}

	printf("]");
}

void print_vector(double *vector, int vector_size) {
	printf("(");

	for (int i = 0; i < vector_size; i++) {
		if (i > 0)
			printf(", ");

		printf("%.2f", vector[i]);
	}

	printf(")");
}

void print_observations(double **observations, int observations_size, int vector_size) {
	printf("[");
	
	for (int i = 0; i < observations_size; i++) {
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

void print_centroids(double **centroids, int k, int member_cnt) {
	for (int i = 0; i < k; i++) {
		printf("Centroid %d: ", i);
		print_vector(centroids[i], member_cnt);
		printf("\n");
	}
}

int compare_clusters(const int *clusters_map1, const int *clusters_map2, int clusters_size) {
	int i = 0;
	
	while (i < clusters_size) {
		if (clusters_map1[i] != clusters_map2[i])
			return 0;
		
		i++;
	}
	
	return 1;
}

// TODO implement the part that creates an array of clusters (after cluster_map is final) in a separate function


// member_cnt is needed to know how many fields to use in a given observation

int *km(double **centroids, double **pixels, int k, unsigned int pixel_cnt, int member_cnt) {
	print_centroids(centroids, k, member_cnt);
	clusters_sizes = (int *) calloc(k, sizeof(int));
	int *clusters_map = (int *) calloc(pixel_cnt, sizeof(int));

	if (pixel_cnt < k) {
		fprintf(stderr, "Could not compute clusters.");

		for (int i = 0; i < k; i++)
			free(centroids[i]);
		free(centroids);
		free(clusters_map);
		free(clusters_sizes);

		exit(1);
	}

	while (1) {
		print_centroids(centroids, k, member_cnt);
		int *new_clusters_map = partition(pixels, centroids, k, pixel_cnt, member_cnt);
		print_centroids(centroids, k, member_cnt);

		if (compare_clusters(clusters_map, new_clusters_map, pixel_cnt)) {
			free(clusters_map);
			return new_clusters_map;
		}

		free(clusters_map);
		clusters_map = new_clusters_map;
		update_centroid(centroids, clusters_map, pixels, k, pixel_cnt, member_cnt);
	}
}

double *centroid_mean(double **pixels, unsigned int pixel_cnt, int member_cnt) {
	double *vector = (double *) calloc(member_cnt, sizeof(double));
	
	for (int i = 0; i < pixel_cnt; i++) {
		double *temp = vsum(vector, pixels[i], member_cnt);
		free(vector);
		vector = temp;
	}
	
	for (int i = 0; i < member_cnt; i++)
		vector[i] /= pixel_cnt;
	
	return vector;
}

// Used for centroid_mode

typedef struct {
	double *pixel;
	unsigned int count;
} PixelFrequency;

// Similar as centroid_mean, but that calculates the mode instead of mean

// This function will be used to calculate the mode of the pixels in a cluster

// It returns a vector that corresponds to the pixel that occurs the most often in the cluster

double *centroid_mode(double **pixels, unsigned int pixel_cnt, int member_cnt) {
	PixelFrequency *frequencies = calloc(pixel_cnt, sizeof(PixelFrequency));
	unsigned int unique_pixel_cnt = 0;

	for (unsigned int i = 0; i < pixel_cnt; i++) {
		int found = 0;
		for (unsigned int j = 0; j < unique_pixel_cnt; j++) {
			if (memcmp(pixels[i], frequencies[j].pixel, member_cnt * sizeof(double)) == 0) {
				frequencies[j].count++;
				found = 1;
				break;
			}
		}
		if (!found) {
			frequencies[unique_pixel_cnt].pixel = pixels[i];
			frequencies[unique_pixel_cnt].count = 1;
			unique_pixel_cnt++;
		}
	}

	double *mode = NULL;
	unsigned int max_count = 0;
	for (unsigned int i = 0; i < unique_pixel_cnt; i++) {
		if (frequencies[i].count > max_count) {
			max_count = frequencies[i].count;
			mode = frequencies[i].pixel;
		}
	}

	free(frequencies);
	return mode;
}


double *vsum(const double *vector1, const double *vector2, int vector_size) {
	double *vector = (double *) malloc(sizeof(double) * vector_size);
	
	for (int i = 0; i < vector_size; i++)
		vector[i] = vector1[i] + vector2[i];
	
	return vector;
}

// Vector substraction

double *vsub(const double *vector1, const double *vector2, int vector_size) {
	double *vector = (double *) malloc(sizeof(double) * vector_size);
	
	for (int i = 0; i < vector_size; i++)
		vector[i] = vector1[i] - vector2[i];
	
	return vector;
}

double innerprod(const double *vector1, const double *vector2, int vector_size) {
	double prod = 0;
	
	for (int i = 0; i < vector_size; i++)
		prod += vector1[i] * vector2[i];
	
	return prod;
}

double norm(const double *vector, int vector_size) {
	return sqrt(innerprod(vector, vector, vector_size));
}

// Returns a unique random natural from 0 to size-1.
// Set it up and return a first number by specyfing the range
// Calling it with range=-1 will return a number not outputted yet
// range=-2 is just a free
int rand_num(int range) {
	static int *numArr = NULL;
	static int numNums = 0;
	int i, n;
	
	if (range == -2) {
		free(numArr);
		return -1;
	}
	
	if (range >= 0) {
		if (numArr != NULL)
			free(numArr);
		
		if ((numArr = (int *) malloc(sizeof(int) * range)) == NULL)
			return -1;
		
		for (i = 0; i < range; i++)
			numArr[i] = i;
		
		numNums = range;
	}
	
	if (numNums == 0)
		return -1;
	
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

// Function to check whether any two centroids are identical
// Returns true if some centroids are the same, false otherwise
bool some_centroids_are_the_same(double **centroids, int k, int member_cnt) {
	for (int i = 0; i < k; i++) {
		for (int j = i + 1; j < k; j++) {
			if (memcmp(centroids[i], centroids[j], member_cnt * sizeof(double)) == 0)
				return true;
		}
	}
	return false; // No centroids are the same
}

double **create_centroids(double **pixels, int k, unsigned int pixel_cnt, int member_cnt) {
	double **centroids = (double **) malloc(sizeof(double *) * k);

	srand(seed);  // Fixed seed for debugging

	int pixel_idx = rand_num(pixel_cnt);

	// Pick random pixels as centroids, without replacement
	// Will iterate until it finds centroids that are all different
	int cluster_idx = 0;
	if (forced_pixel != NULL) {
		// First centroid is equal to forced_pixel, if applicable
		centroids[cluster_idx] = (double *) malloc(sizeof(double) * member_cnt);
		*((RGBA_Pixel_Double *)centroids[cluster_idx]) = *forced_pixel;
		cluster_idx++;
		dbg_printf("Using forced_pixel for centroid %d\n", cluster_idx);
	}
	for (; cluster_idx < k; ++cluster_idx) {
		do {
			pixel_idx = rand_num(-1);
			centroids[cluster_idx] = (double *) malloc(sizeof(double) * member_cnt);
			for (int j = 0; j < member_cnt; j++) {
				centroids[cluster_idx][j] = pixels[pixel_idx][j];
			}
		} while (some_centroids_are_the_same(centroids, cluster_idx+1, member_cnt));
		dbg_printf("Using pixel %d for centroid %d\n", pixel_idx, cluster_idx);
	}

#ifdef DEBUG
	print_centroids(centroids, k, member_cnt);
#endif

	rand_num(-2); // free

	return centroids;
}

int *partition(double **pixels, double **centroids, int k, unsigned int pixel_cnt, int member_cnt) {
	int *clusters_map = (int *) malloc(sizeof(int) * pixel_cnt);
	float curr_distance;
	int centroid;

	for (int i = 0; i < pixel_cnt; i++) {
		float min_distance = DBL_MAX;

		for (int c = 0; c < k; c++) {
			double *temp = vsub(pixels[i], centroids[c], member_cnt);

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

void update_centroid(double **centroids, int *clusters_map, double **pixels, int k, unsigned int pixel_cnt, int member_cnt) {
	double **temp_arr = (double **) malloc(sizeof(double *) * pixel_cnt);

	for (int cluster_idx = 0, count = 0; cluster_idx < k; ++cluster_idx) {
		for (int i = 0; i < pixel_cnt; i++) {
			int curr = clusters_map[i];

			if (curr == cluster_idx) {
				temp_arr[count] = pixels[i];
				count++;
			}
		}

		if (kmodes) {
			centroids[cluster_idx] = centroid_mode(temp_arr, count, member_cnt);
		} else {
			centroids[cluster_idx] = centroid_mean(temp_arr, count, member_cnt);
		}
		print_centroids(centroids, k, member_cnt);
		count = 0;
	}

	free(temp_arr);
}

double ***map_clusters(int *clusters_map, double **observations, int k, unsigned int pixel_cnt, int vector_size) {
	double ***clusters = (double ***) malloc(sizeof(double **) * k);
	
	for (int i = 0; i < k; i++)
		clusters[i] = map_cluster(clusters_map, observations, i, pixel_cnt, vector_size);
	
	return clusters;
}

double **map_cluster(const int *clusters_map, double **observations, int c, unsigned int pixel_cnt, int vector_size) {
	int count = 0;
	int *temp_arr = (int *) malloc(sizeof(int) * pixel_cnt);
	
	for (int i = 0; i < pixel_cnt; i++) {
		if (clusters_map[i] == c) {
			temp_arr[count] = i;
			++count;
		}
	}
	
	double **cluster = (double **) malloc(sizeof(double *) * count);
	
	for (int i = 0; i < count; i++)
		cluster[i] = observations[temp_arr[i]];
	
	free(temp_arr);
	clusters_sizes[c] = count;
	
	return cluster;
}
