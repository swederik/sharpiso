/// \file anisograd.cxx
/// compute gradients from scalar data
/// Version 0.0.1

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

#include "anisograd.h"
#include "ijkscalar_grid.txx"
#include "isodual3D_datastruct.h"
#include "anisograd_operators.h"

using namespace ISODUAL3D;

// local type definition
namespace {
    
    typedef IJK::BOOL_GRID_BASE<ISODUAL_GRID> BOOL_GRID_BASE;
    typedef IJK::BOOL_GRID<ISODUAL_GRID> BOOL_GRID;
    
};

// help routines 


// Normalize the vectors.
void normalize (float *vec, const int num_elements);



// local routines
void compute_gradient_central_difference
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 GRADIENT_TYPE * gradient);

// Compute central difference in one direction and return a single value
void compute_central_difference_d
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d,
 GRADIENT_TYPE &cntrl_diff_d);





// Computes the central difference in the d th dim 
// this is for the normals
void compute_central_difference_d
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d,
 const int index_coord,
 GRADIENT_TYPE &cntrl_diff_d);





void compute_boundary_gradient
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1, GRADIENT_TYPE * gradient);

void compute_grad_H_d
( const ISODUAL_SCALAR_GRID_BASE & scalar_grid, const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1, const DIRECTION d, const int index_coord, GRADIENT_TYPE *gradient);

// the stand alone version which computes m and k 
// for a particular vertex for a partiuclar direction 

void compute_m_k_d
(const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 const GRADIENT_GRID_BASE & gradient_grid,
 const int direction,
 GRADIENT_TYPE Mx[3],
 GRADIENT_TYPE &Kx
 );


// Compute central difference main function 
void compute_gradient_central_difference
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 GRADIENT_GRID & gradient_grid)
{
    const int dimension = scalar_grid.Dimension();
    
    gradient_grid.SetSize(scalar_grid, dimension);
    
    BOOL_GRID boundary_grid;
    boundary_grid.SetSize(scalar_grid);
    compute_boundary_grid(boundary_grid);
    
    for (VERTEX_INDEX iv = 0; iv < scalar_grid.NumVertices(); iv++) {
        if (boundary_grid.Scalar(iv)) {
            compute_boundary_gradient(scalar_grid, iv, gradient_grid.VectorPtr(iv));
        }
        else {
            compute_gradient_central_difference
            (scalar_grid, iv, gradient_grid.VectorPtr(iv));
        }
    }
}



// Compute anisotropic filtering of gradients main function 
using namespace std;

// Compute Gd
void compute_grad_H_d
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d,
 GRADIENT_TYPE * gradient)
{
    const int dimension = scalar_grid.Dimension();
    GRADIENT_TYPE   temp0, temp1;
    for (int i=0; i<dimension; i++) {
        if (i==d) {
            compute_forward_difference_d(scalar_grid, iv1, i, gradient[i]);
        }
        else{
            compute_central_difference_d(scalar_grid, iv1, i, temp0);
            VERTEX_INDEX iv2 = scalar_grid.NextVertex(iv1, d);
            compute_central_difference_d(scalar_grid, iv2, i, temp1);
            gradient[i] = (temp0 + temp1)/2.0;
        }
    }
}


// this is for the normals [help function for Gnd]
void compute_grad_H_d
( const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d,
 const int index_coord,
 GRADIENT_TYPE *gradient)

{
    const int dimension  = gradient_grid.Dimension();
    GRADIENT_TYPE   temp0, temp1;
    for (int i=0; i<dimension; i++) {
        if (i==d) {
            compute_forward_difference_d(scalar_grid, gradient_grid, iv1, i, index_coord, gradient[i]);
        }
        else{
            compute_central_difference_d(scalar_grid, gradient_grid, iv1, i, index_coord, temp0);
            
            VERTEX_INDEX iv2 = gradient_grid.NextVertex(iv1, d);
            compute_central_difference_d(scalar_grid, gradient_grid, iv2, i, index_coord, temp1);
            
            gradient[i] = (temp0 + temp1)/2.0;
        }
        
    }
}


// compute GNd
void compute_grad_H_d
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid, 
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d,
 GRADIENT_TYPE * gradient)
{
    // This generates a  3X3 matrix
    const int dimension = scalar_grid.Dimension();
    for (int index_coord=0; index_coord<dimension; index_coord++) {
        
        compute_grad_H_d (scalar_grid, gradient_grid, iv1, d, index_coord, gradient + 3*index_coord);
    }
}

