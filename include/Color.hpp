// Color.hpp


#ifndef COLOR_HPP
#define COLOR_HPP


#include <iostream>
#include <array>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <functional>


class Color : public std::array<int, 3> {
    public:
    // Constructor
    Color() = default;

    Color(std::initializer_list<int> list);

    Color(int R, int G, int B);


    // Getter
    int getR() const;
    int getG() const;
    int getB() const;
    std::array<int, 3> getRGB() const;


    // Setter
    void setR(int R);
    void setG(int G);
    void setB(int B);
    void setRGB(int R, int G, int B);


    // Operations for Color

    // clamp: set values from 0 to 255
    void clamp();

    // invert: invert colors
    void invert();

    // getGrayscale: return grayscale value
    int getGrayscale() const;

    // setGrayscale: set R, G, B to grayscale value
    void setGrayscale();

    // getLuminance: get luminance by a formula
    double getLuminance() const;

    // darken/brighten: darken or brighten color by factor*100 %, factor = 0~1
    void darken(double factor);

    void lighten(double factor);


    // Transformation

    // HSV -> RGB transformation
    static Color fromHSV(double H, double S, double V);

    // RGB -> HSV transformation
    std::array<double, 3> toHSV() const;

    // Hex (#000000) -> RGB transformation
    static Color fromHex(const std::string& hex);

    // RGB -> Hex (#000000) transformation
    std::string toHex() const;

    // Operator overload
    bool operator==(const Color& other) const;

    bool operator!=(const Color& other) const;

    Color operator+(const Color& other) const;

    Color operator-(const Color& other) const;

    Color operator*(double factor) const;

    friend std::ostream& operator<<(std::ostream& os, const Color& color);

    friend std::istream& operator>>(std::istream& is, Color& color);


    private:
    
};


// specialize std::hash<Color>
// to use hash containers e.g. std::unordered_set
namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& c) const {
            return (static_cast<size_t>(c.getR()) << 16) |
                   (static_cast<size_t>(c.getG()) << 8)  |
                   static_cast<size_t>(c.getB());
        }
    };
}


#endif  // COLOR_HPP
