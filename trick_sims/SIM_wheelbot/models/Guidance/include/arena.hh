#ifndef ARENA_H
#define ARENA_H
#include <iostream>
#include <vector>
#include "gridSquare.hh"
#include "point.hh"

class Arena {
    public:
        Arena(unsigned int width, unsigned int height);
        Arena(unsigned int width, unsigned int height, unsigned char bits[]);
        ~Arena();
        void block(unsigned int x, unsigned int y);
        void unblock(unsigned int x, unsigned int y);
        void mark(unsigned int x, unsigned int y, char c);
        std::vector<GridSquare*> getNeighbors(GridSquare* gridSquarePointer);
        GridSquare* getGridSquare(unsigned int x, unsigned int y);
        int getGridSquareCoordinates(GridSquare* gridSquarePointer, Point& coords);
        int movementCostEstimate(GridSquare* orig, GridSquare* dest, int& cost);
        int distanceBetween(GridSquare* orig, GridSquare* dest, int& distance);
        int getHeight(){return height;}
        int getWidth(){return width;}

        friend std::ostream& operator<< (std::ostream& s, const Arena& arena);

    private:
        int height;
        int width;
        GridSquare *grid;
        int calcOffset(unsigned int x, unsigned int y, size_t& offset);
        int calcOffset(GridSquare* gridSquare, size_t& offset);
};
#endif
