#include "align.h"
#include <string>

using std::string;
using std::cout;
using std::endl;

Image transform(Image src, int first_color, int second_color, int delta_i, int delta_j)
{
    Image res(src.n_rows, src.n_cols);

    for (int i = 0; i < int(src.n_rows); i++) {
        if (i < delta_i || i - delta_i > int(src.n_rows) - 1) { continue; }
        for (int j = 0; j < int(src.n_cols); j++) {
            if (j < delta_j || j - delta_j > int(src.n_cols) - 1) { continue; }

            auto new_pixel = std::make_tuple(std::get<0>(src(i, j)), std::get<1>(src(i, j)), std::get<2>(src(i, j)));

            switch(second_color) {
                case 0:
                    std::get<0>(new_pixel) = std::get<0>(src(i - delta_i, j - delta_j));
                    break;
                case 1:
                    std::get<1>(new_pixel) = std::get<1>(src(i - delta_i, j - delta_j));
                    break;
                case 2:
                    std::get<2>(new_pixel) = std::get<2>(src(i - delta_i, j - delta_j));
                default:
                    break;
            }

            res(i, j) = new_pixel;
        }
    }
    return res;
}

long long mse(Image src, int first_color, int second_color, int delta_i, int delta_j)
{
    long long res = 0;

    for (int i = src.n_rows * 0.05; i < int(src.n_rows * 0.95); i++) {
        if (i < delta_i || i - delta_i > int(src.n_rows) - 1) { continue; }
        for (int j = src.n_cols * 0.05; j < int(src.n_cols * 0.95); j++) {
            if (j < delta_j || j - delta_j > int(src.n_cols) - 1) { continue; }

            long long part = 0;

            switch(first_color) {
                case 0:
                    part += std::get<0>(src(i , j));
                    break;
                case 1:
                    part += std::get<1>(src(i , j));
                    break;
                case 2:
                    part += std::get<2>(src(i , j));
                default:
                    break;
            }

            switch(second_color) {
                case 0:
                    part -= std::get<0>(src(i - delta_i, j - delta_j));
                    break;
                case 1:
                    part -= std::get<1>(src(i - delta_i, j - delta_j));
                    break;
                case 2:
                    part -= std::get<2>(src(i - delta_i, j - delta_j));
                default:
                    break;
            }

            res += pow(part, 2);
        }
    }

    res /= src.n_cols * src.n_rows;
    return res;
}

long long cc(Image src, int first_color, int second_color, int delta_i, int delta_j)
{
    long long res = 0;

    for (int i = src.n_rows * 0.05; i < int(src.n_rows * 0.95); i++) {
        if (i < delta_i || i - delta_i > int(src.n_rows) - 1) { continue; }
        for (int j = src.n_cols * 0.05; j < int(src.n_cols * 0.95); j++) {
            if (j < delta_j || j - delta_j > int(src.n_cols) - 1) { continue; }

            long long part = 0;

            switch(first_color) {
                case 0:
                    part = std::get<0>(src(i , j));
                    break;
                case 1:
                    part = std::get<1>(src(i , j));
                    break;
                case 2:
                    part = std::get<2>(src(i , j));
                default:
                    break;
            }

            switch(second_color) {
                case 0:
                    part *= std::get<0>(src(i - delta_i , j - delta_j));
                    break;
                case 1:
                    part *= std::get<1>(src(i - delta_i , j - delta_j));
                    break;
                case 2:
                    part *= std::get<2>(src(i - delta_i , j - delta_j));
                default:
                    break;
            }
            res += part;
        }
    }
    return res;
}

