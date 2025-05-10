// Palette.hpp


#ifndef PALETTE_HPP
#define PALETTE_HPP


#include "Color.hpp"
#include <vector>
#include <array>
#include <set>
#include <algorithm>
#include <iostream>
#include <cmath>


class Palette : public std::vector<Color> {
    public:
    // Constructor
    Palette() = default;

    Palette(std::initializer_list<Color> colors);

    Palette(std::vector<Color> colors);


    // Operations for Palette

    // add: add Color
    void add(const Color& color);

    // remove: remove Color
    void remove(const Color& color);

    // remove: remove this->at(index)
    void remove(const size_t index);

    // reverse: reverse palette
    void reverse();

    // clear: remove all
    void clear();

    // contains: Palette contains the Color?
    bool contains(const Color& color) const;

    // dedupe: dedupulication of the same Colors
    void dedupe();


    // Sort

    // sortByRGB: sort by RGB
    void sortByRGB();

    // sortByHue: sort by Hue, 色相
    void sortByHue();

    // sortByLuminance: sort by luminance, 輝度
    void sortByLuminance();


    // make Palette

    // makeGradationHue: Hmin から Hmax の値に基づいて n 色のグラデーションを作成
    static Palette makeGradationHue(size_t n, double Hmin, double Hmax, double S, double B);

    // makeGradationSat: Smin から Smax の値に基づいて n 色のグラデーションを作成
    static Palette makeGradationSat(size_t n, double H, double Smin, double Smax, double B);

    // makeGradationBri: Bmin から Bmax の値に基づいて n 色のグラデーションを作成
    static Palette makeGradationBri(size_t n, double H, double S, double Bmin, double Bmax);

    // makeGrayScale: n 階調のグレイスケールパレット
    static Palette makeGrayScale(size_t n);


    // IO operators
    friend std::ostream& operator<<(std::ostream& os, const Palette& palette);
    friend std::istream& operator>>(std::istream& is, Palette& palette);


    private:

};

#endif // PALETTE_HPP