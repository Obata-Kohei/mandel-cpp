#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <vector>
#include <cmath>
#include <png.h>
#include <omp.h>
#include "Color.hpp"
#include "Palette.hpp"
#include "types.hpp"

class Mandelbrot {
    private:
    size_t precision;  // 浮動小数点数の精度, mpfr使用
    size_t width_px, height_px;  // 出力画像の解像度
    Float re_target, im_target, width_target, height_target;  // 複素数平面上のどの座標・範囲を描画するか
    Palette palette;  // パレット

    Float re_min, re_max;
    Float im_min, im_max;
    size_t mandel_count_max;  // 発散回数を計算するときの上限回数


    public:
    Mandelbrot() = default;

    // 全てのパラメタの設定
    void setAllParams(
        size_t precision,
        size_t width_px, size_t height_px,
        const Float& re_target, const Float& im_target,
        const Float& width_target, const Float& height_target,
        size_t mandel_count_max=300,
        const Palette& palette=Palette::makeGrayScale(256)
    );

    // 複素平面上のパラメタを設定
    void setComplexParams(
        const Float& re_target, const Float& im_target,
        const Float& width_target, const Float& height_target
    );

    // Setter
    void setPrecision(size_t precision);
    void setWidthPx(size_t width_px);
    void setHeightPx(size_t height_px);
    void setReTarget(const Float& re_target);
    void setImTarget(const Float& im_target);
    void setWidthTarget(const Float& width_target);
    void setHeightTarget(const Float& height_target);
    void setPalette(const Palette& palette);
    void setMandelCountMax(size_t mandel_count_max);

    // Getter
    size_t getPrecision() const;
    size_t getWidthPx() const;
    size_t getHeightPx() const;
    Float getReTarget() const;
    Float getImTarget() const;
    Float getWidthTarget() const;
    Float getHeightTarget() const;
    Palette getPalette() const;
    size_t getMandelCountMax() const;

    // ラスタースキャン順の height_px * width_px サイズのvector.
    // mandelCountの結果を格納する
    std::vector<size_t> makeCountVector() const;

    // ラスタースキャン順の height_px * width_px サイズのvector.
    // nToColorの結果を格納する
    std::vector<Color> makeColorVector(bool eq_hist) const;


    private:
    // re_min, im_maxなどの複素数平面上での描画範囲を更新
    void updateComplexRange();

    // 画像上の座標x, yから対応する複素平面上の複素数を得る
    Complex getComplexAt(const size_t x, const size_t y) const;

    // abs(z)が初めて2を超えるnを計算する
    size_t mandelCount(const Complex& c) const;

    // mandelCountの結果nからpalette中の⾊を決めるstatic method
    Color nToColor(size_t n) const;

    // ヒストグラム平坦化を用いてnからColorを決定。コントラストが上がる
    Color nToColor_EqHist(size_t n, std::vector<Float> brightness_table) const;

    // 発散にかかる回数nのヒストグラム
    std::vector<size_t> nHist() const;

    // 発散にかかる回数nの累積分布(CDF)
    std::vector<size_t> nCdf() const;

    // 発散にかかる回数 n の勾配のマグニチュードを，makeCountVectorの結果から計算
    std::vector<Float> calcGradMag() const;
};

#endif  // MANDELBROT_HPP
