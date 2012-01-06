/*
 *  sh_point_datastruct.h
 *  SHARPPOINT
 *
 *  Created by arindam bhattacharya on 11/6/11.
 *  Copyright 2011 Ohio State University. All rights reserved.
 *
 */

#ifndef _cube_
#define _cube_
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
const double TOLERANCE = 0.0001;
namespace sh_cube {
  //point
  class POINT {
  public:
    double scalar;
    double grads[3];
    double pos[3];
    void print_point();
    void normalize_grads();
  };
  //edge
  class EDGE {
  public:
    POINT p1,p2;
    POINT pt_intersect;
    bool is_intersect;
  };
  //cube 
  class CUBE {
  public:
    int num_edges, num_pts, dim, ne_intersect;
    vector<POINT> pts;
    vector<EDGE> edges;
    int intersects_ind[12];
  };
  
};
#endif