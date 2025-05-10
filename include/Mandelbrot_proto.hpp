#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <vector>
#include <cmath>
#include <png.h>
#include <omp.h>
#include "Color.hpp"
#include "Palette.hpp"

// Complex<Float> の形で、複素数型を扱う
// 実数型 Float と複素数型のテンプレート Complex
// Float: e.g. double
// Complex: e.g. std::complex<double>
template <typename Float, template<typename> class Complex>
class Mandelbrot {
    public:

    // 全てのパラメタの設定
    void setAllParams(
        int width_px, int height_px,
        Float re_target, Float im_target,
        Float width_target, Float height_target,
        const Palette& palette
    );

    // 複素平面上のパラメタを設定
    void setComplexParams(
        Float re_target, Float im_target,
        Float width_target, Float height_target
    );

    // Setter
    void setWidthPx(int width_px);
    void setHeightPx(int height_px);
    void setReTarget(Float re_target);
    void setImTarget(Float im_target);
    void setWidthTarget(Float width_target);
    void setHeightTarget(Float height_target);
    void setPalette(const Palette& palette);
    //static void setMandelCountMax(int mandel_count_max);

    // Getter
    int getWidthPx() const;
    int getHeightPx() const;
    Float getReTarget() const;
    Float getImTarget() const;
    Float getWidthTarget() const;
    Float getHeightTarget() const;
    Palette getPalette() const;
    int getMandelCountMax() const;

    // ラスタースキャン順の height_px * width_px サイズのvector.
    // mandelCountの結果を格納する
    std::vector<int> makeCountVector() const;

    // ラスタースキャン順の height_px * width_px サイズのvector.
    // nToColorの結果を格納する
    std::vector<Color> makeColorVector() const;

    // PNGファイルの作成
    bool savePNG(const std::string& filename) const;


    private:
    int width_px, height_px;
    Float re_target, im_target, width_target, height_target;
    Palette palette;

    Float re_min, re_max;
    Float im_min, im_max;
    static const int mandel_count_max = 700;

    // 画像上の座標x, yから対応する複素平面上の複素数を得る
    Complex<Float> getComplexAt(const int x, const int y) const;

    // abs(z)が初めて2を超えるnを計算する
    int mandelCount(const Complex<Float>& c) const;

    // mandelCountの結果nからpalette中の⾊を決めるstatic method
    Color nToColor(int n) const;

    // 発散にかかる回数 n の勾配のマグニチュードを，makeCountVectorの結果から計算
    std::vector<Float> calcGradMag() const;
};

// implementation

template <typename Float, template<typename> class Complex>
void Mandelbrot<Float, Complex>::setAllParams(
    int width_px, int height_px,
    Float re_target, Float im_target,
    Float width_target, Float height_target,
    const Palette& palette)
{
    this->width_px = width_px;
    this->height_px = height_px;
    this->re_target = re_target;
    this->im_target = im_target;
    this->width_target = width_target;
    this->height_target = height_target;
    this->palette = palette;

    this->re_min = this->re_target - this->width_target / 2;
    this->re_max = this->re_target + this->width_target / 2;
    this->im_min = this->im_target - this->height_target / 2;
    this->im_max = this->im_target + this->height_target / 2;
}

template <typename Float, template<typename> class Complex>
void Mandelbrot<Float, Complex>::setComplexParams(
    Float re_target, Float im_target,
    Float width_target, Float height_target)
{
    this->re_target = re_target;
    this->im_target = im_target;
    this->width_target = width_target;
    this->height_target = height_target;

    this->re_min = this->re_target - this->width_target / 2;
    this->re_max = this->re_target + this->width_target / 2;
    this->im_min = this->im_target - this->height_target / 2;
    this->im_max = this->im_target + this->height_target / 2;
}

template <typename Float, template<typename> class Complex>
void Mandelbrot<Float, Complex>::setReTarget(Float re_target) {
    this->re_target = re_target;
}

template <typename Float, template<typename> class Complex>
void Mandelbrot<Float, Complex>::setImTarget(Float im_target) {
    this->im_target = im_target;
}

template <typename Float, template<typename> class Complex>
void Mandelbrot<Float, Complex>::setWidthTarget(Float width_target) {
    this->width_target = width_target;
}

template <typename Float, template<typename> class Complex>
void Mandelbrot<Float, Complex>::setHeightTarget(Float height_target) {
    this->height_target = height_target;
}

template <typename Float, template<typename> class Complex>
int Mandelbrot<Float, Complex>::getMandelCountMax() const {
    return this->mandel_count_max;
}

