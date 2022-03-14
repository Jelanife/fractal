#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#ifndef libbmp_h
#define libbmp_h

//error codes
typedef enum _BMP_ERRORS{
    NO_ERROR,
    BMP_FILE_ERROR,
    BMP_ERROR
} BMP_ERRORS;

//padding
#define compute_padding(a) ((a) % 4)

//header includes info header
typedef struct _bmp_header
{   
    //header 14 bytes 
    uint32_t filesize;              //file size in bytes
    uint32_t reserved;              //reserved for...
    uint32_t data_offset;           //offset from 0x00 to bmp data

    //Info Header 40 bytes
    uint32_t size;                  //size of the info header
    uint32_t width;                 //horizontal width in pixels
    uint32_t height;                //vertical height in pixels
    uint16_t planes;                //number of planes
    uint16_t bits_per_pixel;        //number of bits per pixel
    uint32_t compression;           //0 is no compression
    uint32_t image_size;             //0 for no compression
    uint32_t x_pixels_per_meter;    //horizontal resolution
    uint32_t y_pixels_per_meter;    //virtical resoltion
    uint32_t colors_used;           //2^24 colors used ie 8-bits per pixel
    uint32_t important_colors;      //0=all
} bmp_header;

//24 bit pixel
typedef struct _rgb_pixel
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} rgb_pixel;

//image
typedef struct _bmp_image
{
    bmp_header header;
    rgb_pixel ** data;
} bmp_image;

//init header
void init_header(bmp_header * image, uint32_t width, uint32_t height);

//init pixel
#define init_pixel(r,g,b) (rgb_pixel){b,g,r}

//allocate memory for a bmp_image
void bmp_image_malloc(bmp_image * image, uint32_t width, uint32_t height);

//free bmp_image
void bmp_image_free(bmp_image * image);
//write header to file
int fwrite_header(bmp_header* header, char* file_path);

//write bmp_image to file
int fwrite_bmp_image(bmp_image* header, char* file_path);

//TODO
//read bmp_image from a file
int fread_bmp_image(bmp_image* image, char* file_path);

#endif