//Functions to compute the reliable gradients

#ifndef _TEST_TYPES_
#define _TEST_TYPES_

#include "religrad_datastruct.h"
#include "ijkscalar_grid.txx"
#include "religrad_inputIO.h"
#include <iostream>

using namespace RELIGRADIENT;
using SHARPISO::GRADIENT_GRID;

// compute gradient using central difference 
void compute_gradient_central_difference(
		const RELIGRADIENT_SCALAR_GRID_BASE & scalar_grid,
		GRADIENT_GRID & gradient_grid, const INPUT_INFO & io_info);
// compute central difference returns the normalized gradient grid
// and the magnitude
void compute_gradient_central_difference_normalized(
		const RELIGRADIENT_SCALAR_GRID_BASE & scalar_grid,
		GRADIENT_GRID & normalized_grad_grid,
		GRADIENT_MAGNITUDE_GRID & grad_magnitude_grid,
		const INPUT_INFO & io_info);

// compute angle based reliable gradients
void compute_reliable_gradients_angle(
		const RELIGRADIENT_SCALAR_GRID_BASE & scalar_grid,
		GRADIENT_GRID & gradient_grid,
		GRADIENT_MAGNITUDE_GRID & grad_mag_grid,
		IJK::BOOL_GRID<RELIGRADIENT_GRID> &reliable_grid,
		INPUT_INFO & io_info);

// compute scalar based reliable gradients
void compute_reliable_gradients_SBP(
		const RELIGRADIENT_SCALAR_GRID_BASE & scalar_grid,
		const GRADIENT_GRID & gradient_grid,
		const GRADIENT_MAGNITUDE_GRID & grad_mag_grid,
		IJK::BOOL_GRID<RELIGRADIENT_GRID> &reliable_grid,
		INPUT_INFO & io_info);

// Advanced angle based reliable gradients computations
void compute_reliable_gradients_advangle(
	const RELIGRADIENT_SCALAR_GRID_BASE & scalar_grid,
	const GRADIENT_GRID & gradient_grid,
	const GRADIENT_MAGNITUDE_GRID & grad_mag_grid,
	IJK::BOOL_GRID<RELIGRADIENT_GRID> &reliable_grid,
	INPUT_INFO & io_info);

void compute_reliable_gradients_advangle_version2(
	const RELIGRADIENT_SCALAR_GRID_BASE & scalar_grid,
	const GRADIENT_GRID & gradient_grid,
	const  GRADIENT_MAGNITUDE_GRID & grad_mag_grid,
	IJK::BOOL_GRID<RELIGRADIENT_GRID> & reliable_grid,
	INPUT_INFO & io_info);

#endif
