#pragma once

#include <map>
#include <array>

namespace lib
{

namespace detail
{

template<
    typename Mat,
    std::size_t Dim
>
class Placeholder
{
    static_assert(Mat::dimensions_number >= Dim, "Placeholder's dimension exceeds base matrix dimensions count");
    static_assert(Dim > 0, "No dimension");

    friend Mat;
    friend class Placeholder<Mat, Dim + 1>;

    using Coordinates = std::array<std::size_t, Mat::dimensions_number>;

public:

    using value_type = typename Mat::value_type;
    using self_type = Placeholder<Mat, Dim>;

    Placeholder() = delete;

    Placeholder(self_type& other) = delete;
    Placeholder& operator=(self_type& other) = delete;

    Placeholder(self_type&& other) = delete;
    Placeholder& operator=(self_type&& other) = delete;

    ~Placeholder() = default;

    template<
        bool Cond = (Dim > 1),
        typename = typename std::enable_if_t<Cond>
    >
    Placeholder<Mat, Dim - 1> operator[](const std::size_t& index)
    {
        return Placeholder<Mat, Dim - 1>(matrix, std::move(coordinates), index);
    }

    template<
        typename Val,
        bool Cond = (Dim == 1),
        typename = typename std::enable_if_t<Cond>
    >
    Placeholder<Mat, Dim>& operator=(const Val other)
    {
        if constexpr (std::is_reference_v<std::remove_const<Val>>) {
            set(std::forward(other), indices);
        } else {
            set(other, indices);
        }
        return *this;
    }

    template<
        bool Cond = (Dim == 1),
        typename = typename std::enable_if_t<Cond>
    >
    operator value_type() const
    {
        return get(indices);
    }

private:
    Placeholder(Mat& m, const std::size_t& i)
        : matrix(m)
    {
        coordinates[Mat::dimensions_number - Dim] = i;
    }

    Placeholder(Mat& m, Coordinates&& c, const std::size_t& i)
        : matrix(m)
        , coordinates(std::move(c))
    {
        coordinates[Mat::dimensions_number - Dim] = i;
    }

    template<typename Val, std::size_t... I>
    void set(const Val other, std::index_sequence<I...>)
    {
        if constexpr (std::is_reference_v<std::remove_const<Val>>) {
            matrix.set(std::forward(other), coordinates[I]...); 
        } else {
            matrix.set(other, coordinates[I]...);
        }
    }

    template<std::size_t... I>
    value_type get(std::index_sequence<I...>) const
    {
        return matrix.get(coordinates[I]...);
    }

    Mat& matrix;
    Coordinates coordinates;

    static const std::make_index_sequence<Mat::dimensions_number> indices;
};

}

template<
    typename T,
    T Default = 0,
    std::size_t Dims = 2
>
class Matrix
{
    static_assert(Dims > 0, "Too few dimensions");
public:
    using value_type = T;
    using self_type = Matrix<T, Default, Dims>;
    using SubMatrix = std::conditional_t< (Dims > 1), Matrix<T, Default, Dims - 1>, value_type >;
    using nodes_container = std::map<std::size_t, SubMatrix>;
    using const_iterator = typename nodes_container::const_iterator;

    static constexpr std::size_t default_value = Default;
    static constexpr std::size_t dimensions_number = Dims;

    std::size_t size() const
    {
        if constexpr (Dims == 1) {
            return nodes.size();
        } else {
            std::size_t sz = 0;
            for(auto&& [index, submatrix] : nodes)
                sz += submatrix.size();
            return sz;
        }
    }

    template<typename Index>
    T get(Index&& coordinate) const
    {
        const_iterator it = nodes.find(coordinate);
        if(it != nodes.end())
            return it->second;
        return default_value;
    }

    template<typename Index, typename... Indexes>
    T get(Index&& head, Indexes&&... coordinates) const
    {
        const_iterator it = nodes.find(head);
        if(it != nodes.end())
            return it->second.get(std::forward<Indexes>(coordinates)...);
        return default_value;
    }

    template<typename Index>
    void set(const T& val, Index&& coordinate)
    {
        iterator it = nodes.find(coordinate);
        if(it == nodes.end()) {
            if(val != default_value)
                nodes[coordinate] = val;
        } else {
            if(val != default_value)
                it->second = val;
            else
                nodes.erase(it);
        }
    }

    template<typename Index, typename... Indexes>
    void set(const T& val, Index&& head, Indexes&&... coordinates)
    {
        iterator it = nodes.find(head);
        if(it != nodes.end()) {
            it->second.set(val, std::forward<Indexes>(coordinates)...);
            if(it->second.size() == 0)
                nodes.erase(it);
        } else {
            if(val == default_value)
                return;
            nodes[head].set(val, std::forward<Indexes>(coordinates)...);
        }
    }

    detail::Placeholder<self_type, Dims> operator[](const std::size_t& index)
    {
        return detail::Placeholder<self_type, Dims>(*this, index);
    }

    const_iterator begin() const
    {
        return nodes.cbegin();
    }

    const_iterator end() const
    {
        return nodes.cend();
    }

private:
    using iterator = typename nodes_container::iterator;

    nodes_container nodes;
};

}

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