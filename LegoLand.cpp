// LegoLand.cpp
// Main file that rans the application

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <tchar.h>
#include <ctime>

#include <osg/Node>
#include <osg/Group>
#include <osgViewer/Viewer>

#include "Model.hpp"
#include "Voxel.hpp"
#include "Plane.hpp"
#include "VoxelImage.hpp"
#include "Brick.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	clock_t final, init = clock();
	double model_height = 500;
	unsigned int thickness = 0;
	unsigned int bricksPerStep = 0;
	Brick::InitiateAvailableBricks();
	Brick::InitiateAvailableColors();

	cout << "======================================" << endl;
	cout << "=            LegoLand 1.0            =" << endl;
	cout << "= By:                                =" << endl;
	cout << "=   Nina Shpalensky                  =" << endl;
	cout << "=   Daniel Barsky                    =" << endl;
	cout << "=           Winter 2011              =" << endl;
	cout << "======================================" << endl;
	if (argc < 3 || argc > 6)
	{
		cout << "Error: invalid number of arguments" <<endl;
		cout << "Usage: legoland.exe <source_model.ply> <destination_model.dat> [model height] [model thickness] [bricks per step]" << endl;
		exit(-1);
	}

	char *source_model, *dest_model;
	source_model = argv[1];
	dest_model = argv[2];

	if (argc >= 4) 
		model_height = atof(argv[3]);

	if (argc >= 5)
		thickness = atoi(argv[4]);

	if (argc == 6)
		bricksPerStep = atoi(argv[5]);

	// read object
	Model model(source_model, model_height);

	// Pre processing of triangles - splitting each triangle which is too big into small ones
	model.Supersample();
 
	// normalizing the point and triangles to be in the positive range
	model.Normalize();

	// building voxels out of model points
	VoxelImage * pVoxelImage = model.Voxelization();

	// Thickening VoxelImage to get a stable model
	pVoxelImage->Thicken(thickness);

	// visualization using OSG
	osgViewer::Viewer viewer;
//	osg::ref_ptr<osg::Group> root = pVoxelImage->DrawScene();
//	osg::ref_ptr<osg::Group> root = pVoxelImage->DrawSlice(Z, 5);

	// devision into bricks
	vector<Brick *> *list = Brick::BrickifyVoxelImage(pVoxelImage);
	// export .DAT file for visualization in MLCad
	Brick::ExportBrickModel(*list, dest_model, bricksPerStep);

	final = clock();

	// visualization using OSG
	osg::ref_ptr<osg::Group> root = Brick::DrawBricks(list);
	viewer.setSceneData(root.get());

	//while( !viewer.done()){ 
	//	viewer.frame();
	//}
	cout << "Time taken: " << (double)(final-init) / ((double)CLOCKS_PER_SEC) << " seconds" << endl;
	return viewer.run();
}
