#include <iostream>
#include <fstream>
#include <string>

#include "Model.hpp"
#include "VoxelImage.hpp"

using namespace std;
using std::vector;

Model::Model(char* ascii_filepath, double model_hight): _model_hight(model_hight), _isColored(false)
{
	std::ifstream fin(ascii_filepath);

	int num_points = 0;
	int num_triangles = 0;
	int edge_index;
	_max_x = 0;
	_max_y = 0;
	_max_z = 0;

	if(!fin) {
		std::cout << "Error! No file found" << std::endl;
		exit(-1);
	}

	std::string pgarb = "";
	fin >> pgarb;

	if (pgarb != "ply") {
		std::cout << "Error! Incorrect format" << std::endl;
		exit(-1);
	}
	
	std::cout << "Importing file..." << std::endl;
	fin >> pgarb;

	while (pgarb != "end_header") {
		if (pgarb == "vertex") {
			fin >> num_points;
			cout << num_points << " vertices"  << endl;
		}
		if (pgarb == "face") {
			fin >> num_triangles;
			cout << num_triangles << " triangles" <<  endl;
		}
		if (pgarb == "red")
			_isColored = true;
		fin >> pgarb;
	}
	if (_isColored)
		cout << "Model is colored" << endl;
	else
		cout << "Model isn't colored" << endl;
	for (int i = 1 ; i <= num_points ; i++) {
		Point* newPoint = new Point;
		if (_isColored) {
			fin >> newPoint->_x
				>> newPoint->_z
				>> newPoint->_y
				>> int(newPoint->_red)
				>> int(newPoint->_green)
				>> int(newPoint->_blue)
				>> int(newPoint->_alpha);
		}
		else {
			fin >> newPoint->_x
				>> newPoint->_z
				>> newPoint->_y;
			newPoint->_red = 0;
			newPoint->_green = 0;
			newPoint->_blue = 0;
			newPoint->_alpha = 255;
		}
		_points.push_back(newPoint);
	}

	for (int i = 1 ; i <= num_triangles ; i++) {
		Triangle* newTriangle = new Triangle;
		int point1Index, point2Index, point3Index;
		fin >> edge_index
			>> point1Index
			>> point2Index
			>> point3Index;
		newTriangle->_point1 = _points[point1Index];
		newTriangle->_point2 = _points[point2Index];
		newTriangle->_point3 = _points[point3Index];
		_triangles.push_back(newTriangle);
		double distance12 = sqrt((newTriangle->_point1->_x+newTriangle->_point2->_x)*(newTriangle->_point1->_x+newTriangle->_point2->_x) + 
							(newTriangle->_point1->_y+newTriangle->_point2->_y)*(newTriangle->_point1->_y+newTriangle->_point2->_y) +
							(newTriangle->_point1->_z+newTriangle->_point2->_z)*(newTriangle->_point1->_z+newTriangle->_point2->_z));
		double distance13 = sqrt((newTriangle->_point1->_x+newTriangle->_point3->_x)*(newTriangle->_point1->_x+newTriangle->_point3->_x) + 
							(newTriangle->_point1->_y+newTriangle->_point3->_y)*(newTriangle->_point1->_y+newTriangle->_point3->_y) +
							(newTriangle->_point1->_z+newTriangle->_point3->_z)*(newTriangle->_point1->_z+newTriangle->_point3->_z));
		double distance32 = sqrt((newTriangle->_point3->_x+newTriangle->_point2->_x)*(newTriangle->_point3->_x+newTriangle->_point2->_x) + 
							(newTriangle->_point3->_y+newTriangle->_point2->_y)*(newTriangle->_point3->_y+newTriangle->_point2->_y) +
							(newTriangle->_point3->_z+newTriangle->_point2->_z)*(newTriangle->_point3->_z+newTriangle->_point2->_z));
	}
	cout << "File imported successfully!" << endl;
	fin.close();
}