Image mirror(Image srcImage, int radius) {
    Image dstImage(srcImage.n_rows + 2 * radius, srcImage.n_cols + 2 * radius);

    for (int i = 0; i < int(srcImage.n_rows); i++) {
        for (int j = 0; j < int(srcImage.n_cols); j++) {
            dstImage(i + radius, j + radius) = srcImage(i, j);
        }
    }

    // up
    for (int i = 0; i < radius; i++) {
        for (int j = radius; j < int(srcImage.n_cols) + radius; j ++) {
            dstImage(radius - i - 1, j) = dstImage(radius + i,j);
        }
    }

    // down
    for (int i = 0; i < radius; i++) {
        for (int j = radius; j < int(srcImage.n_cols) + radius; j ++) {
            dstImage(radius + srcImage.n_rows + i, j) = dstImage(radius + srcImage.n_rows - i - 1, j);
        }
    }

    // left
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < int(srcImage.n_rows) + radius * 2; j ++) {
            dstImage(j, radius - i - 1) = dstImage(j, radius + i);
        }
    }

    // right
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < int(srcImage.n_rows) + 2 * radius; j ++) {
            dstImage(j, radius + srcImage.n_cols + i) = dstImage(j, radius + srcImage.n_cols - i - 1);
        }
    }

    return dstImage;
}

//I am using mse
Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror,
            bool isInterp, bool isSubpixel, double subScale)
{
    if (isSubpixel == true) {
        srcImage = resize(srcImage, subScale);
    }

    for (int f_color = 0; f_color < 2; f_color++) {
        int first_color = f_color % 3;
        int second_color = (f_color + 1) % 3;
        long long min_mse = mse(srcImage, first_color, second_color, 0, 0);
        int min_i = 0, min_j = 0;
        // long long max_cc = cc(srcImage, first_color, second_color, 0, 0);
        // int max_i = 0, max_j = 0;
        int val = 15 * srcImage.n_cols  / 500.0;

        for (int i = -val; i <= val; i++) {
            for (int j = -val; j <= val; j++) {
                long long curr_mse = mse(srcImage, first_color, second_color, i, j);
                // long long curr_cc = cc(srcImage, first_color, second_color, i, j);
                if (curr_mse <= min_mse) {
                    min_mse = curr_mse;
                    min_i = i;
                    min_j = j;
                }
                // if (curr_cc >= max_cc) {
                //     max_cc = curr_cc;
                //     max_i = i;
                //     max_j = j;
                // }
            }
        }
        srcImage = transform(srcImage, first_color, second_color, min_i, min_j);
    }

    if (isSubpixel == true) {
        srcImage = resize(srcImage, 1.0 / subScale);
    }
    if (isPostprocessing == true) {
        if (postprocessingType == "--gray-world") {
            srcImage = gray_world(srcImage);
        }
        if (postprocessingType == "--unsharp") {
            if (isMirror == true) {
                srcImage = mirror(srcImage, 1);
            }
            srcImage = unsharp(srcImage);
            if (isMirror == true) {
                srcImage = srcImage.submatrix(1, 1, srcImage.n_rows - 2, srcImage.n_cols - 2);
            }
        }
        if (postprocessingType == "--autocontrast") {
            srcImage = autocontrast(srcImage, fraction);
        }
        if (postprocessingType == "--white-balance") {
            if (isMirror == true) {
                srcImage = mirror(srcImage, 1);
            }
            srcImage = white_balance(srcImage);
            if (isMirror == true) {
                srcImage = srcImage.submatrix(1, 1, srcImage.n_rows - 2, srcImage.n_cols - 2);
            }
        }
    }

    return srcImage;
}

Image sobel_x(Image src_image) {
    Matrix<double> kernel = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
    return custom(src_image, kernel);
}

Image sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel);
}

Image unsharp(Image src_image) {
    Matrix<double> kernel = {{ -1.0 / 6,  -2.0 / 3,  -1.0 / 6},
                             { -2.0 / 3,  13.0 / 3,  -2.0 / 3},
                             { -1.0 / 6,  -2.0 / 3,  -1.0 / 6}};
    return custom(src_image, kernel);
}

