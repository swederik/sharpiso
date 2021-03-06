/// \file isodual3D_datastruct.h
/// Data structure definitions for isodual3D.

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2011 Rephael Wenger

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _ISODUAL3D_DATASTRUCT_
#define _ISODUAL3D_DATASTRUCT_

#include <string>

#include "ijk.txx"
#include "ijkscalar_grid.txx"
#include "ijkvector_grid.txx"
#include "ijkmerge.txx"


#include "isodual3D_types.h"

namespace ISODUAL3D {

  class MERGE_DATA;
  class MULTIRES_GRID;

  // **************************************************
  // GRID DATA STRUCTURES
  // **************************************************

  typedef IJK::GRID_PLUS<int, AXIS_SIZE_TYPE, VERTEX_INDEX, VERTEX_INDEX> 
    ISODUAL_GRID;                  ///< Marching Cubes grid.

  typedef IJK::SCALAR_GRID_BASE<ISODUAL_GRID, SCALAR_TYPE> 
    ISODUAL_SCALAR_GRID_BASE;      ///< isodual base scalar grid.
  typedef IJK::SCALAR_GRID_WRAPPER<ISODUAL_GRID, SCALAR_TYPE>
    ISODUAL_SCALAR_GRID_WRAPPER;   ///< isodual scalar grid wrapper.
  typedef IJK::SCALAR_GRID<ISODUAL_GRID, SCALAR_TYPE> 
    ISODUAL_SCALAR_GRID;           ///< Marching Cubes scalar grid.
  typedef IJK::VECTOR_GRID_BASE<ISODUAL_GRID, LENGTH_TYPE, GRADIENT_TYPE> 
    GRADIENT_GRID_BASE;            ///< isodual base gradient grid
  typedef IJK::VECTOR_GRID<ISODUAL_GRID, LENGTH_TYPE, GRADIENT_TYPE> 
    GRADIENT_GRID;                 ///< isodual gradient grid

  // **************************************************
  // ARRAY DATA STRUCTURES
  // **************************************************

  /// Array of vertices.
  typedef IJK::ARRAY<VERTEX_INDEX> VERTEX_ARRAY;

  // **************************************************
  // DUAL CONTOURING ISOSURFACE CLASS
  // **************************************************

  /// Dual contouring isosurface.
  /// Representation of isosurface 
  ///   returned by Dual Contouring algorithm.
  class DUAL_ISOSURFACE {

  protected:
    VERTEX_INDEX numv_per_isopoly;

  public:
    /// List of isosurface polytope vertices.
    VERTEX_INDEX_ARRAY isopoly_vert;

    /// List of vertex coordinates.
    COORD_ARRAY vertex_coord;

    /// cube_containing_isopoly[i] = cube containing i'th isopoly.
    /// Not always set.
    VERTEX_INDEX_ARRAY cube_containing_isopoly;

  public:
    DUAL_ISOSURFACE(const VERTEX_INDEX numv_per_isopoly) 
      { this->numv_per_isopoly = numv_per_isopoly; };

    VERTEX_INDEX NumVerticesPerIsoPoly() const
      { return(numv_per_isopoly); };

    VERTEX_INDEX NumIsoPoly() const
      { return(isopoly_vert.size()/NumVerticesPerIsoPoly()); };

    void Clear();
  };

  // **************************************************
  // DUAL CONTOURING INPUT DATA AND DATA STRUCTURES
  // **************************************************

  /// dual contouring flags.
  class ISODUAL_DATA_FLAGS {

  protected:
    void Init();

  public:
    // control parameters
    INTERPOLATION_TYPE interpolation_type;
    VERTEX_POSITION_METHOD vertex_position_method;

  public:
    ISODUAL_DATA_FLAGS() { Init(); };
    ~ISODUAL_DATA_FLAGS() { Init(); };
  };

  /// Input data to Dual Contouring and related algorithms
  class ISODUAL_DATA:protected ISODUAL_DATA_FLAGS {

