// Plane.hpp
// Planes are the main components of VoxelImage. 
// They hold the model voxels which are later connected to create the Lego brics.

#include <vector>
#include "Voxel.hpp"

#ifndef _Plane_hpp
#define _Plane_hpp

using std::vector;

// class Plane is built in the following way:
// the Plane is parallel to [_par_coor1, _par_coor2] plane, while _perp_coor is the perpendicular coordinate
// _planeMatrix is a 2D Voxel matrix which represent the voxels matching _perp_coor=0
// each voxel points to it's neighbors, so using these relations each voxel on the scene can be reached from each Plane
class Plane {
public:
	Plane(coordinate par_coor1, coordinate par_coor2, unsigned int max_val1, unsigned int max_val2);

	void InsertVoxel(Voxel* v);
	void RemoveVoxel(Voxel* v);
	Voxel* GetVoxelByCoordinate (unsigned int coor1, unsigned int coor2);

	// members
	unsigned int _max_val[2];
	vector<vector<Voxel*>> _planeMatrix;
	coordinate _par_coor1, _par_coor2, _perp_coor;
};

#endif