Image gray_world(Image src_image) {
    double s, s_r = 0, s_b = 0, s_g = 0;
    int r, g, b;

    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            s_r += std::get<0>(src_image(i, j));
            s_g += std::get<1>(src_image(i, j));
            s_b += std::get<2>(src_image(i, j));
        }
    }

    s_r /= src_image.n_cols * src_image.n_rows;
    s_g /= src_image.n_cols * src_image.n_rows;
    s_b /= src_image.n_cols * src_image.n_rows;
    s = (s_r + s_b + s_g) / 3;

    for (int i = 0; i < int(src_image.n_rows); i++) {
        for (int j = 0; j < int(src_image.n_cols); j++) {
            std::tie(r, g, b) = src_image(i, j);
            r *= s / s_r;
            if (r > 255) { r = 255; }
            if (r < 0) { r = 0; }
            g *= s / s_g;
            if (g > 255) { g = 255; }
            if (g < 0) { g = 0; }
            b *= s / s_b;
            if (b > 255) { b = 255; }
            if (b < 0) { b = 0; }
            src_image(i, j) = std::make_tuple(r, g, b);
        }
    }

    return src_image;
}

Image resize(Image src_image, double scale) {
    Image dst_image(src_image.n_rows * scale, src_image.n_cols * scale);

    if (scale < 1) {
        for(uint i = 0; i < dst_image.n_rows; i++) {
            for(uint j = 0; j < dst_image.n_cols; j++) {
                dst_image(i, j) = src_image(ceil(i / scale), ceil(j / scale));
            }
        }
    } else if (scale > 1) {
        for(uint i = 0; i < dst_image.n_rows; i++) {
            for(uint j = 0; j < dst_image.n_cols; j++) {
                dst_image(i, j) = std::make_tuple(0, 0, 0);
            }
        }

        for(uint i = 0; i < src_image.n_rows; i++) {
            for(uint j = 0; j < src_image.n_cols; j++) {
                dst_image(ceil(i * scale), ceil(j * scale)) = src_image(i, j);
            }
        }
        src_image = mirror(src_image, 1);

        for(uint i = 0; i < dst_image.n_rows; i++) {
            for(uint j = 0; j < dst_image.n_cols; j++) {
                double r, g, b;
                std::tie(r, g, b) = dst_image(i, j);
                if (r < 0.0000001 && g < 0.0000001 && b < 0.0000001) {
                    r = g = b = 0;
                    double rr, gg, bb;
                    int x = trunc(i / scale) + 1;
                    int y = trunc(j / scale) + 1;
                    std::tie(rr, gg, bb) = src_image(x, y);
                    r += rr;
                    g += gg;
                    b += bb;
                    std::tie(rr, gg, bb) = src_image(x + 1, y);
                    r += rr;
                    g += gg;
                    b += bb;
                    std::tie(rr, gg, bb) = src_image(x, y + 1);
                    r += rr;
                    g += gg;
                    b += bb;
                    std::tie(rr, gg, bb) = src_image(x + 1, y + 1);
                    r += rr;
                    g += gg;
                    b += bb;
                    r /= 4;
                    g /= 4;
                    b /= 4;
                    dst_image(i, j) = std::make_tuple(r, g, b);
                }
            }
        }
    } else {
        dst_image = src_image.deep_copy();
    }

    return dst_image;
}

Image custom(Image src_image, Matrix<double> kernel) {
    Image dst_image = src_image.deep_copy();

    for (int i = 0; i < int(src_image.n_rows); i++) {
        for (int j = 0; j < int(src_image.n_cols); j++) {
            std::tuple<uint, uint, uint> res{};

            for (int k = - int(kernel.n_rows / 2); k <= int(kernel.n_rows / 2); k++) {
                for (int l = - int(kernel.n_cols / 2); l <= int(kernel.n_cols / 2); l++) {
                    if (k + i < 0 || l + j < 0 || k + i >= int(src_image.n_rows) || l + j >= int(src_image.n_cols)) { continue; }
                    double r, g, b;
                    std::tie(r, g, b) = src_image(i + k, j + l);
                    r *= kernel(kernel.n_rows / 2 + k, kernel.n_cols / 2 + l);
                    g *= kernel(kernel.n_rows / 2 + k, kernel.n_cols / 2 + l);
                    b *= kernel(kernel.n_rows / 2 + k, kernel.n_cols / 2 + l);
                    std::get<0>(res) += r;
                    std::get<1>(res) += g;
                    std::get<2>(res) += b;
                }
            }

            if (int(std::get<0>(res)) > 255) { std::get<0>(res) = 255; }
            if (int(std::get<1>(res)) > 255) { std::get<1>(res) = 255; }
            if (int(std::get<2>(res)) > 255) { std::get<2>(res) = 255; }
            if (int(std::get<0>(res)) < 0) { std::get<0>(res) = 0; }
            if (int(std::get<1>(res)) < 0) { std::get<1>(res) = 0; }
            if (int(std::get<2>(res)) < 0) { std::get<2>(res) = 0; }
            dst_image(i, j) = res;
        }
    }

    return dst_image;
}

