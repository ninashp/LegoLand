#include <iostream>
#include <fstream>
#include <string>

#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>

#include "Voxel.hpp"
#include "Model.hpp"
#include "Brick.hpp"

using namespace std;

void Voxel::SetInitParams (unsigned int x, unsigned int y, unsigned int z, int red, int green, int blue, int alpha)
{
	_coordinates[X] = x;
	_coordinates[Y] = y;
	_coordinates[Z] = z;
	_color._red = red*alpha/255;
	_color._green = green*alpha/255;
	_color._blue = blue*alpha/255;

	for (unsigned int i = 0; i < 2; i++)
		for (unsigned int j = 0; j < 3; j++)
			_neighbors[i][j]=NULL;

	for (unsigned int i = 0; i < 16; i++)
		_colorHistogram[i] = 0;
}

int Voxel::QuantizeColor(color c)
{
	if (c._red == -1 || c._green == -1 || c._blue == -1)
		return -1;
	unsigned int minIndex = 0;
	double minDistance = 255*3;
	for (unsigned int i = 0; i < Brick::_availableColors.size(); i++) {
		double currentDistance = sqrt( double((c._red - Brick::_availableColors[i]._red)*(c._red - Brick::_availableColors[i]._red) +
											  (c._green - Brick::_availableColors[i]._green)*(c._green - Brick::_availableColors[i]._green) +
											  (c._blue - Brick::_availableColors[i]._blue)*(c._blue - Brick::_availableColors[i]._blue)) );
		if (currentDistance < minDistance) {
			minDistance = currentDistance;
			minIndex = i;
		}
	}
	return minIndex;
}

int Voxel::GetMajorityColor()
{
	unsigned int maxIndex = 0;
	unsigned int max = 0;
	for (unsigned int i = 0; i < Brick::_availableColors.size(); i++) {
		if (this->_colorHistogram[i] > max)	{
			max = this->_colorHistogram[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

osg::ref_ptr<osg::Group> Voxel::PrepareScene()
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::StateSet* state = root->getOrCreateStateSet();
	state->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	state->setMode( GL_LIGHT0, osg::StateAttribute::ON );
	return root;
}

osg::ref_ptr<osg::Group> Voxel::DrawSingleVoxel(osg::ref_ptr<osg::Group> root, Voxel* voxel)
{
	if (voxel == NULL) {
		cout << "Trying to print invalid voxel!" << endl;
		return NULL;
	}

	float dx = float(VOXEL_DIM_X);
	float dy = float(VOXEL_DIM_Y);
	float dz = float(VOXEL_DIM_Z);
	float stud_height = float(STUD_HEIGHT);
	float stud_diam = float(STUD_DIAMETER);

	osg::ref_ptr<osg::Group> VoxelGroup = new osg::Group();
	osg::ref_ptr<osg::Box> VoxelUnit = new osg::Box(osg::Vec3(0, 0, 0), dx, dy, dz);
	osg::ref_ptr<osg::ShapeDrawable> VoxelDrawable = new osg::ShapeDrawable(VoxelUnit.get());
	VoxelDrawable->setColor(osg::Vec4((float)voxel->_color._red/255.0, (float)voxel->_color._green/255.0, (float)voxel->_color._blue/255.0, 1.0));
	osg::ref_ptr<osg::Geode> VoxelGeode = new osg::Geode();
	osg::ref_ptr<osg::Cylinder> StudUnit = new osg::Cylinder(osg::Vec3(0, 0, dz/2+stud_height/2), stud_diam/2, stud_height);
	osg::ref_ptr<osg::ShapeDrawable> StudDrawable = new osg::ShapeDrawable(StudUnit.get());
	StudDrawable->setColor(osg::Vec4((float)voxel->_color._red/255.0, (float)voxel->_color._green/255.0, (float)voxel->_color._blue/255.0, 1.0));
	osg::ref_ptr<osg::Geode> StudGeode = new osg::Geode();
	StudGeode->addDrawable(StudDrawable);
	VoxelGeode->addDrawable(VoxelDrawable);
	VoxelGroup->addChild(VoxelGeode);
	VoxelGroup->addChild(StudGeode);

	osg::ref_ptr<osg::PositionAttitudeTransform> Xform = 
		new osg::PositionAttitudeTransform();
	Xform.get()->setPosition(osg::Vec3(voxel->_coordinates[X]*dx, voxel->_coordinates[Y]*dy, voxel->_coordinates[Z]*dz));
	Xform.get()->addChild(VoxelGroup);
	root->addChild(Xform.get());

	return root;
}
