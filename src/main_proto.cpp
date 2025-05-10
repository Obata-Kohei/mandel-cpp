/*
#include <complex>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Mandelbrot_proto.hpp"

using Float = long double;
template <typename T>
using Complex = std::complex<T>;

void omp_info();

int main() {
    int width_px = 256;
    int height_px = 256;
    Float re_target = Float(-0.32);
    Float im_target = Float(0.63);
    Float width_target = Float(0.2);
    Float height_target = width_target * (height_px) / Float(width_px);
    Palette palette = Palette::makeGrayScale(256);
    //Palette palette = Palette::makeGradationHue(256, 0, 360, 1, 1);

    Mandelbrot<Float, Complex> mand;
    mand.setAllParams(width_px, height_px, re_target, im_target, width_target, height_target, palette);
    //std::vector<int> cnt_vec = mand.makeCountVector();
    //std::vector<Color> col_vec = mand.makeColorVector();

    omp_info();

    if (mand.savePNG("output.png")) {
        std::cout << "PNG saved successfully: output.png\n";
    } else {
        std::cerr << "Failed to save PNG.\n";
    }

    return 0;
}


void omp_info() {
    // OpenMPの実行環境に関する情報
    #ifdef _OPENMP
        std::cout << "OpenMP is enabled." << std::endl;
    #else
        std::cout << "*** OpenMP is NOT enabled. ***" << std::endl;
        return;
    #endif
    // 使用可能なプロセッサ数
    int num_procs = omp_get_num_procs();
    std::cout << "Number of processors: " << num_procs << std::endl;
    // 最大スレッド数
    int max_threads = omp_get_max_threads();
    std::cout << "Max number of threads: " << max_threads << std::endl;
}
*/