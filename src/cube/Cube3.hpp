#ifndef CUBE3_H
#define CUBE3_H

#include"Side.hpp"
#include<string>
#include<vector>
#include<iostream>
#include <optional>

class Cube3 {
private:
    Side* top_side;
    Side* bottom_side;
    Side* front_side;
    Side* right_side;
    Side* back_side;
    Side* left_side;
    std::vector<Side*> sides;

    void u_turn(Direction direction);
    void d_turn(Direction direction);
    void f_turn(Direction direction);
    void r_turn(Direction direction);
    void b_turn(Direction direction);
    void l_turn(Direction direction);

    Side* getSide(char sideChar);
public:
    Cube3();
    void turn(const std::string& str);
    std::string state();
    bool isSolved();
    std::vector<int> getFaceId(char sideChar);
};

#endif