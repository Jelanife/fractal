#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mandelbrot.h"
#include "libbmp.h"
#include "color.h"


int main(int argc, char *argv[])
{
    bmp_image *img;
    set_data *sd;
	uint32_t img_width = 0, img_height = 0;
    double x = -100, y = -100, plot_width = 0;
    int opt = 0, output_hue_scan = 0, hue_end = 0, 
		max_iterations = 0;
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
	

	// Check the required arguments if any are 0, run default

	if ( img_width == 0 || img_height == 0 || x == -100 || y == -100 ||
		 plot_width == 0 ) {
		img_width = 600;
		img_height = 400;
		x = -2;
		y = 0;
		plot_width = 4;
		printf("Invalid or missing parameters, printing default...\n");
	}
		 
    img = (bmp_image*) malloc(sizeof(bmp_image));
    sd = (set_data*) malloc(sizeof(set_data));
    malloc_set_data(sd, img_width, img_height);
    bmp_image_malloc(img, img_width, img_height);

    if (output_hue_scan)
        hue_end = 1535;
    else
        hue_end = 10;
    

    for (int i = 0; i < hue_end; i+=10){
        mandelbrot_set_data(sd, x, y, plot_width, max_iterations);
        sprintf(file_path_post, "pics/%s_%d.bmp", file_name, i);
            
        set_data_to_bmp(sd, img, generate_color_hsv_hue_z_based);
        fwrite_bmp_image(img, file_path_post); 
    }        
    bmp_image_free(img);
    free(img);
    free_set_data(sd);
    free(sd);
    
    return 0;
}

