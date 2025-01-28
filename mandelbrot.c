#include <math.h>

#include "libbmp.h"
#include "mandelbrot.h"
#include "color.h"
#include <pthread.h>

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


void escape_data_threaded(worker_data * data)
{
    int iterations;
    double x, y, z, x_temp;
    //init the worker_data
    iterations = 0;
    x = 0.0;
    y = 0.0;

    do 
    {
        x_temp = x*x - y*y + data->x_0;
        y = 2*x*y+data->y_0;
        x = x_temp;
        z = x*x + y*y;
        iterations++;
    }
    while(z <= 2*2 && iterations < data->max_iterations);

    data->point_data.iterations = iterations;
    data->point_data.z = z;
	pthread_exit(NULL);
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

void mandelbrot_set_data_threaded(set_data *set, 
                    double center_x, 
                    double center_y, 
                    double plot_width, 
                    int max_iterations)
{
    double step, upper, left;
    int x, y, i, n_threads;
	worker_data *worker_data_pool;
	pthread_t *thread_pool;
	pthread_attr_t attr;

	//allocate worker data pool MAX_THREADS	
	worker_data_pool = (worker_data*) malloc(sizeof(worker_data) * MAX_THREADS);
	thread_pool = (pthread_t*) malloc(sizeof(pthread_t) * MAX_THREADS);

    //calculate the distance between pixels and the left and lower bounds
    step = plot_width / (double)set->width;
    left = center_x - step * set->width / 2;
    upper = center_y + step * set->height / 2;
    
    //populate set with z and iteration count
    for (x = 0; x < set->width; x++)
    { 
        y = 0;
        while (y < set->height)
        {
            //y_0 = upper - step * y;
            //x_0 = left + step * x;
           
		    //j = y;
			
			//Compute the number of threads for this iteration
			if (y + MAX_THREADS >= set->height) 
            {
				n_threads = set->height - y;
			} else {
				n_threads = MAX_THREADS;
			}
			
			//Setup thread attributes
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			//Create point pool for the threads and start threads
			for (i = 0; i < n_threads; i++)
            {
				worker_data_pool[i].y_0 = upper - step * (y + i);
				worker_data_pool[i].x_0 = left + step * (x);
				worker_data_pool[i].max_iterations = max_iterations;
				pthread_create(&thread_pool[i], 
				 			   &attr, 
				               (void *) escape_data_threaded,
							   &worker_data_pool[i]);
			}
			
			//Join the threads record the calculatated data
			for (i = 0; i < n_threads; i++)
            {
				pthread_join(thread_pool[i], NULL);
				set->data[y+i][x] = worker_data_pool[i].point_data;

			}
			pthread_attr_destroy(&attr); 
           	y += n_threads; 
        }
	//Free allocated memory
    }
    free(worker_data_pool);
	free(thread_pool);
}

//Write the set data to a bmp struct
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