Image autocontrast(Image src_image, double fraction) {
    Matrix<double> y(src_image.n_rows, src_image.n_cols);
    long long r, g, b;

    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            std::tie(r, g, b) = src_image(i, j);
            y(i, j) = 0.2125 * r + 0.7154 * g + 0.0721 * b;
        }
    }

    std::vector<long long> v{};
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            v.push_back(y(i, j));
        }
    }
    std::sort(v.begin(), v.end());

    auto black = v.begin() + int(v.size() * fraction);
    auto white = v.end() - int(v.size() * fraction) - 1;

    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            std::tie(r, g, b) = src_image(i, j);
            r *= (y(i, j) - *black) * (255 - 0) / (*white - *black) / y(i, j);
            if (r > 255) { r = 255; }
            if (r < 0) { r = 0; }
            g *= (y(i, j) - *black) * (255 - 0) / (*white - *black) / y(i, j);
            if (g > 255) { g = 255; }
            if (g < 0) { g = 0; }
            b *= (y(i, j) - *black) * (255 - 0) / (*white - *black) / y(i, j);
            if (b > 255) { b = 255; }
            if (b < 0) { b = 0; }
            src_image(i, j) = std::make_tuple(r, g, b);
        }
    }

    return src_image;
}

Image gaussian(Image src_image, double sigma, int radius)  {
    return src_image;
}

Image gaussian_separable(Image src_image, double sigma, int radius) {
    return src_image;
}

int count_linear_median(std::vector<uint> v, int radius) {
    int index = 255;
    int res = 0;
    while (res < double((2 * radius + 1) * (2 * radius + 1)) / 2) {
        res += v[index];
        index--;
    }
    return ++index;
}

Image median(Image src_image, int radius) {
    Image dst_image = src_image.deep_copy();

    for (int i = radius; i < int(src_image.n_rows) - radius; i++) {
        for (int j = radius; j < int(src_image.n_cols) - radius; j++) {
            std::vector<uint> r_histogram(256, 0);
            std::vector<uint> g_histogram(256, 0);
            std::vector<uint> b_histogram(256, 0);
            uint r, g, b;

            for (int k = i - radius; k <= i + radius; k++) {
                for (int l = j - radius; l <= j + radius; l++) {
                    std::tie(r, g, b) = src_image(k, l);

                    r_histogram[r]++;
                    g_histogram[g]++;
                    b_histogram[b]++;
                }
            }

            int index_r = count_linear_median(r_histogram, radius);
            int index_g = count_linear_median(g_histogram, radius);
            int index_b = count_linear_median(b_histogram, radius);
            dst_image(i, j) = std::make_tuple(index_r, index_g, index_b);
        }
    }

    return dst_image;
}


