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

struct Edge {
  ContourPoint start;
  ContourPoint end;
  Edge* next = nullptr;
  Edge* prev = nullptr;

  Edge() = default;
  Edge(ContourPoint start, ContourPoint end): start(start), end(end), next(nullptr), prev(nullptr) {}
  Edge(Point start, Point end): start(start.x, start.y, 0, 0), end(end.x, end.y, 0, 1), next(nullptr), prev(nullptr) {}

  Edge(const Edge& other) = default;
  Edge& operator=(const Edge& other) = default;

  ~Edge() = default;
};


#endif FIGURE_H
