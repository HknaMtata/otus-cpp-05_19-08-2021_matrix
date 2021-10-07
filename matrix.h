#pragma once

#include <map>
#include <array>
#include <numeric>
#include <utility>

template<
    typename Mat,
    std::size_t Dim
>
class Placeholder;

// TODO: forbid to crate matrix with 0 dimensions
template<
    typename T,
    T Default = 0,
    std::size_t Dims = 2
    // bool Cond = (Dims > 0),
    // typename = typename std::enable_if_t<Cond, void>
>
class Matrix
{
public:
    using self_type = Matrix<T, Default, Dims>;
    using value_type = T;

    static constexpr std::size_t default_value = Default;
    static constexpr std::size_t dimensions_number = Dims;

    using SubMatrix = std::conditional_t< (Dims > 1), Matrix<T, Default, Dims - 1>, value_type >;

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
            return it->second.get(coordinates...);
        return default_value;
    }

    template<typename Val, typename Index>
    void set(Val&& value, Index&& coordinate)
    {
        iterator it = nodes.find(coordinate);
        if(it == nodes.end()) {
            if(value != default_value)
                nodes[coordinate] = value;
        } else {
            if(value != default_value)
                it->second = value;
            else
                nodes.erase(it);
        }
    }

    template<typename Val, typename Index, typename... Indexes>
    void set(Val&& value, Index&& head, Indexes&&... coordinates)
    {
        iterator it = nodes.find(head);
        if(it != nodes.end()) {
            it->second.set(value, coordinates...);
            if(it->second.size() == 0)
                nodes.erase(it);
        } else {
            if(value == default_value)
                return;
            nodes[head].set(value, coordinates...);
        }
    }

    Placeholder<self_type, Dims> operator[](const std::size_t& index)
    {
        return Placeholder<self_type, Dims>(*this, index);
    }

private:
    using nodes_container = std::map<std::size_t, SubMatrix>;
    using const_iterator = typename nodes_container::const_iterator;
    using iterator = typename nodes_container::iterator;

    nodes_container nodes;
};

// TODO: forbid to crate a Placeholder with MaxDims < 2
// TODO: forbid to crate a Placeholder with Dims == 0
// TODO: forbid to crate a Placeholder with MaxDims < Dims
template<
    typename Mat,
    std::size_t Dim
>
class Placeholder
{
public:
    using value_type = typename Mat::value_type;

    Placeholder() = delete;

    Placeholder(Placeholder<Mat, Dim>& other) = delete;
    Placeholder<Mat, Dim>& operator=(Placeholder<Mat, Dim>& other) = delete;

    Placeholder<Mat, Dim>(Placeholder<Mat, Dim>&& other) = delete;
    Placeholder<Mat, Dim>& operator=(Placeholder<Mat, Dim>&& other) = delete;

    ~Placeholder() = default;

    template<
        bool Cond = (Dim > 1),
        typename = typename std::enable_if_t<Cond, void>
    >
    Placeholder<Mat, Dim - 1> operator[](const std::size_t& index)
    {
        return Placeholder<Mat, Dim - 1>(matrix, std::move(coordinates), index);
    }

    template<
        bool Cond = (Dim == 1),
        typename = typename std::enable_if_t<Cond, void>
    >
    Placeholder<Mat, Dim>& operator=(const value_type& other)
    {
        set(other);
        return *this;
    }

    template<
        bool Cond = (Dim == 1),
        typename = typename std::enable_if_t<Cond, void>
    >
    operator value_type() const
    {
        return get();
    }

private:
    friend Mat;
    friend class Placeholder<Mat, Dim + 1>;

    using Coordinates = std::array<std::size_t, Mat::dimensions_number>;

    Placeholder(Mat& m, const std::size_t& i)
        : matrix(m)
    {
        coordinates[Mat::dimensions_number - Dim] = std::move(i);
    }

    Placeholder(Mat& m, Coordinates&& c, const std::size_t& i)
        : matrix(m)
        , coordinates(std::move(c))
    {
        coordinates[Mat::dimensions_number - Dim] = i;
    }

    // TODO: ref
    template<std::size_t... I>
    void set(const value_type& other, std::index_sequence<I...>)
    {
        matrix.set(other, coordinates[I]...);
    }

    template<typename Indices = std::make_index_sequence<Mat::dimensions_number>>
    void set(const value_type& other)
    {
        set(other, Indices{});
    }

    // TODO: ref
    template<std::size_t... I>
    value_type get(std::index_sequence<I...>) const
    {
        return matrix.get(coordinates[I]...);
    }

    template<typename Indices = std::make_index_sequence<Mat::dimensions_number>>
    value_type get() const
    {
        return get(Indices{});
    }

    Mat& matrix;
    Coordinates coordinates;
};