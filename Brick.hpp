// Brick.hpp
// This class handles everything that involves dividing a voxelized model
// into bricks

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <osg/Node>
#include <osg/Group>
#include "Voxel.hpp"

#ifndef _Brick_hpp
#define _Brick_hpp

using namespace std;

class Brick {
public:
	Brick (); // Default constructor
	Brick (unsigned int x, unsigned int y, unsigned int z, unsigned int size1, unsigned int size2, string filename, unsigned int colorNumber=0); // constructor

	unsigned int _coordinates[3];	// Brick coordinates
	unsigned int _colorNumber;		// Number of the color in _availableColors
	unsigned int _size[2];			// Brick size (width, length)
	unsigned int _count;			// Number of bricks of a certain
	string _filename;

	static vector<Brick*> _availableBricks;	// The available brick sizes
	static vector<color> _availableColors;	// The available brick colors

	static void InitiateAvailableBricks();	// Initiate the available bricks list with the basic 11 LEGO bricks
	static void InitiateAvailableColors();	// Initiate the available colors list with the basic 16 LEGO colors
	static void ClearAvailableBricks();		// Clear the available bricks list

	// Model division into bricks
	static vector<Brick*>* BrickifyVoxelImage(VoxelImage* vimage);									// Convert the entire voxel image to bricks
	static void ExportBrickModel (vector<Brick*> BrickList, string out_filename, unsigned int bricksPerStep);	
																									// Export the brick model to a .DAT file
	
	// Visualization functions
	static osg::ref_ptr<osg::Group> DrawSingleBrick(osg::ref_ptr<osg::Group> root, Brick* brick);	// Draw a single brick using OSG
	static osg::ref_ptr<osg::Group> DrawBricks(vector<Brick*> *list);								// Draw all the bricks using OSG
	static osg::ref_ptr<osg::Group> DrawBrickSlice(vector<Brick*>* list, unsigned int z);			// Draw the bricks in a slice z=const. of the model

private:

	static bool InBounds(Plane* slice, unsigned int a, unsigned int b, bool xIsPrimary);			// Check whether a coordinate is in the model bounds

	static unsigned int BrickFit (Plane* slice, coordinate secondaryCoordinate, unsigned int coorValue, 
		unsigned int z, unsigned int mainBrickSize, unsigned int SecondaryBrickSize, vector<Brick*> *list, string filename);
																									// Attempt to fit a brick at a specific plane in the voxel model
	static void BrickifyPlane(VoxelImage* image, unsigned int z, vector<Brick*> *list);				// Convert a slice of the voxel model z=const. to bricks

	
	// Brick list sorting functions
	static bool CompareBricks (Brick *a, Brick *b);													// Brick comparison function
	static vector<Brick*> *SortBrickList(vector<Brick*> *bricklist);

};

#endif
