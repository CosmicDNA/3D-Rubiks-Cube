#include"Side.hpp"
#include<iostream>
#include <ranges>
#include <array>

Side::Side(_Side_ sidename, int size, Color color, std::vector<int> pieceId, std::function<void(Direction)> turnFunction) : size(size)
{
    this->turnFunction = turnFunction;
    this->sidename = sidename;
    int arraySize = this->size * this->size;
    cubie = new Cubie[arraySize];
    std::span<Cubie> cubie_span(cubie, arraySize);
    for (const auto &[i, a_cubie] : cubie_span | std::views::enumerate)
    {
        a_cubie.color = color;
        a_cubie.id = pieceId[i];
    }

    // Reserve space in the vector to avoid multiple reallocations
    index.reserve(arraySize);
    // calculating index for face turn
    for (int i = size - 1; i > -1; i--) {
        auto transformed = std::views::iota(0, size) | std::views::transform([i, size](int j) {
            return i + j * size;
        });
        std::ranges::copy(transformed, std::back_inserter(index));
    }
}

Side::~Side(){
    delete[] cubie;
}

void Side::turn(Direction Direction)
{
    /* make a copy */
    int numOfPieces = size * size;
    std::vector<Cubie> _copy(numOfPieces);
    for (auto &&[i, copy] : _copy | std::views::enumerate)
    {
        copy = cubie[i];
    }

    /* turn clockwise or anticlockwise */
    switch (Direction)
    {
    default:
    case CLOCKWISE:
        for (int i = 0; i < numOfPieces; i++)
        {
            cubie[i] = _copy[index[numOfPieces - 1 - i]];
        }
        break;
    case ANTICLOCKWISE:
        for (int i = 0; i < numOfPieces; i++)
        {
            cubie[i] = _copy[index[i]];
        }
        break;
    }
    this->turnFunction(Direction);
}