Image median_linear(Image src_image, int radius) {
    int r, g, b;
    Image dst_image = src_image.deep_copy();
    std::vector<uint> r_histogram(256, 0);
    std::vector<uint> g_histogram(256, 0);
    std::vector<uint> b_histogram(256, 0);

    for (int k = 0; k < 2 * radius + 1; k++) {
        for (int j = 0; j < 2 * radius + 1; j++) {
            std::tie(r, g, b) = src_image(k, j);
            r_histogram[r]++;
            g_histogram[g]++;
            b_histogram[b]++;
        }
    }

    int index_r = count_linear_median(r_histogram, radius);
    int index_g = count_linear_median(g_histogram, radius);
    int index_b = count_linear_median(b_histogram, radius);
    dst_image(radius, radius) = std::make_tuple(index_r, index_g, index_b);

    for (uint i = radius; i < src_image.n_rows - radius; i++) {
        if (!((i - radius) % 2)) {
            for (uint j = radius + 1; j < src_image.n_cols - radius; j++) {
                for (int k = -radius; k <= radius; k++) {
                    std::tie(r, g, b) = src_image(i + k, j - radius - 1);
                    r_histogram[r]--;
                    g_histogram[g]--;
                    b_histogram[b]--;
                    std::tie(r, g, b) = src_image(i + k, j + radius);
                    r_histogram[r]++;
                    g_histogram[g]++;
                    b_histogram[b]++;
                }
                index_r = count_linear_median(r_histogram, radius);
                index_g = count_linear_median(g_histogram, radius);
                index_b = count_linear_median(b_histogram, radius);
                dst_image(i, j) = std::make_tuple(index_r, index_g, index_b);
            }

            for (int k = -radius; k <= radius; k++) {
                std::tie(r, g, b) = src_image(i - radius, src_image.n_cols - 1 - radius + k);
                r_histogram[r]--;
                g_histogram[g]--;
                b_histogram[b]--;
                if (i + radius + 1 >= src_image.n_rows) { break; }
                std::tie(r, g, b) = src_image(i + radius + 1, src_image.n_cols - 1 - radius + k);
                r_histogram[r]++;
                g_histogram[g]++;
                b_histogram[b]++;
            }

        } else {
            for (int j = src_image.n_cols - radius - 1; j > radius; j--) {
                for (int k = -radius; k <= radius; k++) {
                    std::tie(r, g, b) = src_image(i + k, j + radius);
                    r_histogram[r]--;
                    b_histogram[b]--;
                    g_histogram[g]--;
                    std::tie(r, g, b) = src_image(i + k, j - radius - 1);
                    r_histogram[r]++;
                    g_histogram[g]++;
                    b_histogram[b]++;
                }
                index_r = count_linear_median(r_histogram, radius);
                index_g = count_linear_median(g_histogram, radius);
                index_b = count_linear_median(b_histogram, radius);
                dst_image(i, j - 1) = std::make_tuple(index_r, index_g, index_b);
            }

            for (int k = -radius; k <= radius; k++) {
                std::tie(r, g, b) = src_image(i - radius, radius + k);
                r_histogram[r]--;
                g_histogram[g]--;
                b_histogram[b]--;
                if (i + radius + 1 >= src_image.n_rows) { break; }
                std::tie(r, g, b) = src_image(i + radius + 1, radius + k);
                r_histogram[r]++;
                g_histogram[g]++;
                b_histogram[b]++;

            }
        }
    }

    return dst_image;
}


