#include"Cube3.hpp"
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <execution>

Cube3::Cube3(){
    top_side = new Side(TOP_SIDE, 3 ,YELLOW, {2, 11, 19, 1, 10, 18, 0, 9, 17}, [this](Direction dir) { this->u_turn(dir); });
    bottom_side = new Side(BOTTOM_SIDE, 3, WHITE, {6, 14, 23, 7, 15, 24, 8, 16, 25}, [this](Direction dir) { this->d_turn(dir); });
    front_side = new Side(FRONT_SIDE, 3, BLUE, {0, 9, 17, 3, 12, 20, 6, 14, 23}, [this](Direction dir) { this->f_turn(dir); });
    right_side = new Side(RIGHT_SIDE, 3, RED, {17, 18, 19, 20, 21, 22, 23, 24, 25}, [this](Direction dir) { this->r_turn(dir); });
    back_side = new Side(BACK_SIDE, 3, GREEN, {19, 11, 2, 22, 13, 5, 25, 16, 8}, [this](Direction dir) { this->b_turn(dir); });
    left_side = new Side(LEFT_SIDE, 3, ORANGE, {2, 1, 0, 5, 4, 3, 8, 7, 6}, [this](Direction dir) { this->l_turn(dir); });
    sides = {top_side, right_side, front_side, bottom_side, left_side, back_side};
}

void Cube3::u_turn(Direction direction)
{
    /* make a copy */
    Cubie front[3], right[3], back[3], left[3];
    for (int i = 0; i < 3; i++)
    {
        front[i] = front_side->cubie[i];
        right[i] = right_side->cubie[i];
        back[i] = back_side->cubie[i];
        left[i] = left_side->cubie[i];
        /* depending on the direction assign the new cubie */
        switch (direction)
        {
        default:
        case CLOCKWISE:
            front_side->cubie[i] = right[i];
            right_side->cubie[i] = back[i];
            back_side->cubie[i] = left[i];
            left_side->cubie[i] = front[i];
            break;
        case ANTICLOCKWISE:
            front_side->cubie[i] = left[i];
            right_side->cubie[i] = front[i];
            back_side->cubie[i] = right[i];
            left_side->cubie[i] = back[i];
            break;
        }
    }
}

void Cube3::d_turn(Direction direction)
{
    /* make a copy */
    Cubie front[3], right[3], back[3], left[3];
    for (int i = 6; i < 9; i++)
    {
        int shiftedIindex = i-6;
        front[shiftedIindex] = front_side->cubie[i];
        right[shiftedIindex] = right_side->cubie[i];
        back[shiftedIindex] = back_side->cubie[i];
        left[shiftedIindex] = left_side->cubie[i];
        /* depending on the direction assign the new cubie */
        switch (direction)
        {
        default:
        case CLOCKWISE:
            front_side->cubie[i] = left[shiftedIindex];
            right_side->cubie[i] = front[shiftedIindex];
            back_side->cubie[i] = right[shiftedIindex];
            left_side->cubie[i] = back[shiftedIindex];
            break;
        case ANTICLOCKWISE:
            front_side->cubie[i] = right[shiftedIindex];
            right_side->cubie[i] = back[shiftedIindex];
            back_side->cubie[i] = left[shiftedIindex];
            left_side->cubie[i] = front[shiftedIindex];
            break;
        }
    }
}

void Cube3::f_turn(Direction direction)
{
    /* make a copy */
    Cubie top[3], right[3], bottom[3], left[3];
    for (int i = 0; i < 3; i++)
    {
        top[i] = top_side->cubie[6 + i];
        right[i] = right_side->cubie[i * 3];
        bottom[i] = bottom_side->cubie[2 - i];
        left[i] = left_side->cubie[8 - i * 3];
        /* depending on the direction assign the new cubie */
        switch (direction)
        {
        default:
        case CLOCKWISE:
            top_side->cubie[6 + i] = left[i];
            right_side->cubie[i * 3] = top[i];
            bottom_side->cubie[2 - i] = right[i];
            left_side->cubie[8 - i * 3] = bottom[i];
            break;
        case ANTICLOCKWISE:
            top_side->cubie[6 + i] = right[i];
            right_side->cubie[i * 3] = bottom[i];
            bottom_side->cubie[2 - i] = left[i];
            left_side->cubie[8 - i * 3] = top[i];
            break;
        }
    }
}