void compute_gradient_central_difference
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 GRADIENT_TYPE * gradient)
{
    const int dimension = scalar_grid.Dimension();
    for (int d = 0; d < dimension; d++) {
        VERTEX_INDEX iv0 = scalar_grid.PrevVertex(iv1, d);
        VERTEX_INDEX iv2 = scalar_grid.NextVertex(iv1, d);
        gradient[d] = (scalar_grid.Scalar(iv2) - scalar_grid.Scalar(iv0))/2;
    }
}







// Computes the backward diff in the d th dim
void compute_backward_difference_d
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d, 
 GRADIENT_TYPE &bkwd_diff_d)
{
    const int dimension = scalar_grid.Dimension();
    VERTEX_INDEX iv0 = scalar_grid.PrevVertex(iv1, d);
    bkwd_diff_d = scalar_grid.Scalar(iv1) - scalar_grid.Scalar(iv0);
};


void compute_central_difference_d
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1, 
 const DIRECTION d,
 const int index_coord,
 GRADIENT_TYPE &cntrl_diff_d)
{
    const int dimension = scalar_grid.Dimension();
    VERTEX_INDEX iv0 = scalar_grid.PrevVertex(iv1, d);
    VERTEX_INDEX iv2 = scalar_grid.NextVertex(iv1, d);
    
    const GRADIENT_TYPE * vertex_gradient_coord0 = gradient_grid.VectorPtrConst(iv0);
    
    const GRADIENT_TYPE * vertex_gradient_coord2 = gradient_grid.VectorPtrConst(iv2);
    
    cntrl_diff_d = (vertex_gradient_coord2[index_coord] - vertex_gradient_coord0[index_coord])/2.0;
}




// Computes the backward diff in the d th dim
void compute_backward_difference_d
( const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1,
 const DIRECTION d,
 const int index_coord,
 GRADIENT_TYPE &bkwd_diff_d)
{
    const int dimension = scalar_grid.Dimension();
    VERTEX_INDEX iv0 = scalar_grid.PrevVertex(iv1, d);
    bkwd_diff_d = scalar_grid.Scalar(iv1) - scalar_grid.Scalar(iv0);
};


void compute_boundary_gradient
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1, GRADIENT_TYPE * gradient)
{
    const int dimension = scalar_grid.Dimension();
    GRID_COORD_TYPE coord[dimension];
    
    scalar_grid.ComputeCoord(iv1, coord);
    
    for (int d = 0; d < dimension; d++) {
        if (coord[d] > 0) {
            VERTEX_INDEX iv0 = scalar_grid.PrevVertex(iv1, d);
            if (coord[d]+1 < scalar_grid.AxisSize(d)) {
                VERTEX_INDEX iv2 = scalar_grid.NextVertex(iv1, d);
                // use central difference
                gradient[d] = (scalar_grid.Scalar(iv2) - scalar_grid.Scalar(iv0))/2;
            }
            else {
                gradient[d] = scalar_grid.Scalar(iv1) - scalar_grid.Scalar(iv0);
            }
        }
        else if (coord[d]+1 < scalar_grid.AxisSize(d)) {
            VERTEX_INDEX iv2 = scalar_grid.NextVertex(iv1, d);
            gradient[d] = scalar_grid.Scalar(iv2) - scalar_grid.Scalar(iv1);
        }
        else {
            gradient[d] = 0;
        }
    }
}


// Compute C for eacn dimension

void compute_C_d 
(GRADIENT_TYPE * GNd,
 GRADIENT_TYPE * Gd,
 const int dimension,
 GRADIENT_TYPE * Cd)
{
    //normalize the Gd
    normalize(Gd, dimension);
    // dot pdt
    for (int i=0; i<dimension; i++)
    {
        GRADIENT_TYPE  temp[3]={0.0};
        
        for (int j=0; j<dimension; j++) 
        {
            temp[i] = GNd[3*j+i];
        }
        vector_dot_pdt (temp, Gd, dimension, Cd[i]);
    }
};