Image median_const(Image src_image, int radius) {
    int r, g, b, index_b, index_r, index_g;
    Image dst_image = src_image.deep_copy();
    std::vector<std::vector<uint>> r_histogram(src_image.n_cols);
    std::vector<std::vector<uint>> g_histogram(src_image.n_cols);
    std::vector<std::vector<uint>> b_histogram(src_image.n_cols);
    std::vector<uint> rr_histogram(256, 0);
    std::vector<uint> gg_histogram(256, 0);
    std::vector<uint> bb_histogram(256, 0);

    for (uint i = 0; i < src_image.n_cols; i++) {
        r_histogram[i].resize(256);
        std::fill(r_histogram[i].begin(), r_histogram[i].end(), 0);
        g_histogram[i].resize(256);
        std::fill(g_histogram[i].begin(), g_histogram[i].end(), 0);
        b_histogram[i].resize(256);
        std::fill(b_histogram[i].begin(), b_histogram[i].end(), 0);
    }

    for (int i = 0; i < 2 * radius + 1; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            std::tie(r, g, b) = src_image(i, j);
            r_histogram[j][r]++;
            g_histogram[j][g]++;
            b_histogram[j][b]++;
        }
    }

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 2 * radius + 1; j++) {
            std::tie(r, g, b) = src_image(i, j);
            rr_histogram[i] += r_histogram[j][i];
            gg_histogram[i] += g_histogram[j][i];
            bb_histogram[i] += b_histogram[j][i];
        }
    }

    index_r = count_linear_median(rr_histogram, radius);
    index_g = count_linear_median(gg_histogram, radius);
    index_b = count_linear_median(bb_histogram, radius);
    dst_image(radius, radius) = std::make_tuple(index_r, index_g, index_b);

    for (uint j = radius + 1; j < src_image.n_cols - radius; j++) {
        for (uint i = 0; i < 256; i++) {
            rr_histogram[i] += r_histogram[j + radius][i];
            gg_histogram[i] += g_histogram[j + radius][i];
            bb_histogram[i] += b_histogram[j + radius][i];
            rr_histogram[i] -= r_histogram[j - radius - 1][i];
            gg_histogram[i] -= g_histogram[j - radius - 1][i];
            bb_histogram[i] -= b_histogram[j - radius - 1][i];
            index_r = count_linear_median(rr_histogram, radius);
            index_g = count_linear_median(gg_histogram, radius);
            index_b = count_linear_median(bb_histogram, radius);
            dst_image(radius, j) = std::make_tuple(index_r, index_g, index_b);
        }
    }

    for (uint i = radius + 1; i < src_image.n_rows - radius; i++) {
        if ((i - radius) % 2) {
            for (uint j = src_image.n_cols - 2 * radius - 1; j < src_image.n_cols; j++) {
                std::tie(r, g, b) = src_image(i - radius - 1, j);
                r_histogram[j][r]--;
                g_histogram[j][g]--;
                b_histogram[j][b]--;
                rr_histogram[r]--;
                gg_histogram[g]--;
                bb_histogram[b]--;
                std::tie(r, g, b) = src_image(i + radius, j);
                r_histogram[j][r]++;
                g_histogram[j][g]++;
                b_histogram[j][b]++;
                rr_histogram[r]++;
                gg_histogram[g]++;
                bb_histogram[b]++;
            }
            index_r = count_linear_median(rr_histogram, radius);
            index_g = count_linear_median(gg_histogram, radius);
            index_b = count_linear_median(bb_histogram, radius);
            dst_image(i, src_image.n_cols - radius - 1) = std::make_tuple(index_r, index_g, index_b);

            for (int j = src_image.n_cols - radius - 2; j >= radius; j--) {
                std::tie(r, g, b) = src_image(i - radius - 1, j - radius);
                r_histogram[j - radius][r]--;
                g_histogram[j - radius][g]--;
                b_histogram[j - radius][b]--;
                std::tie(r, g, b) = src_image(i + radius, j - radius);
                r_histogram[j - radius][r]++;
                g_histogram[j - radius][g]++;
                b_histogram[j - radius][b]++;

                for (uint k = 0; k < 256; k++) {
                    rr_histogram[k] -= r_histogram[j + radius + 1][k];
                    gg_histogram[k] -= g_histogram[j + radius + 1][k];
                    bb_histogram[k] -= b_histogram[j + radius + 1][k];
                    rr_histogram[k] += r_histogram[j - radius][k];
                    gg_histogram[k] += g_histogram[j - radius][k];
                    bb_histogram[k] += b_histogram[j - radius][k];

                }
                index_g = count_linear_median(gg_histogram, radius);
                index_r = count_linear_median(rr_histogram, radius);
                index_b = count_linear_median(bb_histogram, radius);
                dst_image(i, j) = std::make_tuple(index_r, index_g, index_b);
            }
        } else {
            for (int j = 0; j < 2 * radius + 1; j++) {
                std::tie(r, g, b) = src_image(i - radius - 1, j);
                r_histogram[j][r]--;
                g_histogram[j][g]--;
                b_histogram[j][b]--;
                rr_histogram[r]--;
                gg_histogram[g]--;
                bb_histogram[b]--;
                std::tie(r, g, b) = src_image(i + radius, j);
                r_histogram[j][r]++;
                g_histogram[j][g]++;
                b_histogram[j][b]++;
                rr_histogram[r]++;
                gg_histogram[g]++;
                bb_histogram[b]++;
            }

            index_r = count_linear_median(rr_histogram, radius);
            index_g = count_linear_median(gg_histogram, radius);
            index_b = count_linear_median(bb_histogram, radius);
            dst_image(i, radius) = std::make_tuple(index_r, index_g, index_b);

            for (uint j = radius + 1; j < src_image.n_cols - radius; j++) {
                std::tie(r, g, b) = src_image(i - radius - 1, j + radius);
                r_histogram[j + radius][r]--;
                g_histogram[j + radius][g]--;
                b_histogram[j + radius][b]--;
                std::tie(r, g, b) = src_image(i + radius, j + radius);
                r_histogram[j + radius][r]++;
                g_histogram[j + radius][g]++;
                b_histogram[j + radius][b]++;

                for (uint k = 0; k < 256; k++) {
                    rr_histogram[k] -= r_histogram[j - radius - 1][k];
                    gg_histogram[k] -= g_histogram[j - radius - 1][k];
                    bb_histogram[k] -= b_histogram[j - radius - 1][k];
                    rr_histogram[k] += r_histogram[j + radius][k];
                    gg_histogram[k] += g_histogram[j + radius][k];
                    bb_histogram[k] += b_histogram[j + radius][k];
                }

                index_g = count_linear_median(gg_histogram, radius);
                index_r = count_linear_median(rr_histogram, radius);
                index_b = count_linear_median(bb_histogram, radius);
                dst_image(i, j) = std::make_tuple(index_r, index_g, index_b);
            }
        }
    }

    return dst_image;
}



