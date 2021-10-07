#include <iostream>
#include "matrix.h"

namespace ser
{

namespace detail
{

template<typename T>
void print(const T val) {
    std::cout << val;
}

template<typename T, typename... Args>
void print(const T val, Args&&... coordinates) {
    std::cout << val << ',';
    print(std::forward<Args>(coordinates)...);
}

template<typename Val, typename... Args>
void print_flat_dummy(const Val v, Args&&... coordinates)
{
    std::cout << "{";
    print(std::forward<Args>(coordinates)...);
    std::cout << "}: " << v << "\n";
}

}

template<
    typename Mat,
    typename... Args
>
std::enable_if_t<(Mat::dimensions_number == 1)> print_flat(const Mat& m, Args&&... coordinates)
{
    for(auto&& [dim, vals] : m)
    {
        detail::print_flat_dummy(vals, dim, std::forward<Args>(coordinates)...);
    }
}

template<
    typename Mat,
    typename... Args
>
std::enable_if_t<(Mat::dimensions_number > 1)> print_flat(const Mat& m, Args&&... coordinates)
{
    for(auto&& [dim, vals] : m)
    {
        print_flat(vals, dim, std::forward<Args>(coordinates)...);
    }
}

}

int main(int , char**)
{
    lib::Matrix<int> matrix;

    for(std::size_t i = 0; i <= 9; ++i)
    {
        matrix[i][i] = i;
        matrix[i][9 - i] = i;
    }

    for(std::size_t row = 1; row <= 8; ++row)
    {
        for(std::size_t col = 1; col <= 8; ++col)
        {
            std::cout << matrix[row][col] << " ";
        }
        std::cout << "\n";
    }

    std::cout << matrix.size() << "\n";

    ser::print_flat(matrix);

    return 0;
}