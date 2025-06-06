#include "dbscan.h"

// --------------------------------------------------------------------------
DBScan::
DBScan()
// --------------------------------------------------------------------------
{

}

// --------------------------------------------------------------------------
DBScan::
DBScan(double min_x, double max_x, double min_y, double max_y, double resolution)
// --------------------------------------------------------------------------
{

}

// --------------------------------------------------------------------------
node_t *
DBScan::create_node(unsigned int index)
// --------------------------------------------------------------------------
{
	node_t *n = (node_t *) calloc(1, sizeof(node_t));
	if (n == NULL)
		perror("Failed to allocate node.");
	else {
		n->index = index;
		n->next = NULL;
	}
	return n;
}

// --------------------------------------------------------------------------
int DBScan::
append_at_end(
		unsigned int index,
		epsilon_neighbours_t *en)
// --------------------------------------------------------------------------
{
	node_t *n = create_node(index);
	if (n == NULL) {
		free(en);
		return FAILURE;
	}
	if (en->head == NULL) {
		en->head = n;
		en->tail = n;
	} else {
		en->tail->next = n;
		en->tail = n;
	}
	++(en->num_members);
	return SUCCESS;
}

// --------------------------------------------------------------------------
epsilon_neighbours_t *
DBScan::get_epsilon_neighbours(
		unsigned int index,
		std::vector<point_s> points,
		double epsilon,
		double (*dist)(point_t *a, point_t *b))
// --------------------------------------------------------------------------
{
	epsilon_neighbours_t *en = (epsilon_neighbours_t *)calloc(1, sizeof(epsilon_neighbours_t));

	if (en == NULL) {
		perror("Failed to allocate epsilon neighbours.");
		return en;
	}

	for (int i = 0; i < points.size(); ++i) {
		if (i == index)
			continue;
		if (dist(&points[index], &points[i]) > epsilon)
			continue;
		else {
			if (append_at_end(i, en) == FAILURE) {
				destroy_epsilon_neighbours(en);
				en = NULL;
				break;
			}
		}
	}
	return en;
}

// --------------------------------------------------------------------------
void
DBScan::print_epsilon_neighbours(
		std::vector<point_s> points,
		epsilon_neighbours_t *en)
// --------------------------------------------------------------------------
{
	if (en) {
		node_t *h = en->head;
		while (h) {
			printf("(%lf, %lf, %lf)\n",
					points[h->index].x,
					points[h->index].y,
					points[h->index].z);
			h = h->next;
		}
	}
}

// --------------------------------------------------------------------------
void
DBScan::destroy_epsilon_neighbours(epsilon_neighbours_t *en)
// --------------------------------------------------------------------------
{
	if (en) {
		node_t *t, *h = en->head;
		while (h) {
			t = h->next;
			free(h);
			h = t;
		}
		free(en);
	}
}

// --------------------------------------------------------------------------
void
DBScan::dbscan(
		std::vector<point_s> & points,
		double epsilon,
		unsigned int minpts,
		double (*dist)(point_t *a, point_t *b))
// --------------------------------------------------------------------------
{
	unsigned int i, cluster_id = 0;
	for (i = 0; i < points.size(); ++i) {
		if (points[i].cluster_id == UNCLASSIFIED) {
			if (expand(i, cluster_id, points,
					epsilon, minpts,
					dist) == CORE_POINT)
				++cluster_id;
		}
	}
}

// --------------------------------------------------------------------------
int
DBScan::expand(
		unsigned int index,
		unsigned int cluster_id,
		std::vector<point_s> & points,
		double epsilon,
		unsigned int minpts,
		double (*dist)(point_t *a, point_t *b))
// --------------------------------------------------------------------------
{
	int return_value = NOT_CORE_POINT;
	epsilon_neighbours_t *seeds =
			get_epsilon_neighbours(index, points,
					epsilon,
					dist);
	if (seeds == NULL)
		return FAILURE;

	if (seeds->num_members < minpts)
		points[index].cluster_id = NOISE;
	else {
		points[index].cluster_id = cluster_id;
		node_t *h = seeds->head;
		while (h) {
			points[h->index].cluster_id = cluster_id;
			h = h->next;
		}

		h = seeds->head;
		while (h) {
			spread(h->index, seeds, cluster_id, points,
					epsilon, minpts, dist);
			h = h->next;
		}

		return_value = CORE_POINT;
	}
	destroy_epsilon_neighbours(seeds);
	return return_value;
}

// --------------------------------------------------------------------------
int
DBScan::spread(
		unsigned int index,
		epsilon_neighbours_t *seeds,
		unsigned int cluster_id,
		std::vector<point_s> & points,
		double epsilon,
		unsigned int minpts,
		double (*dist)(point_t *a, point_t *b))
// --------------------------------------------------------------------------
{
	epsilon_neighbours_t *spread =
			get_epsilon_neighbours(index, points,
					epsilon,
					dist);
	if (spread == NULL)
		return FAILURE;
	if (spread->num_members >= minpts) {
		node_t *n = spread->head;
		point_t *d;
		while (n) {
			d = &points[n->index];
			if (d->cluster_id == NOISE ||
					d->cluster_id == UNCLASSIFIED) {
				if (d->cluster_id == UNCLASSIFIED) {
					if (append_at_end(n->index, seeds)
							== FAILURE) {
						destroy_epsilon_neighbours(spread);
						return FAILURE;
					}
				}
				d->cluster_id = cluster_id;
			}
			n = n->next;
		}
	}

	destroy_epsilon_neighbours(spread);
	return SUCCESS;
}


// --------------------------------------------------------------------------
void
DBScan::print_points(std::vector<point_s> points)
// --------------------------------------------------------------------------
{
	unsigned int i = 0;
	printf("Number of points: %u\n"
			" x     y     z     cluster_id\n"
			"-----------------------------\n"
			, points.size());
	while (i < points.size()) {
		printf("%5.2lf %5.2lf %5.2lf: %d\n",
				points[i].x,
				points[i].y, points[i].z,
				points[i].cluster_id);
		++i;
	}
}



// --------------------------------------------------------------------------
DBScan::
~DBScan()
// --------------------------------------------------------------------------
{
}

