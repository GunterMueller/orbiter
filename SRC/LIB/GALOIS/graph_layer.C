// graph_layer.C
// 
// Anton Betten
// December 30, 2013
//
//
// 
//
//

#include "galois.h"

graph_layer::graph_layer()
{
	null();
}

graph_layer::~graph_layer()
{
	freeself();
}

void graph_layer::null()
{
	Nodes = NULL;
}

void graph_layer::freeself()
{
	if (Nodes) {
		delete [] Nodes;
		}
	null();
}

void graph_layer::init(INT nb_nodes, INT id_of_first_node, INT verbose_level)
{
	INT i, id;

	graph_layer::id_of_first_node = id_of_first_node;
	graph_layer::nb_nodes = nb_nodes;
	Nodes = new graph_node[nb_nodes];
	id = id_of_first_node;
	for (i = 0; i < nb_nodes; i++) {
		Nodes[i].id =id;
		Nodes[i].layer = i;
		Nodes[i].label = NULL;
		id++;
		}
}

void graph_layer::place(INT verbose_level)
{
	double dx, dx2;
	INT i;

	dx = 1. / nb_nodes;
	dx2 = dx * .5;
	for (i = 0; i < nb_nodes; i++) {
		Nodes[i].x_coordinate = i * dx + dx2;
		}
	
}

void graph_layer::place_with_grouping(INT *group_size, INT nb_groups, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT *group_start;
	double *group_x;
	double *group_dx;
	INT i, j, nb_elements;

	if (f_v) {
		cout << "graph_layer::place_with_grouping nb_groups=" << nb_groups << endl;
		}
	group_start = NEW_INT(nb_groups + 1);
	group_dx = new double[nb_groups + 1];
	group_x = new double[nb_groups + 1];
	group_start[0] = 0;
	for (j = 0; j < nb_groups; j++) {
		group_start[j + 1] = group_start[j] + group_size[j];
		}
	nb_elements = group_start[nb_groups];
	for (j = 0; j < nb_groups; j++) {
		group_dx[j] = group_size[j] / (double) nb_elements;
		}
	for (j = 0; j < nb_groups; j++) {
		group_x[j] = (double) group_start[j] / (double) nb_elements + (double) group_dx[j] * .5;
		}
	for (j = 0; j < nb_groups; j++) {
		if (f_v) {
			cout << "j=" << j << " / " << nb_groups << " group_size[j]=" << group_size[j] << endl;
			}
		for (i = 0; i < group_size[j]; i++) {
			if (FALSE) {
				cout << "i=" << i << " / " << group_size[j] << endl;
				}
			Nodes[group_start[j] + i].x_coordinate = 
			group_x[j] - 
				((double) group_dx[j] * .5) * 0.4 + 
	(((double)i + .5) * group_dx[j] / (double) group_size[j]) * 0.4;
			}
		}
	FREE_INT(group_start);
	delete [] group_dx;
	delete [] group_x;
	if (f_v) {
		cout << "graph_layer::place_with_grouping done" << endl;
		}
}

void graph_layer::write_memory_object(memory_object *m, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;
	
	if (f_v) {
		cout << "graph_layer::write_memory_object " << nb_nodes << " nodes" << endl;
		}
	m->write_int(id_of_first_node);
	m->write_int(nb_nodes);
	m->write_int(id_of_first_node);
	for (i = 0; i < nb_nodes; i++) {
		Nodes[i].write_memory_object(m, verbose_level - 1);
		}
	m->write_double(y_coordinate);
	if (f_v) {
		cout << "graph_layer::write_memory_object finished, data size (in bytes) = " << m->used_length << endl;
		}
}

void graph_layer::read_memory_object(memory_object *m, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;
	
	if (f_v) {
		cout << "graph_layer::read_memory_object" << endl;
		}
	freeself();

	m->read_int(&id_of_first_node);
	m->read_int(&nb_nodes);
	m->read_int(&id_of_first_node);

	Nodes = new graph_node[nb_nodes];

	for (i = 0; i < nb_nodes; i++) {
		Nodes[i].read_memory_object(m, verbose_level - 1);
		}
	m->read_double(&y_coordinate);
	if (f_v) {
		cout << "graph_layer::read_memory_object finished" << endl;
		}
}





