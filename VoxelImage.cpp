#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>

#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>

#include "VoxelImage.hpp"
#include "Voxel.hpp"
#include "Plane.hpp"
#include "Brick.hpp"

using namespace std;

VoxelImage::VoxelImage (unsigned int max_x, unsigned int max_y, unsigned int max_z, bool isColored):
	_max_x(max_x), 
	_max_y(max_y),
	_max_z(max_z),
	_xy_plane(NULL),
	_xz_plane(NULL),
	_yz_plane(NULL),
	_isInsideMatrix(max_x, max_y, max_z),
	_isColored(isColored)
{
	_xy_plane = new Plane(X, Y, max_x, max_y);
	_yz_plane = new Plane(Y, Z, max_y, max_z);
	_xz_plane = new Plane(X, Z, max_x, max_z);
}; 

isInsideType IsInsideMatrix::GetVoxel(unsigned int x, unsigned int y, unsigned int z)
{
	if (x<0 || y<0 || z<0 || x>_max_x-1 || y>_max_y-1 || z>_max_z-1)
		return OUTSIDE;
	return _pIsInsideMatrix[x][y][z];
}

void VoxelImage::AddVoxel (Voxel* v)
{
	if (v == NULL) // Invalid voxel
		return;
	Voxel* pVoxel = FindVoxelByCoordinates(v->_coordinates[X], v->_coordinates[Y], v->_coordinates[Z]);
	if (pVoxel)	{
		int c = Voxel::QuantizeColor(v->_color);
		pVoxel->_colorHistogram[c]++;
		delete v;
		return;
	}
	_xy_plane->InsertVoxel(v);
	_yz_plane->InsertVoxel(v);
	_xz_plane->InsertVoxel(v);
	int c = Voxel::QuantizeColor(v->_color);
	if (c!=-1)
		v->_colorHistogram[c]++;
	return;
}

int VoxelImage::RemoveVoxel (Voxel* v)
{
	if (v == NULL) // Invalid voxel
		return -1;
	_xy_plane->RemoveVoxel(v);
	_yz_plane->RemoveVoxel(v);
	_xz_plane->RemoveVoxel(v);
	delete v;
	return 0;
}

void VoxelImage::BuildIsInsideMatrix()
{
	cout << "Building Inside Matrix..." << endl;
	// initiallize matrix and set edges
	_isInsideMatrix._pIsInsideMatrix.resize(_max_x);
	for (unsigned int x = 0; x<_max_x; x++) {
		_isInsideMatrix._pIsInsideMatrix[x].resize(_max_y);
		for (unsigned int y = 0; y<_max_y; y++) {
			_isInsideMatrix._pIsInsideMatrix[x][y].resize(_max_z);
			for (unsigned int z = 0; z<_max_z; z++)
				if (IsEdge(x,y,z)) {
					if (IsVoxel(x,y,z)) 
						_isInsideMatrix.SetVoxel(x, y, z, MODEL);
					else
						_isInsideMatrix.SetVoxel(x, y, z, OUTSIDE);
				}
				else
					_isInsideMatrix.SetVoxel(x, y, z, UNKNOWN);
		}
	}

	bool finished = false;
	int temp = 0;
	while (!finished) {
		finished = true;
		temp++;
		cout << "Iteration " << temp << " done" << endl;
		for (unsigned int x = 1; x<_max_x; x++) 
			for (unsigned int y = 1; y<_max_y; y++) 
				for (unsigned int z = 1; z<_max_z; z++) {
					if (_isInsideMatrix.GetVoxel(x, y, z) != UNKNOWN)
						continue;
					if (IsVoxel(x,y,z)) {
						_isInsideMatrix.SetVoxel(x, y, z, MODEL);
						finished = false;
					}
					else {
						if (IsNextToOutsideVoxel(x,y,z)) {
							_isInsideMatrix.SetVoxel(x, y, z, OUTSIDE);	
							finished = false;
						}
					}
				}
	}

	// fill the unvisited voxels with INSIDE label
	for (unsigned int x = 0; x<_max_x; x++) 
		for (unsigned int y = 0; y<_max_y; y++) 
			for (unsigned int z = 0; z<_max_z; z++) { 
				if (_isInsideMatrix.GetVoxel(x, y, z) == UNKNOWN)
					_isInsideMatrix.SetVoxel(x, y, z, INSIDE);
			}
	//cout << "BuildIsInsideMatrix ended" << endl;
}