  protected:
    ISODUAL_SCALAR_GRID scalar_grid;
    GRADIENT_GRID gradient_grid;

    // flags
    bool is_scalar_grid_set;
    bool is_gradient_grid_set;

    void Init();
    void FreeAll();

  public:
    ISODUAL_DATA() { Init(); };
    ~ISODUAL_DATA() { FreeAll(); };

    // Set functions
    void CopyScalarGrid             /// Copy scalar_grid to ISODUAL_DATA
      (const ISODUAL_SCALAR_GRID_BASE & scalar_grid2);
    void CopyGradientGrid             /// Copy gradient_grid to ISODUAL_DATA
      (const GRADIENT_GRID_BASE & gradient_grid2);
    void SubsampleScalarGrid        /// Subsample scalar_grid.
      (const ISODUAL_SCALAR_GRID_BASE & scalar_grid2, 
       const int subsample_resolution);
    void SupersampleScalarGrid      /// Supersample scalar_grid.
      (const ISODUAL_SCALAR_GRID_BASE & scalar_grid2, 
       const int supersample_resolution);
    void SetInterpolationType       /// Set type of interpolation.
      (const INTERPOLATION_TYPE interpolation_type);
    void SetVertexPositionMethod    /// Set isosurface vertex position method.
      (const VERTEX_POSITION_METHOD vertex_position_method);

    /// Copy, subsample or supersample scalar grid.
    /// Precondition: flag_subsample and flag_supersample are not both true.
    void SetScalarGrid
      (const ISODUAL_SCALAR_GRID_BASE & full_scalar_grid, 
       const bool flag_subsample, const int subsample_resolution,
       const bool flag_supersample, const int supersample_resolution);

    /// Copy, subsample or supersample scalar and gradient grids.
    /// Precondition: flag_subsample and flag_supersample are not both true.
    void SetGrids
      (const ISODUAL_SCALAR_GRID_BASE & full_scalar_grid,
       const GRADIENT_GRID_BASE & full_gradient_grid,
       const bool flag_subsample, const int subsample_resolution,
       const bool flag_supersample, const int supersample_resolution);

    // Get functions
    bool IsScalarGridSet() const     /// Return true if scalar grid is set.
      { return(is_scalar_grid_set); };
    bool IsGradientGridSet() const   /// Return true if gradient grid is set.
      { return(is_gradient_grid_set); };
    const ISODUAL_SCALAR_GRID_BASE & ScalarGrid() const /// Return scalar_grid
      { return(scalar_grid); };
    const GRADIENT_GRID_BASE & GradientGrid() const     /// Return gradient_grid
      { return(gradient_grid); };

    /// Return interpolation type.
    INTERPOLATION_TYPE InterpolationType() const
      { return(interpolation_type); };

    /// Return isosurface vertex position method.
    VERTEX_POSITION_METHOD VertexPositionMethod() const
      { return(vertex_position_method); };

    /// Check data structure
    bool Check(IJK::ERROR & error) const;
  };


  // **************************************************
  // ISODUAL TIME
  // **************************************************

  /// dual contouring time.
  /// Uses system clock() function to determine time.  
  /// System clock() function should return cpu time, 
  ///   but may return wall time.
  class ISODUAL_TIME {

  public:
    // all times are in seconds
    float preprocessing;  
    // time to create data structure for faster isosurface extraction
    float extract;    // time to extract isosurface mesh
    float merge;      // time to merge identical vertices
    float position;   // time to position isosurface vertices
    float total;      // extract_time+merge_time+position_time

    ISODUAL_TIME();
    void Clear();
    void Add(const ISODUAL_TIME & isodual_time);
  };

  // **************************************************
  // GRID INFO
  // **************************************************

  /// Regular grid information.
  class GRID_INFO {

  public:
    GRID_INFO();                    ///< Constructor.

