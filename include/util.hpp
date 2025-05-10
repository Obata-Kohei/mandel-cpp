#include <iostream>
#include <omp.h>
#include <png.h>
#include "Color.hpp"

void omp_info();

bool savePNG(const std::string& filename, const std::vector<Color>& img, size_t width_px, size_t height_px);

// ffmpeg -framerate 30 -i output%d.png -c:v libx264 -pix_fmt yuv420p output.mp4
//bool makeMP4(const std::string& filename, )