// Compute M for each dimension for a particular index_coord
void compute_M_d_i
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1,
 GRADIENT_TYPE * Cd,
 const int d,
 const int index_coord,
 GRADIENT_TYPE  & Mz_i)
{
    GRADIENT_TYPE temp0, temp1;
    // this is for index coord
    // diff+indexcoord(N indexcoord) - diff+indexcoord(scalar_grid)* C indexcoord
    
    //calculate the first term 
    compute_forward_difference_d (scalar_grid, gradient_grid, iv1, d, index_coord, temp0);
    //calculate the second term 
    compute_forward_difference_d (scalar_grid, iv1, d, temp1);
    
    Mz_i = temp0 - temp1*Cd[d];
}
// Compute M for each dimension 

void compute_M_d 
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX iv1,
 GRADIENT_TYPE * Cd,
 const int dimension,
 const int d, 
 GRADIENT_TYPE * Mz)
{
    for (int i=0; i<dimension; i++) {
        compute_M_d_i(scalar_grid, gradient_grid, iv1, Cd, d, i, (Mz[i])); 
    }
};


// calculate K x 
void compute_K_d
(const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 GRADIENT_TYPE *Gd,
 GRADIENT_TYPE *GNd,
 GRADIENT_TYPE *Cd,
 const int num_elements_GNd,
 const int num_elements_Cd,
 const int num_elements_Gd,
 GRADIENT_TYPE & Kd)
{
    
    GRADIENT_TYPE mag0(0.0);
    GRADIENT_TYPE C_square(0.0);
    GRADIENT_TYPE mag1(0.0);
    
    vector_sum_of_squares(GNd, num_elements_GNd, mag0);
    vector_sum_of_squares(Gd, num_elements_Gd, mag1);
    vector_dot_pdt (Cd, Cd, num_elements_Cd, C_square);
    
    Kd = mag0 - C_square*mag1;
};

// Compute gx as e^(-x/2*mu^2)
void compute_g_x(const float mu, const float param, const int flag_aniso, float & result )
{
    // the flag_aniso when set to zero calculates isotropic diffusion 
    // when set to 1 calculates the anisotropic diffusion 
    result = exp ((param*param*float(flag_aniso))/(-2.0*mu*mu));
   
    cout <<"flag_aniso "<< flag_aniso<<endl;
    cout <<"param "<<param<<endl;
    cout <<"result " << result<<endl;
    cout <<" mu "<<mu<<endl;
    
};

// Compute w projection 
void compute_w_projection 
(const GRADIENT_TYPE *w, 
 const GRADIENT_TYPE *N, 
 GRADIENT_TYPE *w_projection)
{
    GRADIENT_TYPE dw_dot_N(0.0);
    vector_dot_pdt(w, N, 3, dw_dot_N);
    
    for (int i=0; i<3; i++) {
        w_projection[i]= w[i] - dw_dot_N * N[i];
    }
}
/*
// compute w
void compute_w
(
 const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const float mu,
 const VERTEX_INDEX iv1,
 const GRADIENT_TYPE *Mx,
 const GRADIENT_TYPE *My,
 const GRADIENT_TYPE *Mz,
 const GRADIENT_TYPE &Kx,
 const GRADIENT_TYPE &Ky,
 const GRADIENT_TYPE &Kz,
 const int dimension,
 GRADIENT_TYPE w[3]
 )
{
    
    VERTEX_INDEX iv[dimension];
    
    // Calculate the Mx and kx for prev vertices 
    for (int i=0; i <dimension; i++)
    {
        iv[i] = scalar_grid.PrevVertex(iv1, i);
    }
    GRADIENT_TYPE M[9]={0.0};
    GRADIENT_TYPE * pointer_M;
    GRADIENT_TYPE K[3]={0.0};
    GRADIENT_TYPE gK[3]={0.0};
    
    for (int i=0; i<dimension; i++) {
        compute_m_k_d (scalar_grid, iv[i], gradient_grid, i,  M+(3*i), K[i]);
    }
    
    for (int i=0; i<dimension; i++) {
        
        compute_g_x (mu, K[i],gK[i]);
    }
    
    GRADIENT_TYPE gKx(0.0), gKy(0.0), gKz(0.0);
    compute_g_x (mu, Kx, gKx);
    compute_g_x (mu, Ky, gKy);
    compute_g_x (mu, Kz, gKz);
    
    
    for(int i=0; i<dimension; i++)
    {
        //w[i] = gKx*Mx[i] - gK[0]*M[i]   + gKy*My[i] - gK[1]*M[3+i] + gKz*Mz[i] - gK[2]*M[6+i];
        w[i] = Mx[i] - M[i]   + My[i] - M[3+i] + Mz[i] - M[6+i];
    }
    
}
*/
// compute the updated gradients
// N is the gradient of the vertex iv1m

