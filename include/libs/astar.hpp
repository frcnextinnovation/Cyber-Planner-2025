#ifndef ASTAR_HPP
#define ASTAR_HPP

#include <eigen3/Eigen/Eigen>

#include "log.hpp"

namespace nextinnovation {
#define OBSTACLE_OFFSET 100.0
#define OBSTACLE_FIELD_REDUCTION 0.7
#define ASTAR_HEURISTIC_COEFFICIENT 1e-3

bool astar(const std::vector<std::vector<bool>>& grid_map,
           const Eigen::Vector2i& start, const Eigen::Vector2i& goal,
           std::vector<Eigen::Vector2i>& path,
           std::vector<Eigen::Vector2i>& visited) {
  log_debug("A* algorithm started.");
  auto start_time = std::chrono::high_resolution_clock::now();

  int n = grid_map.size(), m = grid_map[0].size();
  std::vector<std::vector<int>> g(n, std::vector<int>(m, 0x3f3f3f3f));
  std::vector<std::vector<Eigen::Vector2i>> parent(
      n, std::vector<Eigen::Vector2i>(m, Eigen::Vector2i(-1, -1)));

  std::vector<Eigen::Vector2i> dir = {
      Eigen::Vector2i(0, 1), Eigen::Vector2i(1, 0), Eigen::Vector2i(0, -1),
      Eigen::Vector2i(-1, 0), Eigen::Vector2i(1, 1), Eigen::Vector2i(1, -1),
      Eigen::Vector2i(-1, 1), Eigen::Vector2i(-1, -1)};

  std::multimap<double, Eigen::Vector2i> open;
  Eigen::Vector2i current;
  bool found = false;

  g[start(0)][start(1)] = 0;
  parent[start(0)][start(1)] = start;
  open.insert(std::make_pair((goal - start).lpNorm<2>(), start));
  while (!open.empty()) {
    current = open.begin()->second;
    open.erase(open.begin());
    visited.push_back(current);
    if (current == goal) {
      found = true;
      break;
    }
    for (auto& d : dir) {
      Eigen::Vector2i next = current + d;
      // out of bounds or obstacle
      if (next(0) < 0 || next(0) >= n || next(1) < 0 || next(1) >= m ||
          grid_map[next(0)][next(1)]) {
        continue;
      }

      if (g[next(0)][next(1)] < 0x3f3f3f3f) {
        // current point has been visited
        if (g[next(0)][next(1)] >
            g[current(0)][current(1)] + d.lpNorm<2>() +
                (parent[current(0)][current(1)] + next - 2 * current)
                    .lpNorm<1>()) {
          // the new path is shorter
          g[next(0)][next(1)] =
              g[current(0)][current(1)] + d.lpNorm<2>() +
              (parent[current(0)][current(1)] + next - 2 * current).lpNorm<1>();
          parent[next(0)][next(1)] = current;
          continue;
        }
      } else {
        // current point has not been visited
        g[next(0)][next(1)] =
            g[current(0)][current(1)] + d.lpNorm<2>() +
            (parent[current(0)][current(1)] + next - 2 * current).lpNorm<1>();
        parent[next(0)][next(1)] = current;
        open.insert(std::make_pair(
            g[next(0)][next(1)] + (goal - next).lpNorm<1>(), next));
      }
    }
  }

  path.clear();
  if (found) {
    while (current != start) {
      path.push_back(current);
      current = parent[current(0)][current(1)];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return true;
    log_debug("A* algorithm succeeded, time elapsed: %d.%dms",
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::high_resolution_clock::now() - start_time)
                  .count(),
              std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now() - start_time)
                      .count() %
                  1000);
    return true;
  }
  log_debug("A* algorithm failed, time elapsed: %d.%dms",
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start_time)
                .count(),
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start_time)
                    .count() %
                1000);
  return false;
}

