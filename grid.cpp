#include "grid.h"

/*
 * Functions for reading input files
*/
std::vector<std::vector<ContourPoint>> read_contour_csv(const std::string& filename) {
  /*
   * Reading a CSV file describing contours:
      - The first is the outer contour with a POSITIVE traversal direction
      - Next are the points of the inner contour with a NEGATIVE traversal direction
      - The separator is the '?' symbol
   *
   * The function accepts as arguments:
      - const std::string& filename -- data file name
   * 
   * The function returns a vector containing all contours (in vector format).
  */

  std::vector<std::vector<ContourPoint>> all_contours;
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Err: cannot open the file " << filename << std::endl;
    return all_contours;
  }

  std::string line;
  std::vector<ContourPoint> current_contour;

  size_t cur_c_num = 0;
  size_t cur_num = 0;

  while(std::getline(file, line)) {
    if (line.empty()) { continue; }
    if (line == "?") {
      if (!current_contour.empty()) {
        all_contours.push_back(std::move(current_contour));
        current_contour.clear();

        ++cur_c_num;
      }
      continue;
    }

    std::stringstream ss(line);
    std::string x_str; std::string y_str;

    if (std::getline(ss, x_str, ';') && std::getline(ss, y_str)) {
      try {
        double x = std::stod(x_str);
        double y = std::stod(y_str);
        current_contour.push_back(ContourPoint(x, y, true, cur_c_num, cur_num));
        ++cur_num;
      } catch (const std::invalid_argument& e) {
        std::cerr << "Err: invalid argument in contour file, str: " << line << std::endl;
      }
    }
  }

  if (!current_contour.empty()) {
    all_contours.push_back(std::move(current_contour));
  }

  file.close();

  return all_contours;
}

void build_contour_connections(std::vector<ContourPoint>& contour) {
  /*
   * Construction of the initial circuit topology.
   *
   * The function accepts as arguments:
      - const std::vector<ContourPoint>& contour -- some contour
   *
   * The function returns nothing
  */
  if (contour.empty()) {
    std::cerr << "Warning: contour is empty" << std::endl;
  }

  size_t n = contour.size();
  for (size_t i = 0; i < n; ++i) {
    contour[i].next = &contour[(i + 1) % n];
    contour[i].prev = &contour[(i - 1 + n) % n];
  }
}

std::vector<Point> read_throw_in_points_csv(const std::string& filename) {
  /*
     * Reading a CSV file describing throw-in points:
     *
     * The function accepts as arguments:
        - const std::string& filename -- data file name
     *
     * The function returns a vector containing throw-in points.
    */

  std::vector<Point> points;
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Err: cannot open the file " << filename << std::endl;
    return points;
  }

  std::string line;

  while (std::getline(file, line)) {
    if (line.empty()) continue;

    std::stringstream ss(line);
    std::string x_str, y_str;

    if (std::getline(ss, x_str, ';') && std::getline(ss, y_str)) {
      try {
        double x = std::stod(x_str);
        double y = std::stod(y_str);
        points.emplace_back(Point(x, y));
      }
      catch (const std::invalid_argument& e) {
        std::cerr <<  "Err: invalid argument in throw-in points file, str: " << line << std::endl;
      }
    }
  }

  file.close();
  return points;
}


/*
   * Hard triangle grid building
  */
  // Area preparation
double Z(ContourPoint P, ContourPoint Q, ContourPoint R) {
  return (Q.x - P.x)*(R.y - P.y) - (Q.y - P.y)*(R.x - P.x);
}

bool check_edge_intersection(const std::set<Edge>& edges, Edge AB) {
  /*
   * Checks for intersections between the given edge and the other specified edges, excluding adjacent edges
   * The function accepts as arguments:
        - std::set<Edge>& edges -- a set of edges with which we are seeking an intersection
        - Edge AB -- target edge
   * The function returns a Boolean flag with a value of 0 if there is no intersection, and 1 if there is
  */
  ContourPoint* A = AB.start;
  ContourPoint* B = AB.end;
  if (!A || !B) {return false; }

  double d1, d2, d3, d4;

  for (auto& edge : edges) {
    ContourPoint* C = edge.start;
    ContourPoint* D = edge.end;
    if (!C || !D) { continue; }
    
    // checking for edge coincidence and adjacency
    if ((A->num == C->num && B->num == D->num) || (A->num == D->num && B->num == C->num)) { continue; }
    if (A->num == C->num || A->num == D->num || B->num == C->num || B->num == D->num) { continue; }

    // fast checking
    bool intersect_X = (std::min(A->x, B->x) <= std::max(C->x, D->x)) &&
      (std::min(C->x, D->x) <= std::max(A->x, B->x));
    bool intersect_Y = (std::min(A->y, B->y) <= std::max(C->y, D->y)) &&
      (std::min(C->y, D->y) <= std::max(A->y, B->y));

    if (!intersect_X || !intersect_Y) { continue; }

    // strong checking
    d1 = Z(*A, *B, *C);
    d2 = Z(*A, *B, *D);
    d3 = Z(*C, *D, *A);
    d4 = Z(*C, *D, *B);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) { return true;}

    // if one of the vertices lies on the edge
    constexpr double EPS = 1e-9;
    if (std::fabs(d1) < EPS && std::min(A->x, B->x) <= C->x && C->x <= std::max(A->x, B->x) && std::min(A->y, B->y) <= C->y && C->y <= std::max(A->y, B->y)) { return true; }
    if (std::fabs(d2) < EPS && std::min(A->x, B->x) <= D->x && D->x <= std::max(A->x, B->x) && std::min(A->y, B->y) <= D->y && D->y <= std::max(A->y, B->y)) { return true; }
    if (std::fabs(d3) < EPS && std::min(C->x, D->x) <= A->x && A->x <= std::max(C->x, D->x) && std::min(C->y, D->y) <= A->y && A->y <= std::max(C->y, D->y)) { return true; }
    if (std::fabs(d4) < EPS && std::min(C->x, D->x) <= B->x && B->x <= std::max(C->x, D->x) && std::min(C->y, D->y) <= B->y && B->y <= std::max(C->y, D->y)) { return true; }
  }
  return false;
}