bool VoxelImage::IsEdge (unsigned int x, unsigned int y, unsigned int z) const
{
	return (x==0 || y==0 || z==0 || x == _max_x || y == _max_y || z == _max_z);
}

bool VoxelImage::IsNextToOutsideVoxel (unsigned int x, unsigned int y, unsigned int z)
{
	return (_isInsideMatrix.GetVoxel(x+1, y, z) == OUTSIDE || _isInsideMatrix.GetVoxel(x, y+1, z)== OUTSIDE || 
		    _isInsideMatrix.GetVoxel(x, y, z+1) == OUTSIDE || _isInsideMatrix.GetVoxel(x-1, y, z)== OUTSIDE ||
		    _isInsideMatrix.GetVoxel(x, y-1, z) == OUTSIDE || _isInsideMatrix.GetVoxel(x, y, z-1)== OUTSIDE);
}

bool VoxelImage::IsVoxel (unsigned int x, unsigned int y, unsigned int z) const
{
	Voxel* pVoxel = _xy_plane->GetVoxelByCoordinate(x, y);
	while (pVoxel != NULL && pVoxel->_coordinates[Z] != z)
		pVoxel = pVoxel->_neighbors[NEXT][Z];
	if (pVoxel == NULL || pVoxel->_coordinates[Z] != z)
		return false;
	return true;
}

Voxel* VoxelImage::FindVoxelByCoordinates(unsigned int x, unsigned int y, unsigned int z) const
{
	Voxel* pVoxel = _xy_plane->GetVoxelByCoordinate(x, y);
	while (pVoxel != NULL && pVoxel->_coordinates[Z] != z)
		pVoxel = pVoxel->_neighbors[NEXT][Z];
	if (pVoxel == NULL || pVoxel->_coordinates[Z] != z)
		return NULL;
	return pVoxel;
}

void VoxelImage::Thicken(unsigned int user_thickness)
{
	unsigned int thickness;
	if (user_thickness == 1)
		return;
	if (user_thickness == 0)
		thickness = int(ceil(double(max(max(_max_x,_max_y),_max_z))/100));
	else
		thickness = user_thickness;
	cout << "Thickening..." << endl;
	cout << "Model thickness: " << thickness << endl;

	BuildIsInsideMatrix();

	cout << "Round 1:" << endl;
	ThickenPlane(_xy_plane, thickness);
	cout << "XY plane done" << endl;
	ThickenPlane(_xz_plane, thickness);
	cout << "XZ plane done" << endl;
	ThickenPlane(_yz_plane, thickness);
	cout << "YZ plane done" << endl;
	BuildIsInsideMatrix();
	cout << "Round 2:" << endl;
	ThickenPlane(_yz_plane, thickness);
	cout << "YZ plane done" << endl;
	ThickenPlane(_xz_plane, thickness);
	cout << "XZ plane done" << endl;
	ThickenPlane(_xy_plane, thickness);
	cout << "XY plane done" << endl;
}

void VoxelImage::ThickenPlane(Plane * plane, const unsigned int thickness)
{
	coordinate direction = plane->_perp_coor;
	for (unsigned int i = 0; i<plane->_max_val[0]; i++) {
		for (unsigned int j = 0; j<plane->_max_val[1]; j++) {
			Voxel* currVoxel = plane->_planeMatrix[i][j];
			while (currVoxel) {
				bool shouldBeThickened = ShouldBeThickened(currVoxel, thickness, direction);
				if (shouldBeThickened) {
					thickeningDirection thickDirection = FindThickeningDirection(currVoxel, direction);
					if (thickDirection != NO_DIRECTION)
						ThickenSingleVoxel(currVoxel, direction, thickDirection, thickness);
				}
				currVoxel = NextDistantVoxel(currVoxel, direction);
			}
		}
	}
}

bool VoxelImage::ShouldBeThickened (Voxel* voxel, const unsigned int reqThickness, coordinate direction) const
{
	unsigned int thickness = 1;
	Voxel * currVoxel = voxel;
	Voxel * nextVoxel = voxel->_neighbors[NEXT][direction];
	while ((nextVoxel) && (currVoxel->_coordinates[direction] + 1 == nextVoxel->_coordinates[direction])) {
		thickness++;
		currVoxel = nextVoxel;
		nextVoxel = currVoxel->_neighbors[NEXT][direction];
	}
	return thickness < reqThickness;
}

