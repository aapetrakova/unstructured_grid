#ifndef FIGURE_H
#define FIGURE_H

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <tuple>

struct Point {
  double x;
  double y;

  Point() = default;
  Point(double x, double y): x(x), y(y) {}
  Point(const Point& other) = default;
  Point& operator=(const Point& other) = default;
  virtual ~Point() = default;
};

struct ContourPoint: public Point {
  size_t c_num;
  size_t num;
  ContourPoint* next = nullptr;
  ContourPoint*prev = nullptr;

  ContourPoint() = default;
  ContourPoint(double x, double y, size_t c_num, size_t num): Point(x, y), c_num(c_num), num(num) {}
  
  ContourPoint(const ContourPoint& other) = default;
  ContourPoint& operator=(const ContourPoint& other) = default;

  ~ContourPoint() override = default;
};


#endif FIGURE_H
