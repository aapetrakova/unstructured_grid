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
        cur_num = 0;
      }
      continue;
    }

    std::stringstream ss(line);
    std::string x_str; std::string y_str;

    if (std::getline(ss, x_str, ';') && std::getline(ss, y_str)) {
      try {
        double x = std::stod(x_str);
        double y = std::stod(y_str);
        current_contour.push_back(ContourPoint(x, y, cur_c_num, cur_num));
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

void build_contour_connections(std::vector<std::vector<ContourPoint>>& all_contours) {
  /*
   * Construction of the initial circuit topology.
   *
   * The function accepts as arguments:
      - const std::string& filename -- data file name
   *
   * The function returns nothing
  */

  for (auto& contour : all_contours) {
    if (contour.empty()) continue;

    size_t n = contour.size();
    for (size_t i = 0; i < n; ++i) {
      contour[i].next = &contour[(i + 1) % n];
      contour[i].prev = &contour[(i - 1 + n) % n];
    }
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