void  Model::Supersample () 
{
	
	double threshold = 0;

	if ((double)VOXEL_DIM_X < min((double)VOXEL_DIM_Y,(double)VOXEL_DIM_Z))
		threshold = (double)VOXEL_DIM_X;
	else
		threshold = min((double)VOXEL_DIM_Y,(double)VOXEL_DIM_Z);
	
	// rescaling 
	// find max & min z distance
	double minZ = _points[0]->_z;
	double maxZ = _points[0]->_z;
	for (unsigned int i = 1 ; i < _points.size() ; i++) {
		maxZ = (_points[i]->_z > maxZ ? _points[i]->_z : maxZ);
		minZ = (_points[i]->_z < minZ ? _points[i]->_z : minZ);
	}

	float scale = _model_hight/(maxZ-minZ);
	for (unsigned int j = 0 ; j < _points.size() ; j++) {
		_points[j]->_x = _points[j]->_x*scale;
		_points[j]->_y = _points[j]->_y*scale;
		_points[j]->_z = _points[j]->_z*scale;
	}

	cout << "Model scale: " << scale << endl;
	Point *point_a , *point_b, *point_c;

	double dx1, dx2, dx3, dy1, dy2, dy3, dz1, dz2, dz3, l1, l2, l3, max_l;

	cout << "Supesampling...";
	unsigned int i = 0;
	unsigned int iteration = 0;
	do 	{
		if (iteration++%1000 == 0)
			cout << ".";
		// Calculate distances
		dx1 = abs(_triangles[i]->_point3->_x-_triangles[i]->_point1->_x);
		dx2 = abs(_triangles[i]->_point1->_x-_triangles[i]->_point2->_x);
		dx3 = abs(_triangles[i]->_point2->_x-_triangles[i]->_point3->_x);
		dy1 = abs(_triangles[i]->_point3->_y-_triangles[i]->_point1->_y);
		dy2 = abs(_triangles[i]->_point1->_y-_triangles[i]->_point2->_y);
		dy3 = abs(_triangles[i]->_point2->_y-_triangles[i]->_point3->_y);
		dz1 = abs(_triangles[i]->_point3->_z-_triangles[i]->_point1->_z);
		dz2 = abs(_triangles[i]->_point1->_z-_triangles[i]->_point2->_z);
		dz3 = abs(_triangles[i]->_point2->_z-_triangles[i]->_point3->_z);
		// Calculate edge lengths
		l1 = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
		l2 = sqrt(dx2*dx2 + dy2*dy2 + dz2*dz2);
		l3 = sqrt(dx3*dx3 + dy3*dy3 + dz3*dz3);

		max_l = max(l1, max(l2, l3));
		if (max_l < threshold) {  // All edges are below the threshold
			i++;
			continue;
		}
		if (l1 > max(l2, l3)) {
			point_a = _triangles[i]->_point1;
			point_b = _triangles[i]->_point3;
			point_c = _triangles[i]->_point2;
		}
		else {
			if (l2 > l3) {
			point_a = _triangles[i]->_point1;
			point_b = _triangles[i]->_point2;
			point_c = _triangles[i]->_point3;
			}
		else {
				point_a = _triangles[i]->_point2;
				point_b = _triangles[i]->_point3;
				point_c = _triangles[i]->_point1;
			}
		}

		// Divide the triangle into 2 triangles
		Point* new_point = new Point;
		new_point->_x = (point_a->_x + point_b->_x)/2;
		new_point->_y = (point_a->_y + point_b->_y)/2;
		new_point->_z = (point_a->_z + point_b->_z)/2;
		new_point->_red = (point_a->_red + point_b->_red)/2;
		new_point->_green = (point_a->_green + point_b->_green)/2;
		new_point->_blue = (point_a->_blue + point_b->_blue)/2;
		new_point->_alpha = (point_a->_alpha + point_b->_alpha)/2;
		_points.push_back(new_point);
		Triangle* new_triangle1 = new Triangle;
		Triangle* new_triangle2 = new Triangle;
		new_triangle1->_point1 = point_a;
		new_triangle1->_point2 = point_c;
		new_triangle1->_point3 = new_point;
		new_triangle2->_point1 = new_point;
		new_triangle2->_point2 = point_b;
		new_triangle2->_point3 = point_c;
		_triangles.push_back(new_triangle1);
		_triangles.push_back(new_triangle2);
		_triangles.erase(_triangles.begin() + i);
	} while (i < _triangles.size());
	cout << "\nDone! Number of triangles: " << _triangles.size() << endl;
}

void  Model::Normalize () 
{
	cout << "Normalizing model..." << endl;
    // find min
    double xMin = _points[0]->_x;
	double yMin = _points[0]->_y;
	double zMin = _points[0]->_z;

	for (unsigned int i = 0 ; i < _points.size() ; i++) {
		xMin = min(_points[i]->_x,xMin);
		yMin = min(_points[i]->_y,yMin);
		zMin = min(_points[i]->_z,zMin);
	}

	// normalize
	for (unsigned int i = 0 ; i < _points.size() ; i++) {
		_points[i]->_x = _points[i]->_x - xMin;
		_points[i]->_y = _points[i]->_y - yMin;
		_points[i]->_z = _points[i]->_z - zMin;
		_max_x = (_points[i]->_x > _max_x ? _points[i]->_x : _max_x);
		_max_y = (_points[i]->_y > _max_y ? _points[i]->_y : _max_y);
		_max_z = (_points[i]->_z > _max_z ? _points[i]->_z : _max_z);
	}
}

VoxelImage * Model::Voxelization() 
{
	VoxelImage * pVoxelImage = new VoxelImage(ceil(_max_x/VOXEL_DIM_X-0.5)+1, 
											  ceil(_max_y/VOXEL_DIM_Y-0.5)+1, 
											  ceil(_max_z/VOXEL_DIM_Z-0.5)+1, 
											  _isColored);
	cout << "Voxelizing..." << endl;
	for (unsigned int i = 0 ; i < _points.size() ; i++) {
		int x = ceil((_points[i]->_x)/VOXEL_DIM_X-0.5);
		int y = ceil((_points[i]->_y)/VOXEL_DIM_Y-0.5);
		int z = ceil((_points[i]->_z)/VOXEL_DIM_Z-0.5);
		Voxel* pNewVoxel = new Voxel(x, y, z, _points[i]->_red, _points[i]->_green, _points[i]->_blue, _points[i]->_alpha);
		pVoxelImage->AddVoxel(pNewVoxel);
		if ((i > 0 && i%10000 == 0) || i == _points.size()-1)
			cout << "Point " << i << " out of "<< _points.size() << " voxelized" <<  endl;
	}

	// updating colors
	if (pVoxelImage->_isColored) {
		pVoxelImage->UpdateVoxelColors();
	}

	return pVoxelImage;
}