template <typename Float, template<typename> class Complex>
std::vector<int> Mandelbrot<Float, Complex>::makeCountVector() const {
    std::vector<int> count_vec(this->width_px * this->height_px);
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < this->height_px; y++) {
        for (int x = 0; x < this->width_px; x++) {
            Complex<Float> z = this->getComplexAt(x, y);
            int n = this->mandelCount(z);
            count_vec[y * this->width_px + x] = n;
        }
    }
    return count_vec;
}

template <typename Float, template<typename> class Complex>
std::vector<Color> Mandelbrot<Float, Complex>::makeColorVector() const {
    std::vector<int> count_vec = this->makeCountVector();
    std::vector<Color> color_vec(this->width_px * this->height_px);
    #pragma omp parallel for
    for (size_t i = 0; i < count_vec.size(); i++) {
        color_vec[i] = this->nToColor(count_vec[i]);
    }
    return color_vec;
}


template <typename Float, template<typename> class Complex>
bool Mandelbrot<Float, Complex>::savePNG(const std::string& filename) const {
    std::vector<Color> img = this->makeColorVector();
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        return false;
    }

    // libPNGの初期化
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return false;
    }

    png_set_compression_level(png, PNG_COMPRESSION_TYPE_BASE);

    // 出力先のファイルを設定
    png_init_io(png, fp);

    // 画像情報を設定
    png_set_IHDR(png, info, this->width_px, this->height_px,
                 8,  // 8-bit depth per channel
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // 画像情報をファイルに書き込む
    png_write_info(png, info);

    // 画像データを PNG に変換
    std::vector<png_bytep> row_pointers(this->height_px);
    for (int y = 0; y < this->height_px; ++y) {
        row_pointers[y] = new png_byte[this->width_px * 3];
        for (int x = 0; x < this->width_px; ++x) {
            const Color& pixel = img[y * this->width_px + x];
            row_pointers[y][x * 3]     = static_cast<png_byte>(pixel[0]);  // R
            row_pointers[y][x * 3 + 1] = static_cast<png_byte>(pixel[1]);  // G
            row_pointers[y][x * 3 + 2] = static_cast<png_byte>(pixel[2]);  // B
        }
    }

    // 画像データを書き込み
    png_write_image(png, row_pointers.data());
    png_write_end(png, nullptr);

    // メモリ解放
    for (int y = 0; y < this->height_px; ++y) {
        delete[] row_pointers[y];
    }
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    return true;
}

template <typename Float, template<typename> class Complex>
Complex<Float> Mandelbrot<Float, Complex>::getComplexAt(const int x, const int y) const {
    Float x_f = static_cast<Float>(x);
    Float y_f = static_cast<Float>(y);
    Float width_px_f = static_cast<Float>(width_px);
    Float height_px_f = static_cast<Float>(height_px);
    Float one_f = static_cast<Float>(1.0);
    Float re = (x_f / width_px_f) * re_max + (one_f - x_f / width_px_f) * re_min;
    Float im = (y_f / height_px_f) * im_min + (one_f - y_f / height_px_f) * im_max;

    return Complex<Float>(re, im);
}

template <typename Float, template<typename> class Complex>
int Mandelbrot<Float, Complex>::mandelCount(const Complex<Float>& c) const {
    Complex<Float> z(Float(0.0), Float(0.0));
    int n = 0;

    while (n < mandel_count_max) {
        z = z * z + c;

        // abs(z) を使えば、std::abs や boost::multiprecision::abs が
        // ADL（Argument-Dependent Lookup）で自動的に選ばれます。
        if (abs(z) > Float(2.0)) {
            break;
        }
        n++;
    }

    return n;
}

template <typename Float, template<typename> class Complex>
Color Mandelbrot<Float, Complex>::nToColor(int n) const {
    Float step = Float(this->mandel_count_max / this->palette.size());
    Float left = Float(0.0);

    for (size_t i = 0; i < this->palette.size(); i++) {
        if ((int) (left + step * i) <= n && n <= (int) (left + step * (i + 1))) {
            return palette.at(i);
        }
    }

    return palette.back();
}

template <typename Float, template<typename> class Complex>
std::vector<Float> Mandelbrot<Float, Complex>::calcGradMag() const {
    std::vector<double> mag;
    std::vector<int> n_vec = this->makeCountVector();

    for (int y = 1; y < this->height_px - 1; y++) {
        for (int x = 1; x < this->width_px - 1; x++) {
            Float dx = n_vec.at(y * this->width_px + (x + 1)) - n_vec.at(y * this->width_px + (x - 1));
            Float dy = n_vec.at((y + 1) * this->width_px + x) - n_vec.at((y - 1) * this->width_px + x);
            mag.push_back(sqrt(dx * dx + dy * dy));
        }
    }

    return mag;
}

#endif  // MANDELBROT_HPP
