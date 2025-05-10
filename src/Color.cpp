// Color.cpp


#include "Color.hpp"


// Constructor
Color::Color(int R, int G, int B) {
    (*this)[0] = R;
    (*this)[1] = G;
    (*this)[2] = B;
}

Color::Color(std::initializer_list<int> list) {
    if (list.size() != 3) {  // 例外を発生しうる
        throw std::invalid_argument("Color requires exactly 3 values (R, G, B)");
    }
    auto it = list.begin();
    (*this)[0] = *it++;
    (*this)[1] = *it++;
    (*this)[2] = *it;
}


// Getter
int Color::getR() const { return (*this)[0]; }
int Color::getG() const { return (*this)[1]; }
int Color::getB() const { return (*this)[2]; }
std::array<int, 3> Color::getRGB() const { return {(*this)[0], (*this)[1], (*this)[2]}; }


// Setter
void Color::setR(int R) { (*this)[0] = R; (*this).clamp(); }
void Color::setG(int G) { (*this)[1] = G; (*this).clamp(); }
void Color::setB(int B) { (*this)[2] = B; (*this).clamp(); }
void Color::setRGB(int R, int G, int B) { (*this) = {R, G, B}; (*this).clamp(); }


// Operation for Color

// clamp: set the value from 0 to 255.
void Color::clamp() {
    for (int& v : *this) {
        v = std::max(0, std::min(255, v));
    }
}

// invert: invert color
void Color::invert() {
    for (int& v : *this) {
        v = 255 - v;
    }
}

// getGrayscale: return grayscale value
int Color::getGrayscale() const {
    return static_cast<int>(0.299 * this->getR() + 0.587 * this->getG() + 0.114 * this->getB());
}

// setGrayscale: set R, G, B to grayscale value
void Color::setGrayscale() {
    int gray = this->getGrayscale();
    this->setR(gray);
    this->setG(gray);
    this->setB(gray);
}

// getLuminance: get luminance by a formula
double Color::getLuminance() const {
    return 0.2126 * this->getR() + 0.7152 * this->getG() + 0.0722 * this->getB();
}

// darken/brighten: darken or brighten color by factor*100 %, factor=0~1
void Color::darken(double factor) {
    for (int& v : *this) {
        v = static_cast<int>(v * (1.0 - factor));
    }
    this->clamp();
}

void Color::lighten(double factor) {
    for (int& v : *this) {
        v = static_cast<int>(v + (255 - v) * factor);
    }
    this->clamp();
}


// Transformation

// fromHSV: HSV -> RGB transformation. 0 < H < 360
Color Color::fromHSV(double H, double S, double V) {
    double C = V * S;
    double X = C * (1 - std::fabs(fmod(H / 60.0, 2) - 1));
    double m = V - C;

    double r = 0, g = 0, b = 0;
    if (0 <= H && H < 60) {
        r = C, g = X, b = 0;
    } else if (60 <= H && H < 120) {
        r = X, g = C, b = 0;
    } else if (120 <= H && H < 180) {
        r = 0, g = C, b = X;
    } else if (180 <= H && H < 240) {
        r = 0, g = X, b = C;
    } else if (240 <= H && H < 300) {
        r = X, g = 0, b = C;
    } else if (300 <= H && H < 360) {
        r = C, g = 0, b = X;
    }

    int R = static_cast<int>((r + m) * 255);
    int G = static_cast<int>((g + m) * 255);
    int B = static_cast<int>((b + m) * 255);

    return Color(R, G, B);
}

// toHSV: RGB -> HSV transformation
std::array<double, 3> Color::toHSV() const {
    double R = this->getR() / 255.0;
    double G = this->getG() / 255.0;
    double B = this->getB() / 255.0;

    double maxVal = std::max({R, G, B});
    double minVal = std::min({R, G, B});
    double delta = maxVal - minVal;

    double H = 0;
    if (delta > 0) {
        if (maxVal == R) {
            H = 60 * fmod((G - B) / delta, 6);
        } else if (maxVal == G) {
            H = 60 * ((B - R) / delta + 2);
        } else if (maxVal == B) {
            H = 60 * ((R - G) / delta + 4);
        }
    }
    if (H < 0) H += 360;

    double S = (maxVal == 0) ? 0 : delta / maxVal;
    double V = maxVal;

    return {H, S, V};
}

// fromHex: Hex -> RGB transformation
Color Color::fromHex(const std::string& hex) {
    int r, g, b;
    if (hex[0] == '#' && hex.length() == 7) {
        sscanf(hex.c_str(), "#%02X%02X%02X", &r, &g, &b);
        return Color(r, g, b);
    }
    throw std::invalid_argument("Invalid hex color format");
}

// toHex: RGB -> Hex transformation
std::string Color::toHex() const {
    char hex[8];
    snprintf(hex, sizeof(hex), "#%02X%02X%02X", this->getR(), this->getG(), this->getB());
    return std::string(hex);
}


// Operator overload

// ==
bool Color::operator==(const Color& other) const {
    return getRGB() == other.getRGB();
}

// !=
bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

// +
Color Color::operator+(const Color& other) const {
    return Color(
        std::clamp(getR() + other.getR(), 0, 255),
        std::clamp(getG() + other.getG(), 0, 255),
        std::clamp(getB() + other.getB(), 0, 255)
    );
}

// -
Color Color::operator-(const Color& other) const {
    return Color(
        std::clamp(getR() - other.getR(), 0, 255),
        std::clamp(getG() - other.getG(), 0, 255),
        std::clamp(getB() - other.getB(), 0, 255)
    );
}

// *
Color Color::operator*(double factor) const {
    return Color(
        std::clamp(static_cast<int>(getR() * factor), 0, 255),
        std::clamp(static_cast<int>(getG() * factor), 0, 255),
        std::clamp(static_cast<int>(getB() * factor), 0, 255)
    );
}

// << output
std::ostream& operator<<(std::ostream &os, const Color &color) {
    os << "RGB(" << color.getR() << ", " << color.getG() << ", " << color.getB() << ")";
    return os;
}

// >> input
std::istream& operator>>(std::istream& is, Color& color) {
    int r, g, b;
    char ch;  // カンマや括弧を読み飛ばすために使う

    // 入力が "(R, G, B)" の形式かどうかを確認
    if (is >> ch && ch == '(') {
        if (is >> r >> ch && ch == ',' &&   // R の後にカンマが来る
            is >> g >> ch && ch == ',' &&   // G の後にカンマが来る
            is >> b >> ch && ch == ')') {   // B の後に閉じ括弧が来る
            color.setRGB(r, g, b);  // 入力値で Color オブジェクトを設定
        } else {
            is.setstate(std::ios::failbit);  // フォーマットが間違っている場合はエラー
        }
    } else {
        is.setstate(std::ios::failbit);  // 入力が "(" で始まらない場合はエラー
    }

    return is;
}
