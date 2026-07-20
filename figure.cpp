#include "figure.h"

/*
 * Edge compare operators
*/
bool operator==(const Edge& e1, const Edge& e2){
  if (!e1.start || !e1.end || !e2.start || !e2.end) {
    throw std::runtime_error("Err: can not compare edges with nullptr pointers");
  } 
                                                       
  bool direct = (e1.start->num == e2.start->num && e1.end->num == e2.end->num);
  bool reverse = (e1.start->num == e2.end->num && e1.end->num == e2.start->num);
                                                       
  // regardless of direction (geometric neighbors)   
  return direct || reverse;                                                                               
}                                                    
bool operator!=(const Edge& e1, const Edge& e2) {
  if (!e1.start || !e1.end || !e2.start || !e2.end) {
    throw std::runtime_error("Err: can not compare edges with nullptr pointers");
  }                          
  return !(e1 == e2);
}               
bool operator<(const Edge& e1, const Edge& e2) {
  if (!e1.start || !e1.end || !e2.start || !e2.end) {    
    throw std::runtime_error("Err: can not compare edges with nullptr pointers");
  }
  auto min_max1 = std::make_pair(std::min(e1.start->num, e1.end->num), std::max(e1.start->num, e1.end->num));
  auto min_max2 = std::make_pair(std::min(e2.start->num, e2.end->num), std::max(e2.start->num, e2.end->num));
  
  return min_max1 < min_max2;
}        
bool operator>=(const Edge& e1, const Edge& e2) {
  if (!e1.start || !e1.end || !e2.start || !e2.end) {
    throw std::runtime_error("Err: can not compare edges with nullptr pointers");
  }      
  return !(e1 < e2);
}        
bool operator>(const Edge& e1, const Edge& e2) {
  if (!e1.start || !e1.end || !e2.start || !e2.end) {
    throw std::runtime_error("Err: can not compare edges with nullptr pointers");
  } 
  return !(e1 < e2) && !(e1 == e2);
}                             
bool operator<=(const Edge& e1, const Edge& e2) {
  if (!e1.start || !e1.end || !e2.start || !e2.end) {
    throw std::runtime_error("Err: can not compare edges with nullptr pointers");
  } 
  return (e1 < e2) || (e1 == e2);
} 

/*
 * Triangle
*/
Triangle& Triangle::operator=(const Triangle& other) {
  if (this == &other) return *this;

  A = other.A; B = other.B; C = other.C;

  near_AB = other.near_AB; near_BC = other.near_BC; near_CA = other.near_CA;

  AB.next = other.AB.next; AB.prev = other.AB.prev;
  BC.next = other.BC.next; BC.prev = other.BC.prev;
  CA.next = other.CA.next; CA.prev = other.CA.prev;
  
  is_real = other.is_real;
  return *this;
}

Triangle::Triangle(Triangle&& other) noexcept
  : A(std::move(other.A)), B(std::move(other.B)), C(std::move(other.C))
    , AB(&this->A, &this->B)
    , BC(&this->B, &this->C)
    , CA(&this->C, &this->A)
    , near_AB(other.near_AB)
    , near_BC(other.near_BC)
    , near_CA(other.near_CA) {
  AB.next = other.AB.next; AB.prev = other.AB.prev;
  BC.next = other.BC.next; BC.prev = other.BC.prev;
  CA.next = other.CA.next; CA.prev = other.CA.prev;

  other.near_AB = other.near_BC = other.near_CA = nullptr;
}
Triangle& Triangle::operator=(Triangle&& other) noexcept {
  if (this == &other) return *this;

  A = std::move(other.A); B = std::move(other.B); C = std::move(other.C);

  near_AB = other.near_AB; near_BC = other.near_BC; near_CA = other.near_CA;

  AB.next = other.AB.next; AB.prev = other.AB.prev;
  BC.next = other.BC.next; BC.prev = other.BC.prev;
  CA.next = other.CA.next; CA.prev = other.CA.prev;

  other.near_AB = other.near_BC = other.near_CA = nullptr;

  is_real = other.is_real;
  return *this;
}


/*
 * Function for the geometric stitching of two triangles sharing a common edge.
*/
void link_neighbors(Triangle* t1, Triangle* t2) {
  if (!t1 || !t2) return;

  // check AB
  if (t1->AB == t2->AB) { t1->near_AB = t2; t2->near_AB = t1; return; }
  if (t1->AB == t2->BC) { t1->near_AB = t2; t2->near_BC = t1; return; }
  if (t1->AB == t2->CA) { t1->near_AB = t2; t2->near_CA = t1; return; }

  // check BC
  if (t1->BC == t2->AB) { t1->near_BC = t2; t2->near_AB = t1; return; }
  if (t1->BC == t2->BC) { t1->near_BC = t2; t2->near_BC = t1; return; }
  if (t1->BC == t2->CA) { t1->near_BC = t2; t2->near_CA = t1; return; }

  // check CA
  if (t1->CA == t2->AB) { t1->near_CA = t2; t2->near_AB = t1; return; }
  if (t1->CA == t2->BC) { t1->near_CA = t2; t2->near_BC = t1; return; }
  if (t1->CA == t2->CA) { t1->near_CA = t2; t2->near_CA = t1; return; }
}
