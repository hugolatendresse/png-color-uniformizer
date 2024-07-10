#ifndef PCU_H
#define PCU_H

#include <png.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct rgba_pixel_ {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    } RGBA_Pixel;

    typedef struct rgba_pixel_pos_double_ {
        double r;
        double g;
        double b;
        double a;
        unsigned int pos;
    } RGBA_Pixel_Pos_Double;

    // Width of RGBA pixel and RGBA pixel with position
    #define RGBA_LEN 4
    #define RGBA_POS_LEN 5
    #define RGBA_POS_DOUBLE_LEN 5

    // Globals
    extern int *clusters_sizes;
    extern bool kmodes;

    int display_image(int height, int width, unsigned char *data);
    int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format,
        unsigned int k);

    void print_vector(double *vector, int vector_size);
    void print_observations(double **observations, int observations_size, int vector_size);
    void print_rgba_pixel(RGBA_Pixel_Pos_Double *pixel);
    void print_rgba_pixels(RGBA_Pixel_Pos_Double *pixels, unsigned int pixel_cnt);

    void print_clusters(double ***clusters, int k, int observations_size, int vector_size);
    int compare_clusters(const int *cluster_map1, const int *cluster_map2, int clusters_size);

    int *km(double **centroids, double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
    double **create_centroids(double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
    int *partition(double **pixels, double **centroids, int k, unsigned int pixel_cnt, int member_cnt);
    void update_centroid(double **centroids, int *clusters_map, double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
    double ***map_clusters(int *clusters_map, double **pixels, int k, unsigned int pixel_cnt, int vector_size);
    double **map_cluster(const int *clusters_map, double **pixels, int c, unsigned int pixel_cnt, int vector_size);

    double *centroid_mean(double **pixels,unsigned int pixel_cnt, int member_cnt);
    double *centroid_mode(double **pixels,unsigned int pixel_cnt, int member_cnt);
    double *vsum(const double *vector1, const double *vector2, int vector_size);
    double *vsub(const double *vector1, const double *vector2, int vector_size);
    double innerprod(const double *vector1, const double *vector2, int vector_size);
    double norm(const double *vector, int vector_size);
    int rand_num(int range);

#ifdef __cplusplus
}
#endif

#endif