    VERTEX_INDEX num_cubes;         ///< Number of grid cubes.

    void Clear();                   ///< Clear all data.
  };

  // **************************************************
  // SCALAR INFO
  // **************************************************

  /// Scalar grid information.
  class SCALAR_INFO {

  protected:
    int dimension;

    void Copy(const SCALAR_INFO & info);  ///< Copy scalar info.
    void Init(const int dimension);       ///< Initialize scalar info.
    void FreeAll();                       ///< Free all memory.

  public:
    SCALAR_INFO() { Init(3); };
    SCALAR_INFO(const int dimension) { Init(dimension); };
    ~SCALAR_INFO();
    SCALAR_INFO(const SCALAR_INFO & info) ///< Copy constructor. 
      { Copy(info); };       
    const SCALAR_INFO & operator =        ///< Copy assignment.
      (const SCALAR_INFO & right);

    VERTEX_INDEX num_non_empty_cubes;     ///< Number of cubes containing an isosurface simplex.

    VERTEX_INDEX num_bipolar_edges; ///< Number of bipolar edges.
    //   (number of edges with some vertex value less than the isovalue
    //    and some vertex value greater than or equal to the isovalue)


    // Set functions.
    void SetDimension(const int dimension); ///< Set dimension.

    // Get functions.
    int Dimension() const { return(dimension); };

    void Clear();     // clear all data
  };

  // **************************************************
  // ISODUAL INFO
  // **************************************************

  /// dual contouring information.
  /// Statistical and timing information from the Dual Contouring algorithm.
  class ISODUAL_INFO {

  public:
    GRID_INFO grid;
    SCALAR_INFO scalar;
    ISODUAL_TIME time;

    ISODUAL_INFO();
    ISODUAL_INFO(const int dimension);

    void Clear();     // clear all data
  };

  // **************************************************
  // MERGE DATA
  // **************************************************

  /// Internal data structure for merge_identical_vertices 
  class MERGE_DATA: public IJK::INTEGER_LIST<MERGE_INDEX, MERGE_INDEX> {

  protected:
    MERGE_INDEX num_edges;             ///< Number of edges.
    MERGE_INDEX num_vertices;          ///< Number of vertices.
    MERGE_INDEX num_obj_per_vertex;    ///< Number of objects per vertex.
    MERGE_INDEX num_obj_per_edge;      ///< Number of objects per edge.
    MERGE_INDEX num_obj_per_grid_vertex; ///< Number of objects per grid vertex.
    MERGE_INDEX vertex_id0;            ///< First vertex identifier.

    /// Initialize.
    void Init(const int dimension, const AXIS_SIZE_TYPE * axis_size,
              const MERGE_INDEX num_obj_per_vertex,
              const MERGE_INDEX num_obj_per_edge);

  public:
    MERGE_DATA(const int dimension, const AXIS_SIZE_TYPE * axis_size)
      { Init(dimension, axis_size, 0, 1); };
    MERGE_DATA(const int dimension, const AXIS_SIZE_TYPE * axis_size,
               const MERGE_INDEX num_obj_per_vertex, 
               const MERGE_INDEX num_obj_per_edge)
      { Init(dimension, axis_size, num_obj_per_vertex, num_obj_per_edge); };

    // get functions
    MERGE_INDEX NumEdges() const        /// Number of edges.
      { return(num_edges); };
    MERGE_INDEX NumVertices() const     /// Number of vertices.
      { return(num_vertices); };
    MERGE_INDEX NumObjPerVertex() const { return(num_obj_per_vertex); };
    MERGE_INDEX NumObjPerEdge() const { return(num_obj_per_edge); };
    MERGE_INDEX NumObjPerGridVertex() const
      { return(num_obj_per_grid_vertex); };
    MERGE_INDEX VertexIdentifier       /// Vertex identifier.
      (const MERGE_INDEX iv) const { return(vertex_id0 + iv); };
    MERGE_INDEX VertexIdentifier       /// Vertex identifier.
      (const MERGE_INDEX iv, const MERGE_INDEX j) const 
      { return(vertex_id0 + iv + j * num_vertices); };
    MERGE_INDEX EdgeIdentifier         /// Edge identifier.
      (const MERGE_INDEX ie) const { return(ie); };
    MERGE_INDEX EdgeIdentifier         /// edge identifier
      (const MERGE_INDEX ie, const MERGE_INDEX j) const 
      { return(ie + j * num_edges); };

