#ifndef FIGURE_H
#define FIGURE_H

#include <memory>

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
  
  Triangle() = default;
  Triangle(ContourPoint& A, ContourPoint& B, ContourPoint& C)
    : A(A), B(B), C(C)
      , AB(&this->A, &this->B)
      , BC(&this->B, &this->C)
      , CA(&this->C, &this->A) {}

  Triangle(const Triangle& other) 
    : A(other.A), B(other.B), C(other.C)
      , AB(&this->A, &this->B)
      , BC(&this->B, &this->C)
      , CA(&this->C, &this->A)
      , near_AB(other.near_AB)
      , near_BC(other.near_BC)
      , near_CA(other.near_CA) {}                                                                                                                                            
  Triangle& operator=(const Triangle& other);

  Triangle(Triangle&& other) noexcept;
  Triangle& operator=(Triangle&& other) noexcept;

  ~Triangle() = default;
};

void link_neighbors(Triangle* t1, Triangle* t2);

#endif // FIGURE_H
