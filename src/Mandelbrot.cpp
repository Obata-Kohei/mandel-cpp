#include "Mandelbrot.hpp"

void Mandelbrot::setAllParams(
    size_t precision,
    size_t width_px, size_t height_px,
    const Float& re_target, const Float& im_target,
    const Float& width_target, const Float& height_target,
    size_t mandel_count_max,
    const Palette& palette
) {
    this->setPrecision(precision);
    this->setWidthPx(width_px);
    this->setHeightPx(height_px);
    this->setReTarget(re_target);
    this->setImTarget(im_target);
    this->setWidthTarget(width_target);
    this->setHeightTarget(height_target);
    this->setMandelCountMax(mandel_count_max);
    this->setPalette(palette);

    this->updateComplexRange();
}

void Mandelbrot::setComplexParams(
    const Float& re_target, const Float& im_target,
    const Float& width_target, const Float& height_target
) {
    this->setReTarget(re_target);
    this->setImTarget(im_target);
    this->setWidthTarget(width_target);
    this->setHeightTarget(height_target);

    this->updateComplexRange();
}

// Setter
void Mandelbrot::setPrecision(size_t precision) {
    this->precision = precision;
    boost::multiprecision::mpfr_float::default_precision(this->precision);
}

void Mandelbrot::setWidthPx(size_t width_px) {
    this->width_px = width_px;
}

void Mandelbrot::setHeightPx(size_t height_px) {
    this->height_px = height_px;
}

void Mandelbrot::setReTarget(const Float& re_target) {
    this->re_target = re_target;
    this->updateComplexRange();
}

void Mandelbrot::setImTarget(const Float& im_target) {
    this->im_target = im_target;
    this->updateComplexRange();
}

void Mandelbrot::setWidthTarget(const Float& width_target) {
    this->width_target = width_target;
    this->updateComplexRange();
}

void Mandelbrot::setHeightTarget(const Float& height_target) {
    this->height_target = height_target;
    this->updateComplexRange();
}

void Mandelbrot::setMandelCountMax(size_t mandel_count_max) {
    this->mandel_count_max = mandel_count_max;
}

void Mandelbrot::setPalette(const Palette& palette) {
    this->palette = palette;
}

// Getter
size_t Mandelbrot::getPrecision() const {
    return this->precision;
}

size_t Mandelbrot::getWidthPx() const {
    return this->width_px;
}

size_t Mandelbrot::getHeightPx() const {
    return this->height_px;
}

Float Mandelbrot::getReTarget() const {
    return this->re_target;
}

Float Mandelbrot::getImTarget() const {
    return this->im_target;
}

Float Mandelbrot::getWidthTarget() const {
    return this->width_target;
}

Float Mandelbrot::getHeightTarget() const {
    return this->height_target;
}

Palette Mandelbrot::getPalette() const {
    return this->palette;
}

size_t Mandelbrot::getMandelCountMax() const {
    return this->mandel_count_max;
}

std::vector<size_t> Mandelbrot::makeCountVector() const {
    std::vector<size_t> count_vec(this->width_px * this->height_px);
    #pragma omp parallel for collapse(2)
    for (size_t y = 0; y < this->height_px; y++) {
        for (size_t x = 0; x < this->width_px; x++) {
            Complex z = this->getComplexAt(x, y);
            size_t n = this->mandelCount(z);
            count_vec[y * this->width_px + x] = n;
        }
    }
    return count_vec;
}

std::vector<Color> Mandelbrot::makeColorVector(bool eq_hist) const {
    std::vector<size_t> count_vec = this->makeCountVector();
    std::vector<Color> color_vec(this->width_px * this->height_px);

    std::vector<size_t> hist = this->nHist();
    std::vector<size_t> cdf = this->nCdf();
    size_t total = cdf.back();
    std::vector<Float> brightness_table(cdf.size());
    for (size_t i = 0; i < cdf.size(); ++i) {
        brightness_table[i] = Float(cdf[i]) / Float(total);  // 値は [0.0, 1.0]
    }

    #pragma omp parallel for
    for (size_t i = 0; i < count_vec.size(); i++) {
        if (eq_hist) color_vec[i] = this->nToColor_EqHist(count_vec[i], brightness_table);
        else this->nToColor(count_vec[i]);
    }
    return color_vec;
}

