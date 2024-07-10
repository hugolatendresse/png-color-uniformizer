#ifndef PCU_H
#define PCU_H

#include <png.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    #ifdef DEBUG
    #define dbg_assert(expr) assert(expr)
    #define dbg_printf(...) ((void)printf(__VA_ARGS__))
    #else
    #define dbg_discard_expr_(...) ((void)((0) && printf(__VA_ARGS__)))
    #define dbg_assert(expr) dbg_discard_expr_("%d", !(expr))
    #define dbg_printf(...) dbg_discard_expr_(__VA_ARGS__)
    #endif

    typedef struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } RGB_Pixel;
    #define RGB_PIXEL_LEN (sizeof(RGB_Pixel)/sizeof(unsigned char))

    typedef struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    } RGBA_Pixel;
    #define RGBA_PIXEL_LEN (sizeof(RGBA_Pixel)/sizeof(unsigned char))

    typedef struct {
        double r;
        double g;
        double b;
        double a;
    } RGBAPixelDouble;

    // Globals
    extern int *clusters_sizes;
    extern bool kmodes;
    extern bool change_alpha;
    extern unsigned int seed;
    extern RGBAPixelDouble *forced_pixel;

    int display_image(int height, int width, unsigned char *data);
    int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format,
        unsigned int k);
    int patch(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format);

    void print_vector(double *vector, int vector_size);
    void print_observations(double **observations, int observations_size, int vector_size);
    void print_rgba_pixel(RGBAPixelDouble *pixel);
    void print_rgba_pixels(RGBAPixelDouble *pixels, unsigned int pixel_cnt);

    void print_clusters(double ***clusters, int k, int observations_size, int vector_size);
    int compare_clusters(const int *cluster_map1, const int *cluster_map2, int clusters_size);

    int *km(double **centroids, double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
    double **create_centroids(double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
    int *partition(double **pixels, double **centroids, int k, unsigned int pixel_cnt, int member_cnt);
    void update_centroids(double **centroids, int *clusters_map, double **pixels, int k, unsigned int pixel_cnt, int member_cnt);
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