thickeningDirection VoxelImage::FindThickeningDirection(Voxel* voxel, coordinate c)
{
	unsigned int prev_x, prev_y, prev_z;
	unsigned int next_x, next_y, next_z;

	if (!voxel)
		return NO_DIRECTION;
	
	GetNextLocationByDirection(voxel, c, POS_DIRECTION, next_x, next_y, next_z);
	GetNextLocationByDirection(voxel, c, NEG_DIRECTION, prev_x, prev_y, prev_z);
	Voxel nextEmptyVoxel(next_x, next_y, next_z);
	Voxel prevEmptyVoxel(prev_x, prev_y, prev_z);

	while (IsVoxel(next_x, next_y, next_z)) {
		GetNextLocationByDirection(&nextEmptyVoxel, c, POS_DIRECTION, next_x, next_y, next_z);
		nextEmptyVoxel._coordinates[X]=next_x;
		nextEmptyVoxel._coordinates[Y]=next_y;
		nextEmptyVoxel._coordinates[Z]=next_z;
	}
	while (IsVoxel(prev_x, prev_y, prev_z)) {
		GetNextLocationByDirection(&prevEmptyVoxel, c, NEG_DIRECTION, prev_x, prev_y, prev_z);
		prevEmptyVoxel._coordinates[X]=next_x;
		prevEmptyVoxel._coordinates[Y]=next_y;
		prevEmptyVoxel._coordinates[Z]=next_z;
	}

	bool isPrevVoxelInside = _isInsideMatrix.GetVoxel(prev_x, prev_y, prev_z) == INSIDE;
	bool isNextVoxelInside = _isInsideMatrix.GetVoxel(next_x, next_y, next_z) == INSIDE;

	if (isPrevVoxelInside && !isNextVoxelInside)
		return NEG_DIRECTION;
	if (isNextVoxelInside && !isPrevVoxelInside)
		return POS_DIRECTION;
	if (isNextVoxelInside && isPrevVoxelInside) {
		Voxel * nextVoxel = NextDistantVoxel(voxel, c);
		Voxel * prevVoxel = PrevDistantVoxel(voxel, c);
		int distance2NextVoxel = abs(int(voxel->_coordinates[c] - nextVoxel->_coordinates[c]));
		int distance2PrevVoxel = abs(int(voxel->_coordinates[c] - prevVoxel->_coordinates[c]));
		if (distance2NextVoxel < distance2PrevVoxel)
			return POS_DIRECTION;
		else
			return NEG_DIRECTION;
	}
	return NO_DIRECTION;
}


void VoxelImage::ThickenSingleVoxel(Voxel* voxel, const coordinate axis, 
									thickeningDirection thickDirection, const unsigned int thickness)
{
	Voxel * currVoxel = voxel;
	bool reachedEdge = false;
	bool outOfObject = false;
	for (unsigned int i = 1; i<thickness; i++) {
		if (!currVoxel)
			return;
		unsigned int next_x, next_y, next_z;
		GetNextLocationByDirection(currVoxel, axis, thickDirection, next_x, next_y, next_z);

		if (IsVoxel(next_x, next_y, next_z))
			reachedEdge = true;
		else {
			if (reachedEdge) {
				if (_isInsideMatrix.GetVoxel(next_x, next_y, next_z)==OUTSIDE)
					// if the current voxes is edge and the next is outside
					outOfObject = true;
				else
					reachedEdge = false;
			}
			if (!outOfObject) {
				Voxel * newVoxel = new Voxel(next_x, next_y, next_z);
				AddVoxel(newVoxel);
				_isInsideMatrix.SetVoxel(next_x, next_y, next_z, MODEL);
			}
		}

		if (outOfObject)
			break;
		if (thickDirection==POS_DIRECTION)
			currVoxel = currVoxel->_neighbors[NEXT][axis];
		else
			currVoxel = currVoxel->_neighbors[PREVIOUS][axis];		
	}
}

Voxel* VoxelImage::NextDistantVoxel(Voxel* voxel, const coordinate axis)
{
	Voxel * currVoxel = voxel;
	unsigned int next_x, next_y, next_z;
	GetNextLocationByDirection(voxel, axis, POS_DIRECTION, next_x, next_y, next_z);

	while (IsVoxel(next_x, next_y, next_z)) {
		currVoxel = currVoxel->_neighbors[NEXT][axis];
		GetNextLocationByDirection(currVoxel, axis, POS_DIRECTION, next_x, next_y, next_z);
	}
	return 	currVoxel->_neighbors[NEXT][axis];
}