    /// Get first endpoint of edge containing isosurface vertex isov.
    inline VERTEX_INDEX GetFirstEndpoint(const MERGE_INDEX isov) const
      { return(isov/NumObjPerGridVertex()); };

    /// Get direction of edge containing isosurface vertex isov.
    inline MERGE_INDEX GetEdgeDir(const MERGE_INDEX isov) const
      { return(isov%NumObjPerGridVertex()); };

    bool Check(IJK::ERROR & error) const;     ///< Check allocated memory.
  };

  /// Merge data structure for isosurface vertices.
  /// Vertices are identified by a single integer.
  class ISO_MERGE_DATA: public MERGE_DATA {
  public:
    ISO_MERGE_DATA(const int dimension, const AXIS_SIZE_TYPE * axis_size):
      MERGE_DATA(dimension, axis_size, 1, 0) {};
  };

  // **************************************************
  // MESH VERTEX LIST
  // **************************************************

  /// List of mesh vertices.
  template <class DTYPE, class CTYPE, class STYPE>
    class MESH_VERTEX_LIST {

    protected:
    DTYPE dimension;        ///< Dimension.

    /// Coordinates of mesh vertices.
    /// coord[i*dimension+j] = j'th coordinate of i'th mesh vertex.
    std::vector<CTYPE> coord;
    std::vector<STYPE> scalar;   ///< scalar[i] = scalar value of i'th mesh vertex.

    public:
    MESH_VERTEX_LIST(const DTYPE dim) { this->dimension = dim; };

    // Get functions.
    DTYPE Dimension() const { return(dimension); } ///< Return dimension.

    /// Return j'th coordinate of i'th mesh vertex.
    CTYPE Coord(const int i, const int j) const
      { return(coord[i*Dimension()+j]); };

    /// Copy coordinates of \a i'th mesh vertex into array \a coord[].
    /// @pre coord[] is preallocated to length at least \a dimension.
    template <class CTYPE2>
      void CopyCoord(const int i, CTYPE2 * coord2) const
      {
        const CTYPE * coord_begin = &(coord[i*Dimension()]);
        std::copy(coord_begin, coord_begin+Dimension(), coord2);
      }

    /// Return scalar value of i'th mesh vertex.
    STYPE Scalar(const int i) const { return(scalar[i]); };

    int NumVertices() const     ///< Return number of vertices in list.
      { return(scalar.size()); };

    // Set functions.

    void Add()                 ///< Add a vertex to the list.
      {
        const int k = coord.size();
        coord.resize(k+Dimension());
        scalar.push_back(0);
      }

    void SetScalar(const int i, const STYPE s)  ///< Set scalar of vertex i.
      { scalar[i] = s; };

    /// Set j'th coordinate of vertex i.
    void SetCoord(const int i, const int j, const CTYPE c)
      { coord[i*Dimension()+j] = c; };

    /// Set coordinates of vertex i
    template <class CTYPE2>
      void SetCoord(const int i, CTYPE2 * coord2)
      {
        for (DTYPE d = 0; d < Dimension(); d++)
          { SetCoord(i, d, coord2[d]); };
      }

  };

  typedef MESH_VERTEX_LIST<int, COORD_TYPE, SCALAR_TYPE> 
    ISODUAL_MESH_VERTEX_LIST;    ///< Dual Contouring mesh vertex list.
}

#endif
