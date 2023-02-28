#include <pthread.h>

#include "libbmp.h"

//mandelbrot set
#define MAX_ITERATIONS 10000

/*
 * m_point
 * 
 * Store the maginude, z, and number of iterations
 */
typedef struct m_point
{
    double z;
    int iterations;
} m_point;

/*
 * set_data
 * 
 * Store computed mandelbrot set data
 */
typedef struct _set_data
{
    int width;
    int height;
    m_point ** data;
} set_data;

/*
 * malloc_set_data
 * 
 * @param int width
 * @param int height
 * @param set_data* set
 */
void malloc_set_data(set_data *set, int width, int height);


/* 
 * free_set_data
 * 
 * free allocated set data
 */
void free_set_data(set_data * set);


/*
 * escape_iterations
 *
 * @param double x_0
 * @param double y_0
 * @param int max_iterations
 * @param m_point * data
 * 
 * This function determines whether or not point (x_0, y_0) is in the 
 * mandelbrot set.  Iteration caps to max_iterations if z remains
 * bounded.  When computation is complete, max_iterations or bailout, 
 * z and the number iterations are written to &data.
 */
void escape_data(double, double, int, m_point*);



/*
 * mandelbrot_set_data
 * 
 * @param set_data_ *set
 * @param double center_x
 * @param double center_y
 * @param double width
 * @param int max_iterations
 * 
 * Compute mandelbrot set data representing points in/adjecent to the 
 * mandelbrot set, store data in the set_data parameter.
 */
void mandelbrot_set_data(set_data *set, double center_x, double center_y, 
                        double width, int max_iterations);


/*
 * mandelbrot_set_data_threaded
 * 
 * @param set_data_ *set
 * @param double center_x
 * @param double center_y
 * @param double width
 * @param int max_iterations
 * 
 * Compute mandelbrot set data representing points in/adjecent to the 
 * mandelbrot set, store data in the set_data parameter.  Use a thread
 * to handle rows using MAX_THREADS threads.
 */


void color_scale_a(double nu, int offset, rgb_pixel * pixel);



/*
 * set_data_to_bmp
 * 
 * @param set_data *set
 * @param bmp_image *image
 * @param void(*set_color)(m_point*, rgb_pixel*)
 * 
 * *set_color operates on an m_pixel in set and stores the 
 * result in an rgb_pixel.  This operation is performed on
 * each point set and saves the result in the corresponding
 * pixel in image.  image and set must have the same resolion. 
 */
void set_data_to_bmp(set_data *set, bmp_image *image,
                     void(*set_color)(m_point*, rgb_pixel*));



/*
 * --------------------------------------
 * Functions to pass into set_data_to_bmp
 * --------------------------------------
 */


/*
 * generate_color_hue_sweep using iterations
 * 
 * @param m_point *point
 * @param rgb_pixel *pixel
 * 
 *  This is too hot
 */
void generate_color_hsv_hue_z_based(m_point *point, rgb_pixel *pixel);


/*
 * generate_color_hue_sweep using iterations
 * 
 * @param m_point *point
 * @param rgb_pixel *pixel
 */
void generate_color_hsv_hue_it_based(m_point *point, rgb_pixel *pixel);