void compute_new_N
(GRADIENT_GRID_BASE & gradient_grid,
 GRADIENT_TYPE * N, 
 const VERTEX_INDEX iv1,
 const float lambda,
 GRADIENT_TYPE *w_projection)
{
    // Calculate the new gradients
    for (int i=0; i<DIM3; i++) {
        N[i] = N[i] + lambda * w_projection[i];
    }
    gradient_grid.Set(iv1, N);
};


// Calculate the anisotropic diff of the gradients for each vertex
/*
 void anisotropic_diff_per_vert
 (const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const float mu,
 const float lambda,
 const VERTEX_INDEX iv1,
 GRADIENT_GRID & gradient_grid)
 {
 // variables
 int Dx=0, Dy=1, Dz=2;
 GRADIENT_TYPE Gx[3]={0.0}; // [3x1]
 GRADIENT_TYPE Gy[3]={0.0}; // [3x1]
 GRADIENT_TYPE Gz[3]={0.0}; // [3x1]
 
 GRADIENT_TYPE GNx[9]={0.0}; // [3x3]
 GRADIENT_TYPE GNy[9]={0.0}; // [3x3]
 GRADIENT_TYPE GNz[9]={0.0}; // [3x3]
 
 GRADIENT_TYPE Cx[3]={0.0}; // [1x3]
 GRADIENT_TYPE Cy[3]={0.0}; // [1x3]
 GRADIENT_TYPE Cz[3]={0.0}; // [1x3]
 
 GRADIENT_TYPE Mx[3]={0.0}; // [1x3]
 GRADIENT_TYPE My[3]={0.0}; // [1x3]
 GRADIENT_TYPE Mz[3]={0.0}; // [1x3]
 
 GRADIENT_TYPE Kx(0.0);
 GRADIENT_TYPE Ky(0.0);
 GRADIENT_TYPE Kz(0.0);
 
 GRADIENT_TYPE w[3]={0.0};
 GRADIENT_TYPE w_projection[3]={0.0};
 
 
 
 const int dimension = scalar_grid.Dimension();
 
 // calculate Gx
 compute_grad_H_d (scalar_grid, iv1, Dx, Gx);
 
 // calculate Gy
 compute_grad_H_d (scalar_grid, iv1, Dy, Gy);
 
 // calculate Gz
 compute_grad_H_d (scalar_grid, iv1, Dz, Gz);
 
 // calculate GNx
 compute_grad_H_d ( scalar_grid, gradient_grid, iv1, Dx, GNx);
 
 
 // calculate GNy
 compute_grad_H_d ( scalar_grid, gradient_grid, iv1, Dy, GNy);
 
 // calculate GNz
 compute_grad_H_d ( scalar_grid, gradient_grid, iv1, Dz, GNz);
 
 
 // calculate Cx
 compute_C_d (GNx, Gx, dimension, Cx);
 
 // calculate Cy
 compute_C_d (GNy, Gy, dimension, Cy);
 
 // calculate Cz
 compute_C_d (GNz, Gz, dimension, Cz);
 
 // calculate Mx
 compute_M_d (scalar_grid, gradient_grid, iv1, Cx, dimension, Dx, Mx);
 // calculate My
 compute_M_d (scalar_grid, gradient_grid, iv1, Cy, dimension, Dy, My);
 // calculate Mz
 compute_M_d (scalar_grid, gradient_grid, iv1, Cz, dimension, Dz, Mz);
 
 // calculate K x
 compute_K_d (scalar_grid, Gx, GNx, Cx, 9, 3, 3, Kx);
 // calculate K y
 compute_K_d (scalar_grid, Gy, GNy, Cy, 9, 3, 3, Ky);
 // calculate K z
 compute_K_d (scalar_grid, Gz, GNz, Cz, 9, 3, 3, Kz);
 // Compute w
 compute_w (scalar_grid, gradient_grid, mu, iv1, Mx, My, Mz, Kx, Ky, Kz, dimension, w);
 
 GRADIENT_TYPE * N = gradient_grid.VectorPtr(iv1);
 // Compute w projection 
 compute_w_projection (w, N, w_projection);
 
 compute_new_N (gradient_grid, N,  iv1, lambda, w_projection); 
 
 }
 */


