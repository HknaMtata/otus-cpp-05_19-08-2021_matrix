#include <iostream>
#include "matrix.h"

int main(int , char**)
{
    Matrix<int, 0, 2> m;

    // m[1][1][1] = 1; // Should be a compile error
    // m[1] = 1; // Should be a compile error

    // int v1 = m[1][1][1]; // Should be a compile error
    // (void)v1;

    // int v2 = m[1]; // Should be a compile error
    // (void)v2;

    // Placeholder<int, 2, 2> placeholder = m[1]; // Should be a compile error

    // Matrix<int, 0> ma; // Should be a compile error

    for(std::size_t i = 0; i <= 9; ++i)
    {
        m[i][i] = i;
        m[i][9 - i] = i;
    }

    for(std::size_t row = 1; row <= 8; ++row)
    {
        for(std::size_t col = 1; col <= 8; ++col)
        {
            std::cout << static_cast<int>(m[row][col]) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    std::cout << m.size() << "\n";
    std::cout << "\n";

    // for(std::size_t i = 0; i <= 9; ++i)
    // {
    //     m[i][i] = 0;
    //     m[9 - i][i] = 0;
    // }

    // for(std::size_t row = 1; row <= 8; ++row)
    // {
    //     for(std::size_t col = 1; col <= 8; ++col)
    //     {
    //         std::cout << m[row][col] << " ";
    //     }
    //     std::cout << "\n";
    // }

    // std::cout << "\n";
    // std::cout << m.size() << "\n";
    // std::cout << "\n";

    // Matrix<int, 3> m3d;

    // for(std::size_t x = 0; x < 3; ++x)
    // {
    //     for(std::size_t i = 0; i <= 9; ++i)
    //     {
    //         m3d[x][i][i] = i;
    //         m3d[x][9 - i][i] = i;
    //     }
    // }

    // for(std::size_t x = 0; x < 3; ++x)
    // {
    //     for(std::size_t row = 1; row <= 8; ++row)
    //     {
    //         for(std::size_t col = 1; col <= 8; ++col)
    //         {
    //             std::cout << m3d[x][row][col] << " ";
    //         }
    //         std::cout << "\n";
    //     }
    //     std::cout << "\n";
    // }

    // std::cout << m3d.size() << "\n";
    // std::cout << "\n";

    return 0;
}