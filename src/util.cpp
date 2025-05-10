#include "util.hpp"

void omp_info()
{
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

// PNGファイルの作成
bool savePNG(const std::string& filename, const std::vector<Color>& img, size_t width_px, size_t height_px)
{
    if (img.size() != width_px * height_px) return false;

    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) return false;

    // libPNGの初期化
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
    {
        fclose(fp);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return false;
    }

    png_set_compression_level(png, PNG_COMPRESSION_TYPE_BASE);

    // 出力先のファイルを設定
    png_init_io(png, fp);

    // 画像情報を設定
    png_set_IHDR(png, info, width_px, height_px,
                 8,  // 8-bit depth per channel
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // 画像情報をファイルに書き込む
    png_write_info(png, info);

    // 画像データを PNG に変換
    std::vector<png_bytep> row_pointers(height_px);
    for (size_t y = 0; y < height_px; ++y)
    {
        row_pointers[y] = new png_byte[width_px * 3];
        for (size_t x = 0; x < width_px; ++x)
        {
            const Color& pixel = img[y * width_px + x];
            row_pointers[y][x * 3]     = static_cast<png_byte>(pixel[0]);  // R
            row_pointers[y][x * 3 + 1] = static_cast<png_byte>(pixel[1]);  // G
            row_pointers[y][x * 3 + 2] = static_cast<png_byte>(pixel[2]);  // B
        }
    }

    // 画像データを書き込み
    png_write_image(png, row_pointers.data());
    png_write_end(png, nullptr);

    // メモリ解放
    for (size_t y = 0; y < height_px; ++y)
    {
        delete[] row_pointers[y];
    }
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    return true;
}