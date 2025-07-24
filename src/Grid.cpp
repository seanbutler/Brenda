#include "Grid.h"
#include "TiledParser.h"

bool Grid::loadFromTiled(const std::string& filename) {
    return TiledParser::loadTiledMap(filename, *this);
}
