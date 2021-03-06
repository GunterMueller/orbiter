// action_on_wedge_product.C
//
// Anton Betten
// Jan 26, 2010

#include "galois.h"
#include "action.h"

INT action_on_wedge_product::cntr_new = 0;
INT action_on_wedge_product::cntr_objects = 0;
INT action_on_wedge_product::f_debug_memory = FALSE;

void *action_on_wedge_product::operator new(size_t bytes)
{
	cntr_new++;
	cntr_objects++;
	if (f_debug_memory) {
		cout << "action_on_wedge_product::operator new bytes=" << bytes 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	return malloc(bytes);
}

void *action_on_wedge_product::operator new[](size_t bytes)
{
	INT n;
	
	n = bytes / sizeof(action_on_wedge_product);
	cntr_new++;
	cntr_objects += n;
	if (f_debug_memory) {
		cout << "action_on_wedge_product::operator new[] n=" << n 
			<< " bytes=" << bytes 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	return malloc(bytes);
}

void action_on_wedge_product::operator delete(void *ptr, size_t bytes)
{
	if (f_debug_memory) {
		cout << "action_on_wedge_product::operator delete bytes=" << bytes 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	cntr_new--;
	cntr_objects--;
	return ::free(ptr);
}

void action_on_wedge_product::operator delete[](void *ptr, size_t bytes)
{
	INT n;
	
	n = bytes / sizeof(action_on_wedge_product);
	if (f_debug_memory) {
		cout << "action_on_wedge_product::operator delete[] n=" << n 
			<< " cntr_new=" << cntr_new 
			<< " cntr_objects=" << cntr_objects 
			<< endl;
		}
	cntr_new--;
	cntr_objects -= n;
	return ::free(ptr);
}

action_on_wedge_product::action_on_wedge_product()
{
	null();
}

action_on_wedge_product::~action_on_wedge_product()
{
	free();
}

void action_on_wedge_product::null()
{
	M = NULL;
	F = NULL;
	wedge_v1 = NULL;
	wedge_v2 = NULL;
	wedge_v3 = NULL;
	low_level_point_size = 0;
}

void action_on_wedge_product::free()
{
	if (wedge_v1) {
		FREE_INT(wedge_v1);
		}
	if (wedge_v2) {
		FREE_INT(wedge_v2);
		}
	if (wedge_v3) {
		FREE_INT(wedge_v3);
		}
	null();
}

void action_on_wedge_product::init(action &A, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "action_on_wedge_product::init" << endl;
		cout << "starting with action " << A.label << endl;
		}
	if (A.type_G != matrix_group_t) {
		cout << "action_on_wedge_product::init fatal: A.type_G != matrix_group_t" << endl;
		exit(1);
		}
	M = A.G.matrix_grp;
	F = M->GFq;
	n = M->n;
	q = F->q;
	wedge_dimension = (n * (n - 1)) >> 1;
	//degree = i_power_j(q, wedge_dimension);
	degree = nb_PG_elements(wedge_dimension - 1, q);
	low_level_point_size = wedge_dimension;
	wedge_v1 = NEW_INT(wedge_dimension);
	wedge_v2 = NEW_INT(wedge_dimension);
	wedge_v3 = NEW_INT(wedge_dimension);
}

INT action_on_wedge_product::compute_image_INT(
	action &A, INT *Elt, INT a, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT b;
	
	if (f_v) {
		cout << "action_on_wedge_product::compute_image_INT" << endl;
		}
	//AG_element_unrank(q, wedge_v1, 1, wedge_dimension, a);
	PG_element_unrank_modified(*F, wedge_v1, 1, wedge_dimension, a);
	if (f_vv) {
		cout << "action_on_wedge_product::compute_image_INT a = " << a << " wedge_v1 = ";
		INT_vec_print(cout, wedge_v1, wedge_dimension);
		cout << endl;
		}
	
	compute_image_INT_low_level(A, Elt, wedge_v1, wedge_v2, verbose_level);
	if (f_vv) {
		cout << " v2=v1 * A=";
		INT_vec_print(cout, wedge_v2, wedge_dimension);
		cout << endl;
		}

	//AG_element_rank(q, wedge_v2, 1, wedge_dimension, b);
	PG_element_rank_modified(*F, wedge_v2, 1, wedge_dimension, b);
	if (f_v) {
		cout << "action_on_wedge_product::compute_image_INT done " << a << "->" << b << endl;
		}
	return b;
}

INT action_on_wedge_product::element_entry_frobenius(action &A, INT *Elt, INT verbose_level)
{
	INT f;

	f = A.element_linear_entry_frobenius(Elt, verbose_level);
	return f;
}

INT action_on_wedge_product::element_entry_ij(action &A, INT *Elt, INT I, INT J, INT verbose_level)
{
	INT i, j, k, l, w;

	k2ij(I, i, j, n);
	k2ij(J, k, l, n);
	w = element_entry_ijkl(A, Elt, i, j, k, l, verbose_level);
	return w;
}

INT action_on_wedge_product::element_entry_ijkl(action &A, INT *Elt, INT i, INT j, INT k, INT l, INT verbose_level)
{
	INT aki, alj, akj, ali, u, v, w;

	aki = A.element_linear_entry_ij(Elt, k, i, verbose_level); //Elt[k * n + i];
	alj = A.element_linear_entry_ij(Elt, l, j, verbose_level); //Elt[l * n + j];
	akj = A.element_linear_entry_ij(Elt, k, j, verbose_level); //Elt[k * n + j];
	ali = A.element_linear_entry_ij(Elt, l, i, verbose_level); //Elt[l * n + i];
	u = F->mult(aki, alj);
	v = F->mult(akj, ali);
	w = F->add(u, F->negate(v));
	return w;
}

void action_on_wedge_product::compute_image_INT_low_level(
	action &A, INT *Elt, INT *input, INT *output, INT verbose_level)
// x_{i,j} e_i \wedge e_j * A = 
// \sum_{k < l} \sum_{i < j} x_{i,j} (a_{i,k}a_{j,l} - a_{i,l}a_{j,k}) e_k \wedge e_l
// or (after a change of indices)
// \sum_{i<j} x_{i,j} e_i \wedge e_j * A = 
//   \sum_{i < j} \sum_{k < l} x_{k,l} (a_{k,i}a_{l,j} - a_{k,j}a_{l,i}) e_i \wedge e_j
//
// so, the image of e_i \wedge e_j is 
// \sum_{k < l} x_{k,l} (a_{k,i}a_{l,j} - a_{k,j}a_{l,i}),
// which are the entries in the row indexed by (i,j).
{
	INT *x = input;
	INT *xA = output;
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT i, j, ij, k, l, kl, c, w, z, xkl;
	
	if (f_v) {
		cout << "action_on_wedge_product::compute_image_INT_low_level" << endl;
		}
	if (f_vv) {
		cout << "wedge action: x=";
		INT_vec_print(cout, x, wedge_dimension);
		cout << endl;
		}
	// (i,j) = row index
	for (i = 0; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			ij = ij2k(i, j, n);
			c = 0;

			// (k,l) = column index
			for (k = 0; k < n; k++) {
				for (l = k + 1; l < n; l++) {
					kl = ij2k(k, l, n);
					xkl = x[kl];


					// a_{k,i}a_{l,j} - a_{k,j}a_{l,i} = matrix entry
#if 0

					aki = Elt[k * n + i];
					alj = Elt[l * n + j];
					akj = Elt[k * n + j];
					ali = Elt[l * n + i];
					u = F->mult(aki, alj);
					v = F->mult(akj, ali);
					w = F->add(u, F->negate(v));
#endif
	
					w = element_entry_ijkl(A, Elt, i, j, k, l, verbose_level - 3);
					// now w is the matrix entry
					
					z = F->mult(xkl, w);
					c = F->add(c, z);
					} // next l
				} // next k
			xA[ij] = c;
			} // next j
		} // next i
	if (f_vv) {
		cout << "xA=";
		INT_vec_print(cout, xA, wedge_dimension);
		cout << endl;
		}
	if (M->f_semilinear) {
		INT f = A.linear_entry_frobenius(Elt); //Elt[n * n];
		for (i = 0; i < wedge_dimension; i++) {
			xA[i] = F->frobenius_power(xA[i], f);
			}
		if (f_vv) {
			cout << "after " << f << " field automorphisms: xA=";
			INT_vec_print(cout, xA, wedge_dimension);
			cout << endl;
			}
		}
	if (f_v) {
		cout << "action_on_wedge_product::compute_image_INT_low_level done" << endl;
		}
}