// new version of anisotropic gradient filtering per vertex
void anisotropic_diff_per_vert
(const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const float mu,
 const float lambda,
 const VERTEX_INDEX iv1,
 const int flag_aniso,
 GRADIENT_GRID & gradient_grid)
{
    
    GRADIENT_TYPE mX[DIM3]={0.0};
    GRADIENT_TYPE mX_prev_vert_X[DIM3]={0.0};
    
    GRADIENT_TYPE mY[DIM3]={0.0};
    GRADIENT_TYPE mY_prev_vert_Y[DIM3]={0.0};
    
    GRADIENT_TYPE mZ[DIM3]={0.0};
    GRADIENT_TYPE mZ_prev_vert_Z[DIM3]={0.0};
    
    // Compute M d for dierection 'd' for  vertex iv1
    compute_m_d
    ( scalar_grid, gradient_grid, iv1, 0, mX);
    compute_m_d
    ( scalar_grid, gradient_grid, iv1, 1, mY);
    compute_m_d
    ( scalar_grid, gradient_grid, iv1, 2, mZ);
    
    // compute prev vertex in 0,1,2 direction 
    VERTEX_INDEX prev_vert[DIM3];
    prev_vert[0] = scalar_grid.PrevVertex(iv1, 0);
    prev_vert[1] = scalar_grid.PrevVertex(iv1, 1);
    prev_vert[2] = scalar_grid.PrevVertex(iv1, 2);
    
    compute_m_d
    ( scalar_grid, gradient_grid, prev_vert[0], 0, mX_prev_vert_X);
    
    compute_m_d
    ( scalar_grid, gradient_grid, prev_vert[1], 1, mY_prev_vert_Y);
    
    compute_m_d
    ( scalar_grid, gradient_grid, prev_vert[2], 2, mZ_prev_vert_Z);
    
    // Compute 'k' for each dimensions , 
    // used for anisotropic gradients
    
    SCALAR_TYPE K[DIM3]={0.0};
    SCALAR_TYPE gK[DIM3]={0.0};
    
    SCALAR_TYPE Kprev[DIM3]={0.0};
    SCALAR_TYPE gKprev[DIM3]={0.0};
    
    for (int d=0; d<DIM3; d++) {
        
        // compute gradHN_d
        GRADIENT_TYPE   gradHN_d[DIM9]={0.0};
        compute_gradH_d_normals (gradient_grid, iv1, d,gradHN_d);
        // compute C_d
        GRADIENT_TYPE c[DIM3]={0.0};
        compute_c_d(scalar_grid, gradient_grid, iv1, d, c);
        
        SCALAR_TYPE sum_gradHNd, c_square;
        
        vector_sum_of_squares(gradHN_d, DIM9, sum_gradHNd);
        
        vector_dot_pdt(c, c, DIM3, c_square);
        K[d] = sum_gradHNd  - c_square;
        compute_g_x(mu, K[d], flag_aniso, gK[d]);
    }
    
    
    for (int d=0; d<DIM3; d++) {
        
        // compute gradHN_d
        GRADIENT_TYPE   gradHN_d[DIM9]={0.0};
        compute_gradH_d_normals (gradient_grid, prev_vert[d], d,gradHN_d);
        // compute C_d
        GRADIENT_TYPE c[DIM3]={0.0};
        compute_c_d(scalar_grid, gradient_grid, prev_vert[d], d, c);
        
        SCALAR_TYPE sum_gradHNd, c_square;
        
        vector_sum_of_squares(gradHN_d, DIM9, sum_gradHNd);
        
        vector_dot_pdt(c, c, DIM3, c_square);
        K[d] = sum_gradHNd  - c_square;
        compute_g_x(mu, K[d],flag_aniso, gKprev[d]);
    }
    
    GRADIENT_TYPE    w[DIM3]={0.0};
    for (int i =0; i<DIM3; i++) {
        w[i] =  mX[i] - mX_prev_vert_X[i] +
        mY[i] - mY_prev_vert_Y[i] +
        mZ[i] - mZ_prev_vert_Z[i] ;
    }
    /*
    for (int i =0; i<DIM3; i++) {
        w[i] =  gK[i]*mX[i] - gKprev[i]*mX_prev_vert_X[i] +
        gK[i]*mY[i] - gKprev[i]*mY_prev_vert_Y[i] +
        gK[i]*mZ[i] - gKprev[i]*mZ_prev_vert_Z[i] ;
    }
    */
    GRADIENT_TYPE  wN = 0.0;
    
    GRADIENT_TYPE *Normals = gradient_grid.VectorPtr(iv1);
    for (int i=0; i<DIM3; i++) {
        wN = wN + Normals[i]*w[i];
    }
    
    for (int i=0; i<DIM3; i++) {
        w[i] = w[i] - wN*Normals[i];
    }
    
    for (int i=0; i<DIM3; i++) {
        Normals[i] = Normals[i]  + lambda *w[i];
    }
    gradient_grid.Set(iv1, Normals);
}


