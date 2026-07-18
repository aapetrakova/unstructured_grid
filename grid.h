#ifndef GRID_H
#define GRID_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "figure.h"

/*
 * Functions for reading input files
*/
std::vector<std::vector<ContourPoint>> read_contour_csv(const std::string& filename);
void build_contour_connections(std::vector<std::vector<ContourPoint>>& all_contours);
std::vector<Point> read_throw_in_points_csv(const std::string& filename);

#endif // GRID_H
