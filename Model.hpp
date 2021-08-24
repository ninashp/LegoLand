// Model.hpp
// This is the 3D model representation in LegoLand

#include <vector>

#include "VoxelImage.hpp"

#ifndef _Model_hpp
#define _Model_hpp

// Lego Bricks dimentions
#define VOXEL_DIM_X		8
#define VOXEL_DIM_Y		8
#define VOXEL_DIM_Z		9.6

// for visualization using OSG
#define STUD_DIAMETER	5
#define STUD_HEIGHT		1.7

// Point represents a single point in 3D model
struct Point {
    double _x;
    double _y;
    double _z;
	int _red;
	int _green;
	int _blue;
	int _alpha;
};

// Triangle represents a single triangle in 3D model
struct Triangle {
    Point* _point1;
    Point* _point2;
    Point* _point3;
};

typedef vector <Point*> PointVector;
typedef vector <Triangle*> TriangleVector;

// Model is represented with vector of Points and vector of Triangles
class Model {
  public:
	  // loads PLY file in ASCII
	Model(char* ascii_filepath, double model_hight);

	// Pre processing of triangles - splitting each triangle which is too big into small ones
	void Supersample();

	// Normalizing the point and triangles to be in the positive range
	void Normalize();

	// Voxelization - findind corresponding voxel for each point
	// if the model is colored, finding the color that fits the voxel
	VoxelImage* Voxelization();

	// members
    TriangleVector _triangles;
    PointVector _points;
	double _model_hight;
	double _max_x;
	double _max_y;
	double _max_z;

	bool _isColored;
};

#endif