// the stand alone version which computes m and k 
// for a particular vertex for a partiuclar direction 

void compute_m_k_d
(const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv1,
 const GRADIENT_GRID_BASE & gradient_grid,
 const int direction,
 GRADIENT_TYPE Mx[3],
 GRADIENT_TYPE &Kx
 )

{ // variables
    int Dx=direction;
    GRADIENT_TYPE Gx[3]={0.0}; // [3x1]
    GRADIENT_TYPE GNx[9]={0.0}; // [3x3]
    GRADIENT_TYPE Cx[3]={0.0}; // [1x3]
    const int dimension = scalar_grid.Dimension();
    // calculate Gx
    compute_grad_H_d (scalar_grid, iv1, Dx, Gx);
    // calculate GNx
    compute_grad_H_d (scalar_grid, gradient_grid, iv1, Dx, GNx);
    // calculate Cx
    compute_C_d (GNx, Gx, dimension, Cx);
    // calculate Mx
    compute_M_d (scalar_grid, gradient_grid, iv1, Cx, dimension, Dx, Mx);
    
    // calculate K 
    compute_K_d(scalar_grid, Gx, GNx, Cx, 9, 3, 3, Kx);
}

// Function to check if the vertex iv is inside 2 cube 
// distance from the boundary.

void is_inside_2_grid
(const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const VERTEX_INDEX iv, 
 bool &flag_is_inside_2_grid)
{
    COORD_TYPE coord[DIM3]={0.0};
    scalar_grid.ComputeCoord(iv, coord);
    for (int i=0; i<DIM3; i++) {
        int Size = scalar_grid.AxisSize(i);
        if (!(1 < coord[i] && coord[i] < (Size - 2))) {
            flag_is_inside_2_grid = false;
        }
    } 
}

// Calculate the anisotropic diff of the gradients.
void anisotropic_diff
(const ISODUAL3D::ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const float mu,
 const float lambda,
 const int num_iter,
 const int flag_aniso,
 GRADIENT_GRID & gradient_grid)
{
    //debug
    
    cout <<"flag_ aniso "<<flag_aniso<<endl;
    const int dimension = scalar_grid.Dimension();
    
    gradient_grid.SetSize(scalar_grid, dimension);
    
    BOOL_GRID boundary_grid;
    boundary_grid.SetSize(scalar_grid);
    compute_boundary_grid(boundary_grid);
    
    for (int k=0; k<num_iter; k++) 
    {
        for (VERTEX_INDEX iv = 0; iv < scalar_grid.NumVertices(); iv++)
        {
            bool flag_is_inside_2_grid = true;
            is_inside_2_grid(scalar_grid, iv, flag_is_inside_2_grid);
            if(flag_is_inside_2_grid)
            { 
                anisotropic_diff_per_vert(scalar_grid, mu, lambda, iv, flag_aniso, gradient_grid);
                
            }
        }
    }
};


// helper routines 

// Calculate vector magnitude.
void vector_magnitude (const float * vec, const int num_elements, float & mag)
{
    float sum = 0.0;
    mag = 0.0;
    for (int i=0; i<num_elements; i++) {
        sum = sum + vec[i]*vec[i];
    }
    mag = sqrt(sum);
}





// Normalize the vectors.
void normalize (float *vec, const int num_elements)
{
    float mag = 0.0;
    vector_magnitude (vec, num_elements, mag);
    
    for (int i=0; i<num_elements; i++) {
        if (abs(mag-0.0) < EPSILON) {
            vec[i] = 0.0;
        }
        else
            vec[i] = vec[i] / mag;
    }
}

