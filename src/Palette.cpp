// Palette.cpp


#include "Palette.hpp"


// Constructor
Palette::Palette(std::initializer_list<Color> colors) : std::vector<Color>(colors) {}

Palette::Palette(std::vector<Color> colors) : std::vector<Color>(std::move(colors)) {}


// Operations for Palette

// add: add Color
void Palette::add(const Color& color) {
    this->push_back(color);
}

// remove: remove Color
void Palette::remove(const Color& color) {
    this->erase(std::remove(this->begin(), this->end(), color), this->end());
    this->std::vector<Color>::shrink_to_fit();
}

// remove: remove this->at(index)
void Palette::remove(const size_t index) {
    if (index < this->size()) { // 範囲チェック
        this->erase(this->begin() + index);
        this->std::vector<Color>::shrink_to_fit();
    } else {
        throw std::out_of_range("Index out of range");
    }
}

// reverse: reverse palette
void Palette::reverse() {
    std::reverse(this->begin(), this->end());
}

// clear: clear all Colors, free memory
void Palette::clear() {
    this->std::vector<Color>::clear();
    this->std::vector<Color>::shrink_to_fit();
}

// contains: Palette contains the Color?
bool Palette::contains(const Color& color) const {
    return std::find(begin(), end(), color) != end();
}

// dedupe: dedupulication of the same Colors
/*
void Palette::dedupe() {
    this->sortByRGB();
    auto last = std::unique(this->begin(), this->end());
    this->erase(last, this->end());
    this->shrink_to_fit();
}
*/


// Sort

// sortByRGB: sort by RGB
void Palette::sortByRGB() {
    std::sort(this->begin(), this->end(), [](const Color& a, const Color& b) {
        if (a.getR() != b.getR()) return a.getR() < b.getR();
        if (a.getG() != b.getG()) return a.getG() < b.getG();
        return a.getB() < b.getB();
    });
}

// sortByHue: sort by Hue
void Palette::sortByHue() {
    std::sort(this->begin(), this->end(), [](const Color& a, const Color& b) {
        double hueA = a.toHSV()[0];
        double hueB = b.toHSV()[0];

        // H の周期性を考慮して距離を比較
        double diff = std::fmod(hueA - hueB + 360.0, 360.0);
        return diff < 180.0; // 180°以内の範囲で比較することで、適切なソート順になる
    });
}

// sortByLuminance: sort by luminance
void Palette::sortByLuminance() {
    std::sort(this->begin(), this->end(), [](const Color& a, const Color& b) {
        return a.getLuminance() < b.getLuminance();
    });
}


// make palette

// makeGradationHue: Hmin から Hmax の値に基づいて n 色のグラデーションを作成
Palette Palette::makeGradationHue(size_t n, double Hmin, double Hmax, double S, double B) {
    Palette p;
    double df = (Hmax - Hmin) / (n - 1); // n点必要なら (n-1) 分割
    for (size_t i = 0; i < n; ++i) {
        double H = Hmin + i * df;
        p.add(Color::fromHSV(H, S, B));
    }
    if (p.size() > n) {
        while (p.size() != n) {
            p.remove(p.back());
        }
    } else if (p.size() < n) {
        while (p.size() != n) {
            p.add(p.back());
        }
    }

    return p;
}

// makeGradationSat: Smin から Smax の値に基づいて n 色のグラデーションを作成
Palette Palette::makeGradationSat(size_t n, double H, double Smin, double Smax, double B) {
    Palette p;
    double df = (Smax - Smin) / (n - 1); // n点必要なら (n-1) 分割
    for (size_t i = 0; i < n; ++i) {
        double S = Smin + i * df;
        p.add(Color::fromHSV(H, S, B));
    }
    if (p.size() > n) {
        while (p.size() != n) {
            p.remove(p.back());
        }
    } else {
        while (p.size() != n) {
            p.add(p.back());
        }
    }

    return p;
}

// makeGradationBri: Bmin から Bmax の値に基づいて n 色のグラデーションを作成
Palette Palette::makeGradationBri(size_t n, double H, double S, double Bmin, double Bmax) {
    Palette p;
    double df = (Bmax - Bmin) / (n - 1);
    for (size_t i = 0; i < n; ++i) {
        double B = Bmin + i * df;
        p.add(Color::fromHSV(H, S, B));
    }
    if (p.size() > n) {
        while (p.size() != n) {
            p.remove(p.back());
        }
    } else {
        while (p.size() != n) {
            p.add(p.back());
        }
    }

    return p;
}


// makeGrayScale: n 階調のグレイスケールパレット
Palette Palette::makeGrayScale(size_t n) {
    int step = (size_t) (256 / n);
    Palette p;

    for (size_t i = 0; i < n; i++) {
        p.add(Color(step * i, step * i, step * i));
    }
    p.reverse();

    if (p.size() > n) {
        while (p.size() != n) {
            p.remove(p.back());
        }
    } else {
        while (p.size() != n) {
            p.add(p.back());
        }
    }

    return p;
}


// Operator overload

// << output
std::ostream& operator<<(std::ostream& os, const Palette& palette) {
    for (const auto& color : palette) {
        os << color << " ";
    }
    return os;
}

// >> input
std::istream& operator>>(std::istream& is, Palette& palette) {
    Color color;
    while (is >> color) {
        palette.add(color);
    }
    return is;
}
