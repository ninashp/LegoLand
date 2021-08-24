#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

#include "VoxelImage.hpp"
#include "Brick.hpp"
#include "Plane.hpp"
#include "Model.hpp"

using namespace std;

vector<Brick*> Brick::_availableBricks;
vector<color> Brick::_availableColors;

Brick::Brick () : _colorNumber(0), _count(0)
{
	_coordinates[X] = -1;
	_coordinates[Y] = -1;
	_coordinates[Z] = -1;

	_size[X] = -1;
	_size[Y] = -1;
};

Brick::Brick (unsigned int x, unsigned int y, unsigned int z, unsigned int size1, unsigned int size2, string filename, unsigned int colorNumber): 
	_colorNumber(colorNumber), _count(0)
{
	_coordinates[X] = x;
	_coordinates[Y] = y;
	_coordinates[Z] = z;
	_filename = filename;
	_size[X] = size1;
	_size[Y] = size2;
};

void Brick::InitiateAvailableBricks()
{
	Brick* availableBrick;
	_availableBricks.clear();
	availableBrick = new Brick(0, 0, 0, 8, 2, "3007.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 6, 2, "2456.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 4, 2, "3001.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 3, 2, "3002.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 2, 2, "3003.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 8, 1, "3008.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 6, 1, "3009.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 4, 1, "3010.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 3, 1, "3622.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 2, 1, "3004.DAT");
	_availableBricks.push_back(availableBrick);
	availableBrick = new Brick(0, 0, 0, 1, 1, "3005.DAT");
	_availableBricks.push_back(availableBrick);
	return;
};

void Brick::InitiateAvailableColors()
{
	color availableColor;
	_availableColors.clear();
	availableColor._red = 51;
	availableColor._green = 51;
	availableColor._blue = 51;
	_availableColors.push_back(availableColor);
	availableColor._red = 0;
	availableColor._green = 51;
	availableColor._blue = 178;
	_availableColors.push_back(availableColor);
	availableColor._red = 0;
	availableColor._green = 127;
	availableColor._blue = 51;
	_availableColors.push_back(availableColor);
	availableColor._red = 0;
	availableColor._green = 181;
	availableColor._blue = 166;
	_availableColors.push_back(availableColor);
	availableColor._red = 204;
	availableColor._green = 0;
	availableColor._blue = 0;
	_availableColors.push_back(availableColor);
	availableColor._red = 255;
	availableColor._green = 51;
	availableColor._blue = 153;
	_availableColors.push_back(availableColor);
	availableColor._red = 102;
	availableColor._green = 51;
	availableColor._blue = 0;
	_availableColors.push_back(availableColor);
	availableColor._red = 153;
	availableColor._green = 153;
	availableColor._blue = 153;
	_availableColors.push_back(availableColor);
	availableColor._red = 102;
	availableColor._green = 102;
	availableColor._blue = 88;
	_availableColors.push_back(availableColor);
	availableColor._red = 0;
	availableColor._green = 128;
	availableColor._blue = 255;
	_availableColors.push_back(availableColor);
	availableColor._red = 51;
	availableColor._green = 255;
	availableColor._blue = 102;
	_availableColors.push_back(availableColor);
	availableColor._red = 171;
	availableColor._green = 253;
	availableColor._blue = 249;
	_availableColors.push_back(availableColor);
	availableColor._red = 255;
	availableColor._green = 0;
	availableColor._blue = 0;
	_availableColors.push_back(availableColor);
	availableColor._red = 255;
	availableColor._green = 176;
	availableColor._blue = 204;
	_availableColors.push_back(availableColor);
	availableColor._red = 255;
	availableColor._green = 229;
	availableColor._blue = 0;
	_availableColors.push_back(availableColor);
	availableColor._red = 255;
	availableColor._green = 255;
	availableColor._blue = 255;
	_availableColors.push_back(availableColor);
}

void Brick::ClearAvailableBricks()
{
	while (!_availableBricks.empty())
	{
		delete _availableBricks.front();
		_availableBricks.erase(_availableBricks.begin());
	}
};

void Brick::BrickifyPlane(VoxelImage* Image, unsigned int z, vector<Brick*>* list)
{
	if (_availableBricks.empty())
		InitiateAvailableBricks();
	if (_availableColors.empty())
		InitiateAvailableColors();
	Plane* slice = Image->GetSlice(z);
	
	//if (slice == NULL)
	//{
	//	cout << "Invalid z = " << z << "!" << endl;
	//	return;
	//}
	Plane* perpendicularPlane;
	coordinate mainCoordinate, secondaryCoordinate;
	if (z%2 == 0)
	{
		mainCoordinate = X;
		secondaryCoordinate = Y;
		perpendicularPlane = Image->_yz_plane;
	}
	else
	{
		mainCoordinate = Y;
		secondaryCoordinate = X;
		perpendicularPlane = Image->_xz_plane;
	}

	for (unsigned int i = 0; i < (int) Brick::_availableBricks.size(); i++) // Go over all available bricks
	{
		for (unsigned int j = 0; j <= slice->_max_val[secondaryCoordinate]; j++) // Go over all pixels in a slice by the main coordinate
		{
			if (perpendicularPlane->GetVoxelByCoordinate(j, z) == NULL)
				continue;
			unsigned int mainsize = max(Brick::_availableBricks[i]->_size[mainCoordinate], Brick::_availableBricks[i]->_size[secondaryCoordinate]);
			unsigned int secondarysize = min(Brick::_availableBricks[i]->_size[mainCoordinate], Brick::_availableBricks[i]->_size[secondaryCoordinate]);
			unsigned int num = BrickFit (slice, secondaryCoordinate, j, z, mainsize, secondarysize, list, Brick::_availableBricks[i]->_filename);
			Brick::_availableBricks[i]->_count = Brick::_availableBricks[i]->_count + num;
			num = BrickFit (slice, secondaryCoordinate, j, z, secondarysize, mainsize, list, Brick::_availableBricks[i]->_filename);
			Brick::_availableBricks[i]->_count = Brick::_availableBricks[i]->_count + num;
		}
	}

	delete slice;
	return;
};

unsigned int Brick::BrickFit (Plane* slice, coordinate secondaryCoordinate, unsigned int coorValue, unsigned int z, unsigned int mainBrickSize, unsigned int SecondaryBrickSize, vector<Brick*>* list, string filename)
{
	int bricksFitted = 0;
	int currentColor = -1;
	int xvalue;
	int yvalue;
	bool xIsPrimary = (secondaryCoordinate == Y);
	coordinate mainCoordinate = (xIsPrimary ? X : Y);
	if (!Brick::InBounds(slice, 0, coorValue+SecondaryBrickSize, xIsPrimary)) // Out of bounds
		return 0;
	unsigned int counter = 0;
	for (unsigned int i = 0; i <= slice->_max_val[mainCoordinate]; i++)
	{
		xvalue = (xIsPrimary? i : coorValue);
		yvalue = (xIsPrimary? coorValue : i);
		if (counter == 0 && slice->GetVoxelByCoordinate(xvalue,yvalue) != NULL)
		{
			currentColor = slice->GetVoxelByCoordinate(xvalue,yvalue)->_colorBrickNumber;
		}
		if (slice->_max_val[mainCoordinate]-i < mainBrickSize-counter)
			break;
		bool in = true;
		for (unsigned int j = 0; j < SecondaryBrickSize; j++)
		{
			xvalue = (xIsPrimary? i : coorValue + j);
			yvalue = (xIsPrimary? coorValue + j : i);
			if (currentColor == -1 && slice->GetVoxelByCoordinate(xvalue,yvalue) != NULL)
			{
				currentColor = slice->GetVoxelByCoordinate(xvalue,yvalue)->_colorBrickNumber;
			}
			in = in && (slice->GetVoxelByCoordinate(xvalue,yvalue) != NULL) &&
				(slice->GetVoxelByCoordinate(xvalue,yvalue)->_colorBrickNumber == currentColor ||
				slice->GetVoxelByCoordinate(xvalue,yvalue)->_colorBrickNumber == -1); 
		}

		if (in == true)
			counter++;
		else
			counter = 0;
		if (counter == mainBrickSize)
		{
			if (currentColor == -1)
				currentColor = 0;
			Brick* brick;
			if (xIsPrimary)
				brick = new Brick(i-counter+1, coorValue, z, mainBrickSize, SecondaryBrickSize, filename, currentColor);
			else
				brick = new Brick(coorValue, i-counter+1, z, SecondaryBrickSize, mainBrickSize, filename, currentColor);
			for (unsigned int k = 0; k < mainBrickSize; k++)
			{
				for (unsigned int l = 0; l < SecondaryBrickSize; l++)
				{
					if (xIsPrimary)
					{
						slice->_planeMatrix[i-counter+1+k][coorValue+l] = NULL;
					}
					else
					{
						slice->_planeMatrix[coorValue+l][i-counter+1+k] = NULL;
					}
				}
			}
			list->push_back(brick);
			bricksFitted++;
			counter = 0;
		}
	}
	return bricksFitted;
};

bool Brick::InBounds(Plane* slice, unsigned int a, unsigned int b, bool xIsPrimary)
{
	if (xIsPrimary)
		return ((a >= 0) && (a <= slice->_max_val[X]) && (b >= 0) && (b <= slice->_max_val[Y]));
	else
		return ((a >= 0) && (a <= slice->_max_val[Y]) && (b >= 0) && (b <= slice->_max_val[X]));
};

vector<Brick*>* Brick::BrickifyVoxelImage(VoxelImage* vimage)
{
	cout << "Converting voxel model to bricks..." << endl;
	vector<Brick*>* list = new vector<Brick*>;
	list->clear();
	for (unsigned int z = 0; z <= vimage->GetMaxZ(); z++)
	{
		if ((z > 0 && z % 10 == 0) || (z == vimage->GetMaxZ()))
			cout << "Converted plane z = " << z << endl;
		BrickifyPlane(vimage, z, list);
	}
	return list;
};

osg::ref_ptr<osg::Group> Brick::DrawBricks(vector<Brick*>* list)
{
	osg::ref_ptr<osg::Group> root = Voxel::PrepareScene();

	for (unsigned int i = 0; i < list->size(); i++)
	{
		Brick::DrawSingleBrick(root, list->at(i));
	}

	return root;
};

osg::ref_ptr<osg::Group> Brick::DrawBrickSlice(vector<Brick*>* list, unsigned int z)
{
	osg::ref_ptr<osg::Group> root = Voxel::PrepareScene();

	for (unsigned int i = 0; i < list->size(); i++)
	{
		if (list->at(i)->_coordinates[Z]==z)
			Brick::DrawSingleBrick(root, list->at(i));
	}

	return root;
};

osg::ref_ptr<osg::Group> Brick::DrawSingleBrick(osg::ref_ptr<osg::Group> root, Brick* brick)
{
	float dx = float(VOXEL_DIM_X);
	float dy = float(VOXEL_DIM_Y);
	float dz = float(VOXEL_DIM_Z);
	float stud_height = float(STUD_HEIGHT);
	float stud_diam = float(STUD_DIAMETER);

	color curr_color = Brick::_availableColors [brick->_colorNumber];

	osg::Vec4 color_vec4 ((float) curr_color._red/255.0, (float)curr_color._green/255.0, (float)curr_color._blue/255.0, 1.0);

	osg::ref_ptr<osg::Group> BrickGroup = new osg::Group();
	osg::ref_ptr<osg::Box> BrickUnit = new osg::Box(osg::Vec3((double)brick->_size[X]/2*dx, (double)brick->_size[Y]/2*dy, 0), (brick->_size[X] - 0.1) * dx, (brick->_size[Y] - 0.1) * dy, 0.9*dz);
	osg::ref_ptr<osg::ShapeDrawable> BrickDrawable = new osg::ShapeDrawable(BrickUnit.get());
	BrickDrawable->setColor(color_vec4);
	osg::ref_ptr<osg::Geode> BrickGeode = new osg::Geode();
	osg::ref_ptr<osg::Cylinder> StudUnit = new osg::Cylinder(osg::Vec3(dx/2, dy/2, (0.9*dz)/2+stud_height/2), stud_diam/2, stud_height);
	osg::ref_ptr<osg::ShapeDrawable> StudDrawable = new osg::ShapeDrawable(StudUnit.get());
	StudDrawable->setColor(color_vec4);
	osg::ref_ptr<osg::Geode> StudGeode = new osg::Geode();
	osg::ref_ptr<osg::Group> StudGroup = new osg::Group();
	osg::ref_ptr<osg::PositionAttitudeTransform> studXform;
	StudGeode->addDrawable(StudDrawable);
	for (unsigned int i = 0; i < brick->_size[X]; i++)
	{
		for (unsigned int j = 0; j < brick->_size[Y]; j++)
		{
			studXform = new osg::PositionAttitudeTransform();
			studXform.get()->setPosition(osg::Vec3(i*dx, j*dy, 0));
			studXform.get()->addChild(StudGeode);
			StudGroup->addChild(studXform);
		}
	}
	BrickGeode->addDrawable(BrickDrawable);
	BrickGroup->addChild(BrickGeode);
	BrickGroup->addChild(StudGroup);

	osg::ref_ptr<osg::PositionAttitudeTransform> Xform = 
		new osg::PositionAttitudeTransform();
	Xform.get()->setPosition(osg::Vec3(brick->_coordinates[X]*dx, brick->_coordinates[Y]*dy, brick->_coordinates[Z]*dz));
	Xform.get()->addChild(BrickGroup);
	root->addChild(Xform.get());

	return root;
};

bool Brick::CompareBricks (Brick *a, Brick *b)
{
	return ((a->_coordinates[Z] < b->_coordinates[Z]) || 
		((a->_coordinates[Z] == b->_coordinates[Z]) && (a->_coordinates[Y] < b->_coordinates[Y])) ||
		((a->_coordinates[Z] == b->_coordinates[Z]) && (a->_coordinates[Y] == b->_coordinates[Y]) && (a->_coordinates[X] == b->_coordinates[X])));
}

vector<Brick*> *Brick::SortBrickList(vector<Brick*> *bricklist)
{
	sort(bricklist->begin(), bricklist->end(), CompareBricks);
	return bricklist;
}

void Brick::ExportBrickModel (vector<Brick*> BrickList, string out_filename, unsigned int bricksPerStep)
{
	cout << "Exporting model to file " << out_filename << "..." << endl;
	const char *pFilename;
	SortBrickList(&BrickList);
	unsigned int brickCounter = 0;
	pFilename = out_filename.c_str();
	std::ofstream outfile (pFilename, ios::out);
	outfile << "0 LegoLand Model Generator" << std::endl;
	outfile << "0 Created by: Daniel Barsky & Nina Shpalensky" << std::endl;
	outfile << "0 Filename: " << out_filename << std::endl;
	int a, b, c, d, e, f, g, h, i, x, y, z;
	int y_prev = 0;
	bool first_run = true;
	for (unsigned int n = 0; n < BrickList.size(); n++)
	{
		// Convert to MLCad coordinates
		x = (BrickList[n]->_coordinates[Y]+(double)BrickList[n]->_size[Y]/2)*20;
		y = -int(BrickList[n]->_coordinates[Z]*24);
		z = (BrickList[n]->_coordinates[X]+(double)BrickList[n]->_size[X]/2)*20;
		if (BrickList[n]->_size[X] < BrickList[n]->_size[Y]) // Determine brick orientation
		{
			a = 1;
			g = 0;
			c = 0;
			i = 1;
		}
		else
		{
			a = 0;
			g = -1;
			c = 1;
			i = 0;
		}
		if (first_run || (bricksPerStep == 0 && y != y_prev) || (bricksPerStep > 0 && brickCounter == bricksPerStep-1))
		{
			first_run = false;
			brickCounter = 0;
			outfile << "0 STEP" << std::endl;
		}
		else if (bricksPerStep > 0)
			brickCounter++;
		b = 0;
		d = 0;
		e = 1;
		f = 0;
		h = 0;
		outfile << "1 " << BrickList[n]->_colorNumber << " " << x << " " << y << " " << z << " " << a << " " << b << " " << c << " " << d << " " << e << " " << f << " " << g << " " << h << " " << i << " " << BrickList[n]->_filename << std::endl;
		if ((n > 0 && n % 1000 == 0) || n == BrickList.size()-1)
			cout << "Exported brick " << n << " out of " << BrickList.size() << std::endl;
		y_prev = y;
	}
	cout << "Model exported successfully!" << endl;
}
