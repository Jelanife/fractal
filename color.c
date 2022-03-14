 #include <math.h>
 #include "libbmp.h"
 
 void hsv_to_rgb(double hue, double saturation, 
                    double value, rgb_pixel *pixel)
 {
    int r, g, b;
    double chroma, x, m, h_prime, r_1, g_1, b_1;

    chroma = value * saturation;
    h_prime = hue / 60;
    x = chroma * 1 - (double) fabs(fmod(h_prime, 2.0) - 1);

    if (0 <= h_prime && h_prime <= 1) 
    {
        r_1 = chroma;
        g_1 = x;
        b_1 = 0;
    } 
    else if (1 < h_prime && h_prime <= 2)
    {
        r_1 = x;
        g_1 = chroma;
        b_1 = 0;
    }
    else if (2 < h_prime && h_prime <=3)
    {
        r_1 = 0;
        g_1 = chroma;
        b_1 = x;
    }
    else if (3 < h_prime && h_prime <= 4)
    {
        r_1 = 0;
        g_1 = x; 
        b_1 = chroma;
    }
    else if (4 < h_prime && h_prime <= 5)
    {
        r_1 = x;
        g_1 = 0;
        b_1 = chroma;
    }
    else
    {
        r_1 = chroma;
        g_1 = 0;
        b_1 = x;
    }

    m = value - chroma;
    r = (int)((r_1 + m) * 255);
    g = (int)((g_1 + m) * 255);
    b = (int)((b_1 + m) * 255);

    *pixel = init_pixel(r,g,b);
 }  