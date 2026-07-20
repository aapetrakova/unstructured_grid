#ifndef FIGURE_H
#define FIGURE_H

#include <iostream>
#include <memory>
#include <ostream>

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
  bool is_real;
  ContourPoint* next = nullptr;
  ContourPoint*prev = nullptr;

  ContourPoint() = default;
  ContourPoint(double x, double y, bool is_real, size_t c_num, size_t num): Point(x, y), is_real(is_real), c_num(c_num), num(num) {}
  
  ContourPoint(const ContourPoint& other) = default;
  ContourPoint& operator=(const ContourPoint& other) = default;

  ~ContourPoint() override = default;
};

struct Edge {
  ContourPoint* start = nullptr;
  ContourPoint* end = nullptr;
  Edge* next = nullptr;
  Edge* prev = nullptr;

  Edge() = default;
  Edge(ContourPoint* start, ContourPoint* end): start(start), end(end), next(nullptr), prev(nullptr) {}

  Edge(const Edge& other) = default;
  Edge& operator=(const Edge& other) = default;

  ~Edge() = default;
};

bool operator==(const Edge& e1, const Edge& e2);
bool operator!=(const Edge& e1, const Edge& e2);
bool operator<(const Edge& e1, const Edge& e2);
bool operator>=(const Edge& e1, const Edge& e2);
bool operator>(const Edge& e1, const Edge& e2);
bool operator<=(const Edge& e1, const Edge& e2);

struct Triangle {
  ContourPoint A;
  ContourPoint B;
  ContourPoint C;
  Edge AB;
  Edge BC;
  Edge CA;
  Triangle* near_AB = nullptr;
  Triangle* near_BC = nullptr;
  Triangle* near_CA = nullptr;
  bool is_real;
  
  Triangle() = default;
  Triangle(ContourPoint& A, ContourPoint& B, ContourPoint& C)
    : A(A), B(B), C(C)
      , AB(&this->A, &this->B)
      , BC(&this->B, &this->C)
      , CA(&this->C, &this->A)
      , is_real(true) {}

  Triangle(const Triangle& other) 
    : A(other.A), B(other.B), C(other.C)
      , AB(&this->A, &this->B)
      , BC(&this->B, &this->C)
      , CA(&this->C, &this->A)
      , near_AB(other.near_AB)
      , near_BC(other.near_BC)
      , near_CA(other.near_CA)
      , is_real(other.is_real) {}                                                                                                                                            
  Triangle& operator=(const Triangle& other);

  Triangle(Triangle&& other) noexcept;
  Triangle& operator=(Triangle&& other) noexcept;

  ~Triangle() = default;
};

void link_neighbors(Triangle* t1, Triangle* t2);

#endif // FIGURE_H
