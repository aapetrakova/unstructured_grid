#ifndef GRID_H
#define GRID_H

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "figure.h"

/*
 * Functions for reading input files
*/
std::vector<std::vector<ContourPoint>> read_contour_csv(const std::string& filename);
void build_contour_connections(std::vector<ContourPoint>& all_contours);
std::vector<Point> read_throw_in_points_csv(const std::string& filename);

/*
 * Hard triangle grid building
*/
// Area preparation
double Z(ContourPoint P, ContourPoint Q, ContourPoint R);
bool check_edge_intersection(const std::set<Edge>& edges, Edge AB);
void circ_shift(const ContourPoint& A, std::vector<ContourPoint>& contour);
std::vector<ContourPoint> prepare_area(const std::vector<std::vector<ContourPoint>>& all_contours);

#endif // GRID_H
