#include <dbscan.h>


// --------------------------------------------------------------------------
double distance(point_t *a, point_t *b)
// --------------------------------------------------------------------------
{
return sqrt((a->x-b->x)*(a->x-b->x)+(a->y-b->y)*(a->y-b->y)+(a->z-b->z)*(a->z-b->z));
}


// --------------------------------------------------------------------------
int main( int argc, char** argv )
// --------------------------------------------------------------------------
{
	DBScan * db_scan = new DBScan();

	std::vector<point_s> vector_points;
	//fill in the vector

	//db scan parameters
	double epsilon=0.3;
	unsigned int minpts=4;

         point_s point1;
	 point1.x=1;
	 point1.y=2;
	 point1.z=0;
	 point1.cluster_id=-1;

         point_s point2;
 point2.x=1;
	 point2.y=2;
	 point2.z=0;
	 point2.cluster_id=-1;

         point_s point3;
 point3.x=1;
	 point3.y=2;
	 point3.z=0;
	 point3.cluster_id=-1;

         point_s point4;
 point4.x=1;
	 point4.y=2;
	 point4.z=0;
	 point4.cluster_id=-1;

         point_s point5;
 point5.x=1;
	 point5.y=2;
	 point5.z=0;
	 point5.cluster_id=-1;

vector_points.push_back(point1);
vector_points.push_back(point2);
vector_points.push_back(point3);
vector_points.push_back(point4);
vector_points.push_back(point5);


	//running the db scan
	if (vector_points.size()>0) {
		db_scan->dbscan(vector_points, epsilon,
				minpts, distance);
		printf("Epsilon: %lf\n", epsilon);
		printf("Minimum points: %u\n", minpts);
	}

	for(int i=0;i<vector_points.size();i++)
	{
		printf("point i: %d, x: %f, y:%f, cluster %d \n", i, vector_points[i].x,vector_points[i].y,vector_points[i].cluster_id);

		
	}


	sleep(1000);
	return 0;
}