void Mandelbrot::updateComplexRange() {
    this->re_min = this->re_target - this->width_target / 2;
    this->re_max = this->re_target + this->width_target / 2;
    this->im_min = this->im_target - this->height_target / 2;
    this->im_max = this->im_target + this->height_target / 2;
}

Complex Mandelbrot::getComplexAt(const size_t x, const size_t y) const {
    Float x_f = static_cast<Float>(x);
    Float y_f = static_cast<Float>(y);
    Float width_px_f = static_cast<Float>(width_px);
    Float height_px_f = static_cast<Float>(height_px);
    Float one_f = static_cast<Float>(1.0);
    Float re = (x_f / width_px_f) * re_max + (one_f - x_f / width_px_f) * re_min;
    Float im = (y_f / height_px_f) * im_min + (one_f - y_f / height_px_f) * im_max;

    return Complex(re, im);
}

size_t Mandelbrot::mandelCount(const Complex& c) const {
    Complex z(Float(0.0), Float(0.0));
    size_t n = 0;

    while (n < this->mandel_count_max) {
        z = z * z + c;

        // abs(z) を使えば、std::abs や boost::multiprecision::abs が
        // ADL（Argument-Dependent Lookup）で自動的に選ばれます。
        if (abs(z) > Float(2.0)) {
            break;
        }
        n++;
    }

    return n;

    // Smooth coloring
    /*
    if (n == mandel_count_max) {
        return n; // 集合内：整数で返す or 特別に扱う
    }
    Float log_zn = log(abs(z));
    size_t nu = (size_t) (n + 1 - log(log_zn) / log(2.0));
    return nu;
    */
}

Color Mandelbrot::nToColor(size_t n) const {
    Float step = Float(this->mandel_count_max / this->palette.size());
    Float left = Float(0.0);

    for (size_t i = 0; i < this->palette.size(); i++) {
        if ((size_t) (left + step * i) <= n && n <= (size_t) (left + step * (i + 1))) {
            return palette.at(i);
        }
    }

    return palette.back();
}

Color Mandelbrot::nToColor_EqHist(size_t n, std::vector<Float> brightness_table) const {
    if (n >= this->mandel_count_max) {
        return Color(0, 0, 0);
    }
    Float brightness = brightness_table[n];  // ← nに対応する明るさ
    size_t idx = size_t(brightness * (palette.size() - 1));
    return palette.at(idx);
}

std::vector<size_t> Mandelbrot::nHist() const {
    std::vector<size_t> hist(mandel_count_max + 1, 0);
    std::vector<size_t> n_vec = this->makeCountVector();

    int n_threads = omp_get_max_threads();
    std::vector<std::vector<size_t>> local_hists(n_threads, std::vector<size_t>(mandel_count_max + 1, 0));

    // 各スレッドでローカルヒストグラムを作成
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        std::vector<size_t>& local = local_hists[tid];

        #pragma omp for
        for (size_t i = 0; i < n_vec.size(); ++i) {
            size_t n = n_vec[i];
            if (n <= mandel_count_max) {
                local[n]++;
            }
        }
    }

    // ローカルヒストグラムを統合
    for (int t = 0; t < n_threads; ++t) {
        for (size_t n = 0; n <= mandel_count_max; ++n) {
            hist[n] += local_hists[t][n];
        }
    }

    return hist;
}

std::vector<size_t> Mandelbrot::nCdf() const {
    std::vector<size_t> hist = this->nHist();
    std::vector<size_t> cdf(hist.size(), 0);

    cdf[0] = hist[0];
    for (size_t i = 1; i < hist.size(); i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    return cdf;
}

std::vector<Float> Mandelbrot::calcGradMag() const {
    std::vector<Float> mag;
    std::vector<size_t> n_vec = this->makeCountVector();

    for (size_t y = 1; y < this->height_px - 1; y++) {
        for (size_t x = 1; x < this->width_px - 1; x++) {
            Float dx = n_vec.at(y * this->width_px + (x + 1)) - n_vec.at(y * this->width_px + (x - 1));
            Float dy = n_vec.at((y + 1) * this->width_px + x) - n_vec.at((y - 1) * this->width_px + x);
            mag.push_back(sqrt(dx * dx + dy * dy));
        }
    }

    return mag;
}
