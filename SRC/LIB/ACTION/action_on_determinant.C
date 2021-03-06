// action_on_determinant.C
//
// Anton Betten
// January 16, 2009

#include "galois.h"
#include "action.h"

INT action_on_determinant::cntr_new = 0;
INT action_on_determinant::cntr_objects = 0;
INT action_on_determinant::f_debug_memory = FALSE;

void *action_on_determinant::operator new(size_t bytes)
{
	cntr_new++;
	cntr_objects++;
	if (f_debug_memory) {
		cout << "action_on_determinant::operator new bytes=" << bytes 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	return malloc(bytes);
}

void *action_on_determinant::operator new[](size_t bytes)
{
	INT n;
	
	n = bytes / sizeof(action_on_determinant);
	cntr_new++;
	cntr_objects += n;
	if (f_debug_memory) {
		cout << "action_on_determinant::operator new[] n=" << n 
			<< " bytes=" << bytes 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	return malloc(bytes);
}

void action_on_determinant::operator delete(void *ptr, size_t bytes)
{
	if (f_debug_memory) {
		cout << "action_on_determinant::operator delete bytes=" << bytes 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	cntr_new--;
	cntr_objects--;
	return ::free(ptr);
}

void action_on_determinant::operator delete[](void *ptr, size_t bytes)
{
	INT n;
	
	n = bytes / sizeof(action_on_determinant);
	if (f_debug_memory) {
		cout << "action_on_determinant::operator delete[] n=" << n 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	cntr_new--;
	cntr_objects -= n;
	return ::free(ptr);
}

action_on_determinant::action_on_determinant()
{
	null();
}

action_on_determinant::~action_on_determinant()
{
	free();
}

void action_on_determinant::null()
{
	M = NULL;
}

void action_on_determinant::free()
{
	
	null();
}


void action_on_determinant::init(action &A, INT f_projective, INT m, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	longinteger_object go;
	
	if (f_v) {
		cout << "action_on_determinant::init" << endl;
		cout << "f_projective=" << f_projective << endl;
		cout << "m=" << m << endl;
		}
	action_on_determinant::f_projective = f_projective;
	action_on_determinant::m = m;
	if (A.type_G != matrix_group_t) {
		cout << "action_on_determinant::init action not of matrix group type" << endl;
		exit(1);
		}
	M = A.G.matrix_grp;
	q = M->GFq->q;
	if (f_projective) {
		degree = gcd_INT(m, q - 1);
		}
	else {
		degree = q - 1;
		}
	if (f_v) {
		cout << "degree=" << degree << endl;
		}
	
	if (f_v) {
		cout << "action_on_determinant::init field order is " << q << endl;
		}
}

void action_on_determinant::compute_image(action *A, INT *Elt, INT i, INT &j, INT verbose_level)
{
	//verbose_level = 1;
	INT f_v = (verbose_level >= 1);
	INT a, b, c, l;
	
	if (f_v) {
		cout << "action_on_determinant::compute_image i = " << i << endl;
		}
	if (i < 0 || i >= degree) {
		cout << "action_on_determinant::compute_image i = " << i << " out of range" << endl;
		exit(1);
		}
	if (f_projective) {
		a = M->GFq->alpha_power(i);
		}
	else {
		a = i + 1;
		}
	b = M->GFq->matrix_determinant(Elt, M->n, 0);
	c = M->GFq->mult(a, b);
	if (f_projective) {
		l = M->GFq->log_alpha(c);
		j = l % degree;
		}
	else {
		j = c - 1;
		}
	if (f_v) {
		cout << "action_on_determinant::compute_image det = " << b << endl;
		cout << "action_on_determinant::compute_image " << a << " * " << b << " = " << c << endl;
		if (f_projective) {
			cout << "f_projective, a = " << a << " l = " << l << " c = " << c << endl;
			}
		cout << "image of " << i << " is " << j << endl;
		}
}