void circ_shift(const ContourPoint& A, std::vector<ContourPoint>& contour) {
  /*
   * Cyclically shift the contour points until the given point becomes the first one. Without preserving pointers.
   * The function accepts as arguments:
        - const ContourPoint& A -- target point
        - std::vector<ContourPoint>& contour -- target contour
   * The function returns nothing
  */
  size_t target_num = A.num;
  auto it = std::find_if(contour.begin(), contour.end(), [target_num](const ContourPoint& pt) {
    return pt.num == target_num;
  });

  if (it != contour.end() && it != contour.begin()) {
    std::rotate(contour.begin(), it, contour.end());
  }
}

std::vector<ContourPoint> prepare_area(const std::vector<std::vector<ContourPoint>>& all_contours) {
  /*
   * Transforming the initial domain into a simply connected one by adding internal contours to the external contour.
   * The function accepts as arguments:
        - const std::vector<std::vector<ContourPoint>>& all_contours -- vector with all contours (const, because we cannot modify this vector without problems with pointers)
   * The function returns simple_points -- a new contour defining a simply connected region
  */
  if (all_contours.empty()) return {};

  std::set<Edge> simple_edges;
  std::vector<ContourPoint> simple_points = all_contours[0];

  size_t total_max_points = 0;
  for (const auto& contour : all_contours) {
    total_max_points += contour.size();
  }
  total_max_points += (all_contours.size() - 1) * 2;
  simple_points.reserve(total_max_points);

  size_t n = simple_points.size();
  for (size_t i = 0; i < n; ++i) {
    simple_points[i].next = &simple_points[(i + 1) % n];
    simple_points[i].prev = &simple_points[(i - 1 + n) % n];
    simple_edges.insert(Edge(&simple_points[i], simple_points[i].next));
  }

  for (const auto& contour : all_contours) {
    if (contour.empty() || contour[0].c_num == 0) { continue; }

    bool find_no_intersec_edge = false;
    size_t found_A_idx = 0;
    ContourPoint found_B_point;

    for (auto& B : const_cast<std::vector<ContourPoint>&>(contour)) {
      for (size_t i = 0; i < simple_points.size(); ++i) {
        auto& A = simple_points[i];
        Edge cur_edge(&A, &B);

        if (!check_edge_intersection(simple_edges, cur_edge)) {
          find_no_intersec_edge = true;
          found_A_idx = i;
          found_B_point = B;
          break;
        }
      }
      if (find_no_intersec_edge) { break; }
    }

    if (find_no_intersec_edge) {
      std::vector<ContourPoint> cur_contour = contour;
      circ_shift(found_B_point, cur_contour);

      auto& real_A = simple_points[found_A_idx];

      ContourPoint A_copy(real_A);
      ContourPoint B_copy(found_B_point);
      A_copy.is_real = false; B_copy.is_real = false;

      std::vector<ContourPoint> insertion_block = cur_contour;
      insertion_block.push_back(B_copy);
      insertion_block.push_back(A_copy);

      auto it = simple_points.begin() + found_A_idx;
      simple_points.insert(it + 1, insertion_block.begin(), insertion_block.end());

      simple_edges.clear();
      for (size_t i = 0; i < simple_points.size(); ++i) {
        simple_points[i].next = &simple_points[(i + 1) % simple_points.size()];
        simple_edges.insert(Edge(&simple_points[i], simple_points[i].next));
      }

      for (size_t i = 0; i < contour.size(); ++i) {
        simple_edges.insert(Edge(const_cast<ContourPoint*>(&contour[i]), const_cast<ContourPoint*>(&contour[(i + 1) % contour.size()])));
      }
    }
  }

  build_contour_connections(simple_points);
  return simple_points;
}

