#ifndef CROCO_CARMEN_AMBISENSE_SLAM_DB_SCAN
#define CROCO_CARMEN_AMBISENSE_SLAM_DB_SCAN


#include <string.h>
#include <map>
#include <string>
#include <vector>
#include <carmen/carmen.h>

/**
 * DB scan for clustering based on wifi similarity
 */

#define UNCLASSIFIED -1
#define NOISE -2

#define CORE_POINT 1
#define NOT_CORE_POINT 0

#define SUCCESS 0
#define FAILURE -3

typedef struct point_s point_t;
struct point_s {
	double x, y, z;//measurement laser location x, y
	int cluster_id;//-2, -1, 0, 1...
};

typedef struct node_s node_t;
struct node_s {
	unsigned int index;
	node_t *next;
};

typedef struct epsilon_neighbours_s epsilon_neighbours_t;
struct epsilon_neighbours_s {
	unsigned int num_members;
	node_t *head, *tail;
};


class DBScan
{

public:
	/** Constructor */
	DBScan();

	/** Desstructor */
	~DBScan();

	/** Constructor with the setting of the x y dimension and grid resolution*/
	DBScan(double min_x, double max_x, double min_y, double max_y, double resolution);


public:
	void dbscan(
			std::vector<point_s> & points,
			double epsilon,
			unsigned int minpts,
			double (*dist)(point_t *a, point_t *b));

	void print_points(std::vector<point_s> points);

	void print_epsilon_neighbours(
			std::vector<point_s> points,
			epsilon_neighbours_t *en);



private:

	node_t *create_node(unsigned int index);



	int append_at_end(unsigned int index,
			epsilon_neighbours_t *en);

	epsilon_neighbours_t *get_epsilon_neighbours(
			unsigned int index,
			std::vector<point_s> points,
			double epsilon,
			double (*dist)(point_t *a, point_t *b));



	void destroy_epsilon_neighbours(epsilon_neighbours_t *en);



	int expand(
			unsigned int index,
			unsigned int cluster_id,
			std::vector<point_s> & points,
			double epsilon,
			unsigned int minpts,
			double (*dist)(point_t *a, point_t *b));


	int spread(
			unsigned int index,
			epsilon_neighbours_t *seeds,
			unsigned int cluster_id,
			std::vector<point_s> &  points,
			double epsilon,
			unsigned int minpts,
			double (*dist)(point_t *a, point_t *b));


};

#endif // CROCO_CARMEN_AMBISENSE_SLAM_DB_SCAN
