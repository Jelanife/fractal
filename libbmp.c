#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"

//bmp_header init
void init_header(bmp_header * header,
                uint32_t width,
                uint32_t height)
{
    header->filesize = (width + compute_padding(width)) * height + 54;
    header->reserved = 0;
    header->data_offset = 54;
    header->size = 40;
    header->width = width;
    header->height = height;
    header->planes = 1;
    header->bits_per_pixel = 24;
    header->compression = 0;
    header->image_size = 0;
    header->x_pixels_per_meter = 0;
    header->y_pixels_per_meter = 0;
    header->colors_used = 16777216;
    header->important_colors = 0;
}


//allocate memory for a bmp_image
void bmp_image_malloc(bmp_image * image, 
                        uint32_t width, 
                        uint32_t height)
{
    uint32_t y;
    init_header(&image->header, width, height);
    //allocate row pointers
    image->data = (rgb_pixel**) malloc(
                        sizeof(rgb_pixel*) * height);
    //allocate image rows
    for (y = 0; y < height; y++){
        image->data[y] = (rgb_pixel*) malloc(
                        sizeof(rgb_pixel) * width);
    }
}

//free bmp_image
void bmp_image_free(bmp_image *image)
{
    uint32_t y;

    for (y = 0; y < image->header.height; y++){
        free(image->data[y]);
    }
    free(image->data);
}

//write header to file
int fwrite_header(bmp_header * header, char* file_path)
{
    FILE * file;
    uint16_t signature;

    signature = 0x4d42;
    file = fopen(file_path, "w+");
    if (file)
    {
        fwrite(&signature, sizeof(uint16_t), 1, file);
        fwrite(header, sizeof(bmp_header), 1, file);
        fclose(file);
        return NO_ERROR;
    }
    else
    {
        return BMP_FILE_ERROR;
    }
}

//write bmp_image to file
int fwrite_bmp_image(bmp_image * image, char * file_path)
{
    FILE * file;
    uint32_t y;
    int padding;
    
    uint8_t padding_chars [3] = {0,0,0};
    padding = compute_padding(image->header.width);
    if (NO_ERROR == fwrite_header(&image->header, file_path))
    {
        file = fopen(file_path, "a+");
        if (file)
        {
            fseek(file, 54, SEEK_SET);
            for (y = image->header.height; y > 0; y--)
            {
                fwrite(image->data[y-1], 
                    sizeof(rgb_pixel) * image->header.width,
                    1, file);
                fwrite(padding_chars, 
                    1, padding, file);
                
            }
            fclose(file);
            return NO_ERROR;
        }
        else
        {
            return BMP_FILE_ERROR;
        }
    }
    else
    {
        return BMP_FILE_ERROR;  
    }
}

//read bmp_image from a file
int fread_bmp_image(bmp_image * image, char * file_path)
{
    return NO_ERROR;
}



