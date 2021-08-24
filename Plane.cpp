#include <iostream>
#include <fstream>
#include <string>

#include "Plane.hpp"
#include "Voxel.hpp"

using namespace std;

Plane::Plane(coordinate par_coor1, coordinate par_coor2, unsigned int max_val1, unsigned int max_val2):
_par_coor1(par_coor1), _par_coor2(par_coor2)
{
	if ((par_coor1 == X && par_coor2 == Y) || (par_coor1 == Y && par_coor2 == X))
		_perp_coor = Z;
	else if ((par_coor1 == X && par_coor2 == Z) || (par_coor1 == Z && par_coor2 == X))
		_perp_coor = Y;
	else _perp_coor = X;
	_max_val[0] = max_val1;
	_max_val[1] = max_val2;
	_planeMatrix.resize(max_val1);
	for (unsigned int i = 0; i < max_val1; i++) {
		_planeMatrix[i].resize(max_val2);
		for (unsigned int j = 0; j < max_val2; j++) {
			_planeMatrix[i][j] = NULL;
		}
	}
}

void Plane::InsertVoxel(Voxel* v)
{
	if (v == NULL) // Invalid voxel
		return;
	unsigned int coor1 = v->_coordinates[_par_coor1];
	unsigned int coor2 = v->_coordinates[_par_coor2];
	unsigned int coor3 = v->_coordinates[_perp_coor];
	Voxel* pVoxel = _planeMatrix[coor1][coor2];

	// Add pixel to plane
	if (pVoxel == NULL || coor3 < pVoxel->_coordinates[_perp_coor]) { // Coordinate is empty or pixel is first
		_planeMatrix[coor1][coor2] = v;
		v->_neighbors[PREVIOUS][_perp_coor] = NULL;
		v->_neighbors[NEXT][_perp_coor] = pVoxel;
		if (pVoxel != NULL)
			pVoxel->_neighbors[PREVIOUS][_perp_coor] = v;
	}
	else {
		while ((pVoxel->_neighbors[NEXT][_perp_coor] != NULL) && (coor3 > pVoxel->_coordinates[_perp_coor])) { // Find proper voxel place in linked list
			pVoxel = pVoxel->_neighbors[NEXT][_perp_coor];
		}
		if ((pVoxel->_neighbors[NEXT][_perp_coor] == NULL) && (coor3 > pVoxel->_coordinates[_perp_coor])) { // Voxel is last
			v->_neighbors[NEXT][_perp_coor] = NULL;
			pVoxel->_neighbors[NEXT][_perp_coor] = v;
			v->_neighbors[PREVIOUS][_perp_coor] = pVoxel;
		}
		else {
			if (pVoxel->_neighbors[PREVIOUS][_perp_coor] != NULL)
				pVoxel->_neighbors[PREVIOUS][_perp_coor]->_neighbors[NEXT][_perp_coor] = v;
			v->_neighbors[PREVIOUS][_perp_coor] = pVoxel->_neighbors[PREVIOUS][_perp_coor];
			v->_neighbors[NEXT][_perp_coor] = pVoxel;
			pVoxel->_neighbors[PREVIOUS][_perp_coor] = v;
		}
	}
}

void Plane::RemoveVoxel(Voxel* v)
{
	if (v == NULL) // Invalid voxel
		return;
	unsigned int coor1 = v->_coordinates[_par_coor1];
	unsigned int coor2 = v->_coordinates[_par_coor2];
	unsigned int coor3 = v->_coordinates[_perp_coor];
	Voxel* pVoxel = _planeMatrix[coor1][coor2];

	if (pVoxel == v) {
		_planeMatrix[coor1][coor2] = v->_neighbors[NEXT][_perp_coor];
	}

	if (v->_neighbors[PREVIOUS][_perp_coor] != NULL)
		v->_neighbors[PREVIOUS][_perp_coor]->_neighbors[NEXT][_perp_coor] = v->_neighbors[NEXT][_perp_coor];
	if (v->_neighbors[NEXT][_perp_coor] != NULL)
		v->_neighbors[NEXT][_perp_coor]->_neighbors[PREVIOUS][_perp_coor] = v->_neighbors[PREVIOUS][_perp_coor];
	return;
}

Voxel* Plane::GetVoxelByCoordinate (unsigned int coor1, unsigned int coor2)
{
	if (coor1 < 0 || coor1 >= _max_val[0] || coor2 < 0 || coor2 >= _max_val[1])
		return NULL;
	return _planeMatrix[coor1][coor2];
}

