#include <iostream>
#include "matrix.h"

int main(int , char**)
{
    lib::Matrix<int> m;

    for(std::size_t i = 0; i <= 9; ++i)
    {
        m[i][i] = i;
        m[i][9 - i] = i;
    }

    for(std::size_t row = 1; row <= 8; ++row)
    {
        for(std::size_t col = 1; col <= 8; ++col)
        {
            std::cout << m[row][col] << " ";
        }
        std::cout << "\n";
    }

    std::cout << m.size() << "\n";

    ser::print_flat(m);

    return 0;
}