// VoxelImage.hpp
// This is the main mechanizm of LegoLand that stores the voxels

#include <vector>

#include "Voxel.hpp"
#include "Plane.hpp"


#include <osg/Node>
#include <osg/Group>

#ifndef _VoxelImage_hpp
#define _VoxelImage_hpp

using std::vector;

enum thickeningDirection {
	POS_DIRECTION = 1,
	NEG_DIRECTION = -1,
	NO_DIRECTION = 0
};

enum isInsideType: char {
	UNKNOWN = 0,
	INSIDE = 1,
	OUTSIDE = 2,
	MODEL = 3
};

// 3D matrix of all space voxels containing the information 
// whether each space voxel is INSIDE or OUTSIDE the model
class IsInsideMatrix {
public:
	IsInsideMatrix(unsigned int max_x, unsigned int max_y, unsigned int max_z):
	  _max_x(max_x), _max_y(max_y), _max_z(max_z) {};
	void SetVoxel(unsigned int x, unsigned int y, unsigned int z, isInsideType isInside)
		{_pIsInsideMatrix[x][y][z]=isInside;};
	isInsideType GetVoxel(unsigned int x, unsigned int y, unsigned int z);

	// members
	vector< vector < vector<isInsideType> > > _pIsInsideMatrix;
private:
	unsigned int _max_x; // Maximum x value (minimum value is 0)
	unsigned int _max_y; // Maximum y value (minimum value is 0)
	unsigned int _max_z; // Maximum z value (minimum value is 0)
};

// class that holds the voxels of the model (before division into bricks)
// the voxels are stored in 3 Plane data structures, according to the 3 planes in space
class VoxelImage {
public:
	VoxelImage (unsigned int max_x, unsigned int max_y, unsigned int max_z, bool isColored);

    // Add a voxel to the database
	// if a voxel already exists, averages the colors of the existing voxel and the new one
	void AddVoxel (Voxel* v); 
	int RemoveVoxel (Voxel* v); // Remove a voxel from the database

	// thicken VoxelImage in order to get a stable model
	void Thicken(unsigned int user_thickness);

	// update colors according to majority of points in each voxel
	void UpdateVoxelColors();

	unsigned int GetMaxZ() const { return _max_z;};
	Plane* GetSlice (unsigned int z);  // Get a slice (constant z coordinate)

	// visualization in OSG
	osg::ref_ptr<osg::Group> DrawScene(); // draw all the scene
	osg::ref_ptr<osg::Group> DrawSlice(coordinate coor1, unsigned int value); // draw slice

	// members
	Plane* _xy_plane;
	Plane* _xz_plane;
	Plane* _yz_plane;

	IsInsideMatrix _isInsideMatrix;
	bool _isColored;

private:
	Voxel* FindVoxelByCoordinates (unsigned int x, unsigned int y, unsigned int z) const;

	// does VoxelImage contain a voxel in (x,y,z) coordinates?
	bool IsVoxel (unsigned int x, unsigned int y, unsigned int z) const;
	// is voxel located on the edge of the scene
	bool IsEdge(unsigned int x, unsigned int y, unsigned int z) const;
	// is one of the neighbors is OUTSIDE the model 
	bool IsNextToOutsideVoxel(unsigned int x, unsigned int y, unsigned int z);

	Voxel* NextDistantVoxel(Voxel* voxel, const coordinate axis); // get model voxel which is next to voxel in given axis
	Voxel* PrevDistantVoxel(Voxel* voxel, const coordinate axis); // get model voxel which is previous to voxel in given axis
	// get location for neighbor voxel in the given coordinate direction
	void GetNextLocationByDirection (Voxel* voxel, const coordinate axis, thickeningDirection direction, 
								     unsigned int & x, unsigned int & y, unsigned int & z) const;

	// thickening functions
	void BuildIsInsideMatrix(); // used for thickening model without damaging the appearance from outside
	void ThickenPlane(Plane * plane, const unsigned int thickness); // thicken single plane
	void ThickenSingleVoxel(Voxel* voxel, const coordinate axis, 
						    thickeningDirection thickDirection, const unsigned int thickness);	
	// checking if the voxel should be thickened
	// assumes the voxel doesn't have a close neighbour in the negative direction
	bool ShouldBeThickened (Voxel* voxel, const unsigned int reqThickness, coordinate direction) const;
	thickeningDirection FindThickeningDirection(Voxel* voxel, coordinate c);

	// members
	unsigned int _max_x; // Maximum x value (minimum value is 0)
	unsigned int _max_y; // Maximum y value (minimum value is 0)
	unsigned int _max_z; // Maximum z value (minimum value is 0)
};

#endif