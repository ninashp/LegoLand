// Voxel.hpp
// LagoLand Model is built of single voxels merged into Lego bricks
// Voxel is the basic component of a Plane, from which the VoxelImage is made of

#include <vector>

#include <osg/Node>
#include <osg/Group>

#ifndef _Voxel_hpp
#define _Voxel_hpp

using std::vector;

enum coordinate {
	X = 0,
	Y = 1,
	Z = 2
};

enum direction {
	PREVIOUS = 0,
	NEXT = 1
};

struct color {
	// don't care color value is -1
	int _red;
	int _green;
	int _blue;
};

class Voxel {
  public:
	Voxel (unsigned int x, unsigned int y, unsigned int z,	int red, int green, int blue, int alpha): 
		_colorBrickNumber(-1) 
		{SetInitParams(x, y, z, red, green, blue, alpha);} 

	Voxel (unsigned int x, unsigned int y, unsigned int z):	_colorBrickNumber(-1) 	
		{SetInitParams(x, y, z, -1, -1, -1, 255);}

	// color functions
	static int QuantizeColor(color c);	// quantization into 16 colors according to Lego bricks available colors
	int GetMajorityColor();

	// visualization using OSG
	static osg::ref_ptr<osg::Group> PrepareScene();
	static osg::ref_ptr<osg::Group> Voxel::DrawSingleVoxel(osg::ref_ptr<osg::Group> root, Voxel* voxel);

	// members
	color _color;
	int _colorBrickNumber;
    unsigned int _coordinates[3];
	Voxel* _neighbors[2][3];
	unsigned int _colorHistogram[16];

private:
	void SetInitParams(unsigned int x, unsigned int y, unsigned int z, int red, int green, int blue, int alpha);
};

typedef vector <Voxel*> VoxelList;
typedef vector<osg::ref_ptr<osg::PositionAttitudeTransform>> XformVecType;

#endif
