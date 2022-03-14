#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mandelbrot.h"
#include "libbmp.h"
#include "color.h"


/* int main(int argc, char *argv[])
{
    bmp_image *img;
    uint32_t img_width, img_height;
    double x, y, plot_width;
    int opt, output_hue_scan, hue_end, max_iterations;
    char file_name [200];
    char file_path_post [300];
    

    //Command line args
    max_iterations = MAX_ITERATIONS;
    output_hue_scan = 0;
    while ((opt = getopt(argc, argv, "w:h:x:y:p:f:i:s")) != -1)
    {
        switch (opt)
        {
        case 'w':
            img_width = atoi(optarg);
            printf("%s\n", optarg);
            break;
        case 'h':
            img_height = atoi(optarg);
            printf("%s\n", optarg);
            break;
        case 'x':
            x = strtod(optarg, NULL);
            printf("%s\n", optarg);
            break;
        case 'y':
            y = strtod(optarg, NULL);
            printf("%s\n", optarg);
            break;
        case 'p':
            plot_width = strtod(optarg, NULL);
            printf("%s\n", optarg);
            break;
        case 'f':
            strcpy(file_name, optarg);
            break;
        case 's':
            output_hue_scan = 1;
            break;
        case 'i':
            max_iterations = atoi(optarg);
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }
    } 

    img = (bmp_image*) malloc(sizeof(bmp_image));

    bmp_image_malloc(img, img_width, img_height);

    if (output_hue_scan)
        hue_end = 1535;
    else
        hue_end = 10;
    
    for (int i = 0; i < hue_end; i+=10){
        render_mandelbrot_bmp(img, x, y, plot_width, i, max_iterations);
        sprintf(file_path_post, "pics/%s_%d.bmp", file_name, i);
        fwrite_bmp_image(img, file_path_post);
    }

    bmp_image_free(img);
    free(img);

    
    return 0;
}
 */

int main(int argc, char *argv[])
{
    /* code */
    int res_width, res_height;
    char buffer[200];
    double plot_width;

    set_data set;
    bmp_image image;

    res_width = 600;
    res_height = 400;

    
    malloc_set_data(&set, res_width, res_height);
    bmp_image_malloc(&image, res_width, res_height);
    plot_width = 4;
    for (int i = 1; i < 300; i++){
        mandelbrot_set_data(&set, -0.1992599180602, 1.10009614189035, plot_width, 8000);
        set_data_to_bmp(&set, &image, generate_color_hsv_hue_it_based);
        //set_data_to_bmp(&set, &image, generate_color_hsv_hue_z_based);
        sprintf(buffer, "pics/zoom/1/img_%04d.bmp", i);
        fwrite_bmp_image(&image, buffer);
        plot_width *= 0.95;
    }

    free_set_data(&set);
    bmp_image_free(&image);

    return 0;
}