void Cube3::r_turn(Direction direction)
{
    Cubie top[3], back[3], bottom[3], front[3];
    for (int i = 2; i < 9; i += 3)
    {
        int x = (i - 2) / 3;
        top[x] = top_side->cubie[i];
        front[x] = front_side->cubie[i];
        bottom[x] = bottom_side->cubie[i];
        back[x] = back_side->cubie[8 - i];
        switch (direction)
        {
        default:
        case CLOCKWISE:
            top_side->cubie[i] = front[x];
            front_side->cubie[i] = bottom[x];
            bottom_side->cubie[i] = back[x];
            back_side->cubie[8 - i] = top[x];
            break;
        case ANTICLOCKWISE:
            top_side->cubie[i] = back[x];
            front_side->cubie[i] = top[x];
            bottom_side->cubie[i] = front[x];
            back_side->cubie[8 - i] = bottom[x];
            break;
        }
    }
}

void Cube3::b_turn(Direction direction)
{
    Cubie top[3], right[3], bottom[3], left[3];
    for (int i = 0; i < 3; i++)
    {
        top[i] = top_side->cubie[i];
        right[i] = right_side->cubie[2 + (i * 3)];
        bottom[i] = bottom_side->cubie[8 - i];
        left[i] = left_side->cubie[6 - (i * 3)];
        switch (direction)
        {
        case CLOCKWISE:
        default:
            top_side->cubie[i] = right[i];
            right_side->cubie[2 + (i * 3)] = bottom[i];
            bottom_side->cubie[8 - i] = left[i];
            left_side->cubie[6 - (i * 3)] = top[i];
            break;
        case ANTICLOCKWISE:
            top_side->cubie[i] = left[i];
            right_side->cubie[2 + (i * 3)] = top[i];
            bottom_side->cubie[8 - i] = right[i];
            left_side->cubie[6 - (i * 3)] = bottom[i];
            break;
        }
    }
}

void Cube3::l_turn(Direction direction)
{
    Cubie top[3], back[3], bottom[3], front[3];
    for (int i = 0; i < 7; i += 3)
    {
        int x = i / 3;
        top[x] = top_side->cubie[i];
        front[x] = front_side->cubie[i];
        bottom[x] = bottom_side->cubie[i];
        back[x] = back_side->cubie[8 - i];
        switch (direction)
        {
        default:
        case CLOCKWISE:
            top_side->cubie[i] = back[x];
            front_side->cubie[i] = top[x];
            bottom_side->cubie[i] = front[x];
            back_side->cubie[8 - i] = bottom[x];
            break;
        case ANTICLOCKWISE:
            top_side->cubie[i] = front[x];
            front_side->cubie[i] = bottom[x];
            bottom_side->cubie[i] = back[x];
            back_side->cubie[8 - i] = top[x];
            break;
        }
    }
}

void Cube3::turn(const std::string& str){
    Direction direction = CLOCKWISE;
    int turns = 1;
    for(int i = 0;i < str.length();i++){
        // check if there are (') or (2)
        if(i != str.length() && str[i + 1] == '\'') direction = ANTICLOCKWISE;
        if(i != str.length() && (str[i + 1] >= '0' && str[i + 1] <= '9')) turns = (str[i + 1] - '0');

        // Execute the move
        for (int j = 0; j < turns; j++) {
            getSide(str[i])->turn(direction);
        }


        // reset direction and turns values
        if(direction == ANTICLOCKWISE || turns > 1){
            direction = CLOCKWISE;
            turns = 1;
            i++;
        }
    }
}

std::string Cube3::state() {
    // Create a vector to store the state of the cube
    std::vector<char> cube3State;
    // Reserve space in the vector to avoid multiple reallocations
    cube3State.reserve(this->sides.size() * 9);

    auto processSide = [&cube3State](Side* side){
        // Create a subrange for the first 9 cubies of the current side
        auto cubieRange = std::span(side->cubie, 9);
        // Transform the cubieRange using the function f and append the results to cube3State
        std::ranges::transform(cubieRange, std::back_inserter(cube3State), [](Cubie& cubie) {
            return cubie.getLetter();
        });
    };

    std::for_each(std::execution::par, std::begin(this->sides), std::end(this->sides), processSide);

    // Convert the vector of characters to a string and return it
    return std::string(cube3State.begin(), cube3State.end());
}

Side* Cube3::getSide(char sideChar)
{
    for (auto &side : this->sides)
    {
        if (side->sidename == static_cast<_Side_>(sideChar))
            return side;
    }
    return nullptr;
}

std::vector<int> Cube3::getFaceId(char sideChar) {
    // Get the requested side based on the provided character
    Side* requestedSide = this->getSide(sideChar);
    std::vector<int> faceId;
    // Reserve space in the vector to avoid multiple reallocations
    faceId.reserve(9);

    if (requestedSide != nullptr) {
        // Use std::ranges::transform to fill faceId with the IDs of the cubies
        std::ranges::transform(std::span(requestedSide->cubie, 9), std::back_inserter(faceId), [](const Cubie& cubie) {
            return cubie.id;
        });
    }

    return faceId;
}

bool Cube3::isSolved(){
    return this->state() == "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
}