Voxel* VoxelImage::PrevDistantVoxel(Voxel* voxel, const coordinate axis)
{
	Voxel * currVoxel = voxel;
	unsigned int prev_x, prev_y, prev_z;
	GetNextLocationByDirection(voxel, axis, NEG_DIRECTION, prev_x, prev_y, prev_z);

	while (IsVoxel(prev_x, prev_y, prev_z)) {
		currVoxel = currVoxel->_neighbors[PREVIOUS][axis];
		GetNextLocationByDirection(currVoxel, axis, NEG_DIRECTION, prev_x, prev_y, prev_z);
	}
	return 	currVoxel->_neighbors[PREVIOUS][axis];
}

void VoxelImage::GetNextLocationByDirection (Voxel* voxel, const coordinate axis, thickeningDirection direction, 
										     unsigned int & x, unsigned int & y, unsigned int & z) const
{
	x = (axis==X) ? voxel->_coordinates[X]+1*direction : voxel->_coordinates[X];
	y = (axis==Y) ? voxel->_coordinates[Y]+1*direction : voxel->_coordinates[Y];
	z = (axis==Z) ? voxel->_coordinates[Z]+1*direction : voxel->_coordinates[Z];
}

osg::ref_ptr<osg::Group> VoxelImage::DrawSlice(coordinate coor1, unsigned int value)
{
	Plane* sPlane = NULL;
	coordinate traversal = X;
	bool firstCoorConstant = false;
	switch (coor1) {
		case X :
			sPlane = _xz_plane;
			traversal = Y;
			firstCoorConstant = true;
			break;
		case Y :
			sPlane = _yz_plane;
			traversal = X;
			firstCoorConstant = true;
			break;
		case Z :
			sPlane = _xz_plane;
			traversal = Y;
			firstCoorConstant = false;
			break;
		default: break;
	}
	osg::ref_ptr<osg::Group> root = Voxel::PrepareScene();
	for (unsigned int i=0; i<_max_x ; i++) {
		Voxel* pVoxel = NULL;
		if (firstCoorConstant)
			pVoxel = sPlane->GetVoxelByCoordinate(value,i);
		else
			pVoxel = sPlane->GetVoxelByCoordinate(i,value);
		while (pVoxel) {
			Voxel::DrawSingleVoxel(root, pVoxel);
			pVoxel = pVoxel->_neighbors[NEXT][traversal];
		}
	}
	return root;
}

Plane* VoxelImage::GetSlice (unsigned int z)
{
	if (z < 0 || z > _max_z)
		return NULL;
	Plane* slicePlane = new Plane(X, Y, _max_x, _max_y);
	for (unsigned int i=0; i<_max_y ; i++) {
		Voxel* pVoxel = _yz_plane->GetVoxelByCoordinate(i,z);
		while (pVoxel) {
			slicePlane->_planeMatrix[pVoxel->_coordinates[X]][pVoxel->_coordinates[Y]] = pVoxel;
			pVoxel = pVoxel->_neighbors[NEXT][X];
		}
	}
	return slicePlane;
}

void VoxelImage::UpdateVoxelColors()
{
	for (unsigned int i=0; i<_max_x ; i++) { 
		for (unsigned int j=0; j<_max_y ; j++) {
			Voxel* pVoxel = _xy_plane->GetVoxelByCoordinate(i,j);
			while (pVoxel) {
				pVoxel->_colorBrickNumber = pVoxel->GetMajorityColor();
				pVoxel = pVoxel->_neighbors[NEXT][Z];
			}
		}
	}
}

osg::ref_ptr<osg::Group> VoxelImage::DrawScene()
{
	osg::ref_ptr<osg::Group> root = Voxel::PrepareScene();

	for (unsigned int i=0; i<_max_x ; i++) { 
		for (unsigned int j=0; j<_max_y ; j++) {
			Voxel* pVoxel = _xy_plane->GetVoxelByCoordinate(i,j);
			while (pVoxel) {
				Voxel::DrawSingleVoxel(root, pVoxel);
				pVoxel = pVoxel->_neighbors[NEXT][Z];
			}
		}
	}
	return root;
}