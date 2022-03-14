#include <math.h>

#include "libbmp.h"
#include "mandelbrot.h"
#include "color.h"

void malloc_set_data(set_data *set, int width, int height)
{
    int y;

    set->width = width;
    set->height = height;

    //malloc m_points in set
    set->data = (m_point**) malloc(sizeof(m_point*) * height);
    for (y = 0; y < height; y++)
    {
        set->data[y] = (m_point*) malloc(sizeof(m_point) * width);
    }
}

void free_set_data(set_data * set)
{
    int y;

    for (y = 0; y < set->height; y++)
    {
        free(set->data[y]);
    }
    free(set->data);
}


void escape_data(double x_0, 
                double y_0, 
                int max_iterations, 
                m_point* point_data)
{
    int iterations;
    double x, y, z, x_temp;

    iterations = 0;
    x = 0.0;
    y = 0.0;

    do 
    {
        x_temp = x*x - y*y + x_0;
        y = 2*x*y+y_0;
        x = x_temp;
        z = x*x + y*y;
        iterations++;
    }
    while(z <= 2*2 && iterations < max_iterations);

    point_data->iterations = iterations;
    point_data->z = z;
}


//draw mandelbrot bmp
//image must be inititalized...
void render_mandelbrot_bmp(bmp_image * image, 
                    double center_x,
                    double center_y,
                    double width,
                    int color_offset,
                    int max_iterations)
{
    double x_0, y_0, step, upper, left, logz, nu, max_nu;
    double ** nu_array;
    int x, y;
    m_point point_data;

    //allocate space to store nu values
    nu_array = (double**) malloc(sizeof(double*) * image->header.height);
    for (y = 0; y < image->header.height; y++)
    {
        nu_array[y] = (double *) malloc(sizeof(double) * image->header.width);
    }
    //calculate the distance between pixels and the left and lower bounds
    step = width / (double)image->header.width;
    left = center_x - step * image->header.width / 2;
    upper = center_y + step * image->header.height / 2;
    
    //populate nu_array with nu values based on z
    for (x = 0; x < image->header.width; x++)
    {
        for ( y = 0; y < image->header.height; y++)
        {
            y_0 = upper - step * y;
            x_0 = left + step * x;
            
            escape_data(x_0, y_0, max_iterations, &point_data);
            
            logz = log(point_data.z) / 2;
            nu = log(logz / log(2)) / log(2);
            
            nu_array[y][x] = nu;
            if (nu > max_nu)
            {
                max_nu = nu;
            }
        }
    }

    for (x = 0; x < image->header.width; x++)
    {
        for ( y = 0; y < image->header.height; y++)
        {
            if (!isnan(nu_array[y][x]))
            {
                color_scale_a(nu_array[y][x] / max_nu, color_offset, &image->data[y][x]);
            }
            else
            {
                image->data[y][x] = init_pixel(0,0,0);
            }
        }
    }

    //free nu_array
    for (y = 0; y < image->header.height; y++)
    {
        free(nu_array[y]);
    }
    free(nu_array);

}


void mandelbrot_set_data(set_data *set, 
                    double center_x, 
                    double center_y, 
                    double plot_width, 
                    int max_iterations)
{
    double x_0, y_0, step, upper, left;
    int x, y;
    m_point point_data;

    //calculate the distance between pixels and the left and lower bounds
    step = plot_width / (double)set->width;
    left = center_x - step * set->width / 2;
    upper = center_y + step * set->height / 2;
    
    //populate set with z and iteration count
    for (x = 0; x < set->width; x++)
    {
        for ( y = 0; y < set->height; y++)
        {
            y_0 = upper - step * y;
            x_0 = left + step * x;
            
            escape_data(x_0, y_0, max_iterations, &point_data);
            
            set->data[y][x] = point_data;
        }
    }
}

void mandelbrot_set_data_threaded(set_data *set, double center_x, 
                                double center_y, double width, 
                                int max_iterations)
{
    double x_0, y_0, step, upper, left;
    int y;
    pthread_mutex_t thread_count;

    //set number of threads to 0

    //while y < set->height if the number of threads is less than
    //max_threads add start a new thread
    
}

//processing for one thread
void mandelbrot_set_data_thread(m_point *row, int row_width, 
                                double start_x, double y, 
                                double step, int max_iterations,
                                pthread_mutex_t* thread_count)
{
    int x;
    m_point point;

    for (x = 0; x < row_width; x++)
    {
        escape_data(start_x + step * x, y, max_iterations, &point);
        row[x] = point;
    }
    //unlock thread count, decrement and relock
    pthread_mutex_unlock(thread_count);
    *thread_count--;
    pthread_mutex_lock(thread_count);

    //close the thread
    pthread_exit(NULL);
}

//smooth color gradient with saturated colors
// 0.0 <= nu <= 1.0
//ei new_color = (color + offset ) % 1535
void color_scale_a(double nu, int offset, rgb_pixel * pixel)
{
    int max_color, color;
    uint8_t r, g, b;

    max_color = 1535;
    color = (int) max_color * nu;
    color = (color + offset) % max_color;

    if (color < 256)
    {
        r = 255;
        g = color;
        b = 0;
    }
    else if (color < 512)
    {
        r = 255 - color + 256;
        g = 255;
        b = 0;
    }
    else if (color < 768)
    {
        r = 0;
        g = 255;
        b = color - 512;
    }
    else if (color < 1024)
    {
        r = 0;
        g = 255 - color + 768;
        b = 255;
    }
    else if (color < 1280)
    {
        r = color - 1024;
        g = 0;
        b = 255;
    }
    else
    {
        r = 255;
        g = 0;
        b = 255 - color + 1280;
    }

    *pixel = init_pixel(r ,g, b);
} 

//todo
//implement set_data_to_bmp
void set_data_to_bmp(set_data *set, bmp_image *image,
                     void(*set_color)(m_point*, rgb_pixel*))
{
    int x, y;
    m_point point;
    rgb_pixel pixel;


    for (x = 0; x < set->width; x++)
    {
        for (y = 0; y < set->height; y++)
        {
            point.iterations = set->data[y][x].iterations;
            point.z = set->data[y][x].z;
            set_color(&point, &pixel);
            image->data[y][x] = pixel;
        }
    }
}

/*
 * --------------------------------------
 * Functions to pass into set_data_to_bmp
 * --------------------------------------
 * 
 * These functions use the number of iterations and z stored in 
 * m_point to set the r,g,b value of pixel.
 */


/*
 * generate_color_hue_sweep
 * 
 * @param m_point *point
 * @param rgb_pixel *pixel
 */
void generate_color_hsv_hue_z_based(m_point *point, rgb_pixel *pixel)
{
    double nu, logz, hue, saturation, value;

    logz = log(point->z) / 2;
    nu = log(logz / log(2)) / log(2);

    if (isnan(nu))
    {
        hue = 0.0;
        saturation = 0.0;
        value = 0.0;
    }
    else
    {
        hue = 360*nu;
        saturation = .9;
        value = 1;
    }
    hsv_to_rgb(hue, saturation, value, pixel);

}

void generate_color_hsv_hue_it_based(m_point *point, rgb_pixel *pixel)
{
    double hue, saturation, value;


    if (point->iterations == 8000)
    {
        hue = 0.0;
        saturation = 0.0;
        value = 0.0;
    }
    else
    {
        hue = 360.0 * (double) log(point->iterations) / log(8000.0);
        saturation = 0.9;
        value = 0.9;
    }
    hsv_to_rgb(hue, saturation, value, pixel);

}