bool astar(const std::vector<std::vector<double>>& grid_map,
           const Eigen::Vector2i& start, const Eigen::Vector2i& goal,
           std::vector<Eigen::Vector2i>& path,
           std::vector<Eigen::Vector2i>& visited) {
  log_debug("A* algorithm started.");
  auto start_time = std::chrono::high_resolution_clock::now();

  int n = grid_map.size(), m = grid_map[0].size();
  std::vector<std::vector<double>> g(n, std::vector<double>(m, 0x3f3f3f3f));
  std::vector<std::vector<Eigen::Vector2i>> parent(
      n, std::vector<Eigen::Vector2i>(m, Eigen::Vector2i(-1, -1)));
  std::vector<Eigen::Vector2i> dir = {
      Eigen::Vector2i(0, 1), Eigen::Vector2i(1, 0), Eigen::Vector2i(0, -1),
      Eigen::Vector2i(-1, 0), Eigen::Vector2i(1, 1), Eigen::Vector2i(1, -1),
      Eigen::Vector2i(-1, 1), Eigen::Vector2i(-1, -1)};

  std::multimap<double, Eigen::Vector2i> open;
  Eigen::Vector2i current;
  bool found = false;

  g[start(0)][start(1)] = 0;
  parent[start(0)][start(1)] = start;
  open.insert(std::make_pair((goal - start).lpNorm<2>(), start));

  while (!open.empty()) {
    current = open.begin()->second;
    open.erase(open.begin());
    visited.push_back(current);
    if (current == goal) {
      path.clear();
      while (current != start) {
        path.push_back(current);
        current = parent[current(0)][current(1)];
      }
      path.push_back(start);
      std::reverse(path.begin(), path.end());
      break;
    }

    for (auto& d : dir) {
      Eigen::Vector2i next = current + d;
      // out of bounds or obstacle
      if (next(0) < 0 || next(0) >= n || next(1) < 0 || next(1) >= m ||
          grid_map[next(0)][next(1)] > OBSTACLE_OFFSET - .01) {
        continue;
      }

      if (g[next(0)][next(1)] < 0x3f3f3f3f) {
        // current point has been visited
        if (g[next(0)][next(1)] > g[current(0)][current(1)] +
                                      grid_map[current(0)][current(1)] +
                                      d.lpNorm<1>()) {
          // the new path is shorter
          g[next(0)][next(1)] = g[current(0)][current(1)] +
                                grid_map[current(0)][current(1)] +
                                d.lpNorm<1>();
          parent[next(0)][next(1)] = current;
          continue;
        }
      } else {
        // current point has not been visited
        g[next(0)][next(1)] = g[current(0)][current(1)] +
                              grid_map[current(0)][current(1)] +
                              d.lpNorm<1>();
        parent[next(0)][next(1)] = current;
        open.insert(
            std::make_pair(g[next(0)][next(1)] + ASTAR_HEURISTIC_COEFFICIENT *
                                                     (goal - next).lpNorm<2>(),
                           next));
      }
    }
  }
  if (path.empty()) {
    log_debug("A* algorithm failed, time elapsed:%d.%3dms",
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::high_resolution_clock::now() - start_time)
                  .count(),
              std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now() - start_time)
                      .count() %
                  1000);
    return false;
  } else {
    log_debug("A* algorithm succeeded, time elapsed: %d.%3dms",
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::high_resolution_clock::now() - start_time)
                  .count(),
              std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now() - start_time)
                      .count() %
                  1000);
    return true;
  }
}

void samplePath(const std::vector<Eigen::Vector2i>& path,
                std::vector<Eigen::Vector2i>& sampled_path, int step = 3) {
  sampled_path.clear();
  int n = path.size();
  int remainder = n % step;
  if (remainder == 1) {
    for (int i = 0; i < n; i += step) {
      sampled_path.push_back(path[i]);
    }
    return;
  }
  if (remainder == 0) remainder = step;
  sampled_path.push_back(path[0]);
  for (int i = remainder / 2; i < n - 1; i += step) {
    sampled_path.push_back(path[i]);
  }
  sampled_path.push_back(path[n - 1]);
}

}  // namespace nextinnovation
#endif  // ASTAR_HPP