Image white_balance(Image src_image) {
    Matrix<double> y(src_image.n_rows, src_image.n_cols);
    double r, g, b, k_r = 0.299, k_b = 0.114, k_g = 0.587;
    Matrix<std::tuple<double, double, double>> work_image(src_image.n_rows, src_image.n_cols);
    double y_hist, c_rhist, c_bhist;

    src_image = autocontrast(src_image, 0.01);

    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            std::tie(r, g, b) = src_image(i, j);
            y_hist = k_r * r + k_g * g + k_b * b;
            c_rhist = 0.5 / (1 - k_r) * (r - y_hist);
            c_bhist = 0.5 / (1 - k_b) * (b - y_hist);
            work_image(i, j) = std::make_tuple(y_hist, c_rhist, c_bhist);
        }
    }

    std::vector<std::tuple<double, double, double>> probable_white{};
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            std::tie(y_hist, c_rhist, c_bhist) = work_image(i, j);
            if (y_hist >= 210 && c_rhist >= -3 && c_rhist <= 3 && c_bhist >= -3 && c_bhist <= 3) {
                probable_white.push_back(work_image(i, j));
            }
        }
    }

    if (!probable_white.size()) { return src_image; }
    std::tuple<double, double, double> brightest = std::make_tuple(-1, -1, -1);
    for (auto pw : probable_white) {
        if (std::get<0>(brightest) < std::get<0>(pw)) {
            brightest = pw;
        }
        if (abs(std::get<0>(brightest) - std::get<0>(pw)) < 0.0000001 && abs(std::get<1>(brightest)) > abs(std::get<1>(pw))) {
            brightest = pw;
        }
        if (abs(std::get<0>(brightest) - std::get<0>(pw)) < 0.0000001 && abs(abs(std::get<1>(brightest)) - abs(std::get<1>(pw))) < 0.0000001 && abs(std::get<2>(brightest)) > abs(std::get<2>(pw))) {
            brightest = pw;
        }
    }

    std::tuple<double, double, double> average;
    double y_hav = 0, c_rhav = 0, c_bhav = 0;
    for (auto pw : probable_white) {
        y_hav += std::get<0>(pw);
        c_rhav += std::get<1>(pw);
        c_bhav += std::get<2>(pw);
    }
    average = std::make_tuple(y_hav / probable_white.size(), c_bhav / probable_white.size(), c_rhav / probable_white.size());

    std::tuple<int, int, int> lower = std::make_tuple(
        floor(std::min(std::get<0>(brightest), std::get<0>(average))),
        floor(std::min(std::get<1>(brightest), std::get<1>(average))),
        floor(std::min(std::get<2>(brightest), std::get<2>(average)))
    );
    std::tuple<int, int, int> upper = std::make_tuple(
        ceil(std::max(std::get<0>(brightest), std::get<0>(average))),
        ceil(std::max(std::get<1>(brightest), std::get<1>(average))),
        ceil(std::max(std::get<2>(brightest), std::get<2>(average)))
    );

    std::vector<std::tuple<uint, uint, uint >> corresponding;
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            if (std::get<0>(lower) <= std::get<0>(work_image(i, j)) && std::get<0>(work_image(i, j)) <= std::get<0>(upper) &&
                std::get<1>(lower) <= std::get<1>(work_image(i, j)) && std::get<1>(work_image(i, j)) <= std::get<1>(upper) &&
                std::get<2>(lower) <= std::get<2>(work_image(i, j)) && std::get<2>(work_image(i, j)) <= std::get<2>(upper)) {
                corresponding.push_back(src_image(i, j));
            }
        }
    }

    int r_av = 0, g_av = 0, b_av = 0;
    for (auto cc : corresponding) {
        r_av += std::get<0>(cc);
        g_av += std::get<1>(cc);
        b_av += std::get<2>(cc);
    }

    double r_w, r_factor, r_scale, r_gwa, r_hav = 0;
    double g_w, g_factor, g_scale, g_gwa, g_hav = 0;
    double b_w, b_factor, b_scale, b_gwa, b_hav = 0;
    r_w = r_av / corresponding.size();
    g_w = g_av / corresponding.size();
    b_w = b_av / corresponding.size();

    double y_w = 0.299 * r_w + 0.587 * g_w + 0.114 * b_w;
    r_scale = y_w / r_w;
    g_scale = y_w / r_w;
    b_scale = y_w / r_w;

    double y_av = 0;
    r_av = 0;
    g_av = 0;
    b_av = 0;
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            r_av += std::get<0>(src_image(i, j));
            g_av += std::get<1>(src_image(i, j));
            b_av += std::get<2>(src_image(i, j));
        }
    }
    r_av /= src_image.n_rows * src_image.n_cols;
    g_av /= src_image.n_rows * src_image.n_cols;
    b_av /= src_image.n_rows * src_image.n_cols;

    y_av = 0.299 * r_av + 0.587 * g_av + 0.114 * b_av;
    r_gwa = y_av / r_av;
    g_gwa = y_av / g_av;
    b_gwa = y_av / b_av;

    r_hav = std::get<0>(average) + (1 - k_r) / 0.5 * std::get<1>(average);
    g_hav = std::get<0>(average) - 2 * k_b * (1 - k_b) / k_g * std::get<2>(average) - 2 * k_r * (1 - k_r) / k_g * std::get<1>(average);
    b_hav = std::get<0>(average) + (1 - k_b) / 0.5 * std::get<2>(average);

    int color_cast = 0;
    if (r_hav + 3 >= g_hav && b_hav > r_hav) {
        color_cast++;
    }
    if (g_hav + 3 > r_hav && r_hav > b_hav) {
        color_cast++;
    }
    if (r_hav > g_hav && g_hav > b_hav) {
        color_cast++;
    }
    if (color_cast == 0) {
        return src_image;
    }
    if (color_cast == 1) {
        r_factor = r_gwa;
        g_factor = g_scale;
        b_factor = b_scale;
    }
    if (color_cast == 2) {
        r_factor = r_scale;
        g_factor = g_gwa;
        b_factor = b_scale;
    }
    if (color_cast == 3) {
        r_factor = r_scale;
        g_factor = g_scale;
        b_factor = b_gwa;
    }

    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            std::tie(r, g, b) = src_image(i, j);
            r *= r_factor;
            if (r > 255) { r = 255; }
            if (r < 0) { r = 0; }
            g *= g_factor;
            if (g > 255) { g = 255; }
            if (g < 0) { g = 0; }
            b *= b_factor;
            if (b > 255) { b = 255; }
            if (b < 0) { b = 0; }
            src_image(i, j) = std::make_tuple(r, g, b);
        }
    }
    
    return src_image;
}

Image canny(Image src_image, int threshold1, int threshold2) {
    return src_image;
}