// first step
bool check_available_triangle(std::vector<ContourPoint>& simple_points, ContourPoint& A, ContourPoint& B, ContourPoint& C) {
  double d1, d2, d3;
  for (const auto& D : simple_points) {
    if (D.num == A.num || D.num == B.num || D.num == C.num) { continue; }

    d1 = Z(D, A, B);
    d2 = Z(D, B, C);
    d3 = Z(D, C, A);

    if (d1 == 0 || d2 == 0 || d3 == 0) { return false; }
    if (!(d1*d2 < 0 || d2*d3 < 0 || d3*d1 < 0)) { return false; }
  }

  return true;
}

void link_all_triangles(std::vector<Triangle>& triangles) {
  std::map<Edge, Triangle*> edge_to_triangle_map;
  size_t len = triangles.size();

  for (size_t i = 0; i < len; ++i) {
    Triangle* cur = &triangles[i];

    Edge* cur_edges[3] = {&cur->AB, &cur->BC, &cur->CA};
    for(int j = 0; j < 3; ++j) {
      Edge* edge = cur_edges[j];
      auto it = edge_to_triangle_map.find(*edge);

      if (it != edge_to_triangle_map.end()) {
        Triangle* neighbor = it->second;
        link_neighbors(cur, neighbor);
        edge_to_triangle_map.erase(it);
      } else {
        edge_to_triangle_map[*edge] = cur;
      }
    }
  }
}

std::vector<Triangle> hard_grid_step_1(std::vector<ContourPoint>& simple_points) {
  if (simple_points.size() < 3) { return {}; }

  std::vector<Triangle> hard_triangles;
  hard_triangles.reserve(simple_points.size() - 2);

  size_t num_of_available_points = simple_points.size();
  ContourPoint* A = &simple_points[0];

  while (num_of_available_points != 2) {
    ContourPoint* B = A->next;
    ContourPoint* C = B->next;

    if((Z(*B, *A, *C) < 0) && check_available_triangle(simple_points,*A, *B, *C)) {
      hard_triangles.emplace_back(*A, *B, *C);
      A->next = C;
      C->prev = A;

      --num_of_available_points;
      A = C;
    } else {
      A = B;
    }
  }
  return hard_triangles;
}
std::vector<Triangle> hard_grid_step_2(std::vector<ContourPoint>& simple_points) {
  if (simple_points.size() < 3) { return {}; }
  
  std::vector<Triangle> fake_triangles;
  fake_triangles.reserve(simple_points.size() - 2);
  
  size_t num_of_available_points = simple_points.size();
  ContourPoint* A = &simple_points[0];
  
  size_t failed_attempts_in_a_row = 0;
  
  while (num_of_available_points != 2) {
    ContourPoint* B = A->next;
    ContourPoint* C = B->next;
  
    if((Z(*B, *A, *C) > 0) && check_available_triangle(simple_points,*A, *B, *C)) {
      fake_triangles.emplace_back(*A, *B, *C);
      fake_triangles.back().is_real = false;
      A->next = C;
      C->prev = A;
  
      --num_of_available_points;
      A = C;
      failed_attempts_in_a_row = 0;
    } else {
      A = B;
      ++failed_attempts_in_a_row;
    }
    if (failed_attempts_in_a_row >= num_of_available_points) {
      break;
    }
  }
  return fake_triangles;
}
bool delone_condition(const Triangle& ABC, const ContourPoint* D) {
  double x1 = ABC.A.x - D->x; double y1 = ABC.A.y - D->y;
  double x2 = ABC.B.x - D->x; double y2 = ABC.B.y - D->y;
  double x3 = ABC.C.x - D->x; double y3 = ABC.C.y - D->y;

  double s1 = x1*x1 + y1*y1;
  double s2 = x2*x2 + y2*y2;
  double s3 = x3*x3 + y3*y3;

  double a = x1*(y2 - y3) - y1*(x2 - x3) + (x2*y3 - y2*x3);
  double d = s1 * (x2*y3 - x3*y2) - x1 * (s2*y3 - s3*y2) + y1 * (s2*x3 - s3*x2)  ;

  double sign_a = (a >= 0) ? 1.0 : -1.0;

  return (d*sign_a) <= 0;
}

std::vector<Triangle> make_hard_grid(const std::vector<std::vector<ContourPoint>>& all_contours) {
  std::vector<ContourPoint> simple_points = prepare_area(all_contours);
  std::vector<ContourPoint> simple_points_copy = simple_points;
  std::vector<Triangle> grid = hard_grid_step_1(simple_points);
  std::vector<Triangle> step2 = hard_grid_step_2(simple_points_copy);
  grid.insert(grid.end(), step2.begin(), step2.end());
  link_all_triangles(grid);
  // вставить перестройку по Делоне
  return grid;
}
