#include "Mandelbrot.hpp"
#include "util.hpp"

//using Float = boost::multiprecision::mpfr_float;
//using Complex = boost::multiprecision::mpc_complex;

void omp_info();

int main() {
    omp_info();

    size_t prec = 64;
    size_t w_px = 64;
    size_t h_px = w_px;
    Float re_tar = Float(-1.26222162762384535370226702572022420406);  // -1.26222162762384535370226702572022420406
    Float im_tar = Float(0.04591700163513884695098681782544085357512);  // 0.04591700163513884695098681782544085357512
    Float w_tar = Float(2.0);
    Float h_tar = w_tar;
    Float w_tar_init = w_tar;
    size_t mcnt_max_init = 300;
    Palette pal = Palette::makeGradationHue(256, 0, 360, 1.0, 0.9);
    pal.reverse();
    size_t frames = 200;
    Float scale = Float(0.87);

    Mandelbrot m;
    m.setAllParams(prec, w_px, h_px, re_tar, im_tar, w_tar, h_tar, mcnt_max_init, pal);

    for (size_t i = 0; i < frames; i++) {
        // mandel count maxの動的変更
        Float zoom_ratio = w_tar_init / w_tar;
        size_t mcnt_max = static_cast<size_t>(
            std::min(
                static_cast<int>(mcnt_max_init + pow(10, log10(static_cast<Float>(zoom_ratio)))), 
                30000
            )
        );
        std::cout << "mandel count max: " << mcnt_max << std::endl;
        m.setMandelCountMax(mcnt_max);

        if (savePNG("./frames/output" + std::to_string(i) + ".png", m.makeColorVector(true), m.getWidthPx(), m.getHeightPx())) {
            std::cout << "PNG saved successfully: output" + std::to_string(i) + ".png\n";
        } else {
            std::cerr << "Failed to save PNG.\n";
        }

        w_tar *= scale;
        h_tar = w_tar * h_px / w_px;
        m.setComplexParams(re_tar, im_tar, w_tar, h_tar);
    }

    return 0;
}
