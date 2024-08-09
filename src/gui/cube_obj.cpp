#include"cube_obj.hpp"

void CubeObj::adjustAxis(std::string rotateSide, float angle) {
    if(rotateSide == "TOP") transform.coordinateSystem.rotate('y', angle);
    if(rotateSide == "BOTTOM") transform.coordinateSystem.rotate('y', -angle);
    if(rotateSide == "FRONT") transform.coordinateSystem.rotate('z', angle);
    if(rotateSide == "BACK") transform.coordinateSystem.rotate('z', -angle);
    if(rotateSide == "RIGHT") transform.coordinateSystem.rotate('x', -angle);
    if(rotateSide == "LEFT") transform.coordinateSystem.rotate('x', angle);
}

void CubeObj::rotate(char plane, float value){
    char correctPlane = plane;
    /*
    for(const auto& pair : transform.axis)
        if(pair.second.value == plane)
            correctPlane = pair.first;
    */
    //std::cout << correctPlane << std::endl;
    
}

void CubeObj::showCoordinate(){
    std::cout << "x: " <<
        transform.coordinateSystem.i.x << ", " <<
        transform.coordinateSystem.i.y << ", " <<
        transform.coordinateSystem.i.z << std::endl;
    std::cout << "y: " <<
        transform.coordinateSystem.j.x << ", " <<
        transform.coordinateSystem.j.y << ", " <<
        transform.coordinateSystem.j.z << std::endl;
    std::cout << "z: " <<
        transform.coordinateSystem.k.x << ", " <<
        transform.coordinateSystem.k.y << ", " <<
        transform.coordinateSystem.k.z << std::endl;
}