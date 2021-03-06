// w3q.C
// 
// Anton Betten
//
// started: March 4, 2011
// 
//
//

#include "orbiter.h"


W3q::W3q()
{
	null();
}

W3q::~W3q()
{
	freeself();
}

void W3q::null()
{
	q = 0;
	nb_lines = 0;
	P3 = NULL;
	Q4 = NULL;
	F = NULL;
	Basis = NULL;
	Lines = NULL;
	Q4_rk = NULL;
	Line_idx = NULL;
}

void W3q::freeself()
{
	if (P3) {
		delete P3;
		}
	if (Q4) {
		delete Q4;
		}
	if (Basis) {
		FREE_INT(Basis);
		}
	if (Lines) {
		FREE_INT(Lines);
		}
	if (Q4_rk) {
		FREE_INT(Q4_rk);
		}
	if (Line_idx) {
		FREE_INT(Line_idx);
		}
	null();
}

void W3q::init(finite_field *F, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT f_vvv = (verbose_level >= 3);
	INT h, c, rk;

	W3q::F = F;
	W3q::q = F->q;

	if (f_v) {
		cout << "W3q::init" << endl;
		}
	P3 = new projective_space;
	Q4 = new orthogonal;
	Basis = NEW_INT(2 * 4);
	
	P3->init(3, F, 
		//TRUE /* f_init_group */, 
		//FALSE /* f_line_action */, 
		FALSE /* f_init_incidence_structure */, 
		//TRUE /* f_semilinear */, 
		//TRUE /* f_basis */,
		verbose_level - 1  /*MINIMUM(verbose_level - 1, 3)*/);
	F = P3->F;
	Q4->init(0, 5, F, verbose_level - 1);

	Lines = NEW_INT(P3->N_lines);
	nb_lines = 0;
	for (h = 0; h < P3->N_lines; h++) {
		P3->unrank_line(Basis, h);
		c = evaluate_symplectic_form(Basis, Basis + 4);
		if (c) {
			continue;
			}
		Lines[nb_lines++] = h;
		}
	cout << "We found " << nb_lines << " Lines, they are" << endl;
	INT_vec_print(cout, Lines, nb_lines);
	cout << endl;

	if (nb_lines != Q4->nb_points) {
		cout << "nb_lines != Q4->nb_points" << endl;
		exit(1);
		}
	Q4_rk = NEW_INT(nb_lines);
	Line_idx = NEW_INT(nb_lines);


	for (h = 0; h < nb_lines; h++) {
		P3->unrank_line(Basis, Lines[h]);
		if (f_vv) {
			cout << "Line " << h << " is " << Lines[h] << ":" << endl;
			print_integer_matrix_width(cout, Basis, 2, 4, 4, F->log10_of_q);
			cout << endl;
			}

		isomorphism_Q4q(Basis, Basis + 4, v5);

		if (f_vvv) {
			cout << "v5=";
			INT_vec_print(cout, v5, 5);
			cout << endl;
			}
		
		rk = Q4->rank_point(v5, 1, 0);

		if (f_vvv) {
			cout << "orthogonal point rank " << rk << endl;
			}
		
		Q4_rk[h] = rk;
		Line_idx[rk] = h;
		}
	

	if (f_v) {
		cout << "The isomorphism is:" << endl;
		cout << "h : Lines[h] : Q4_rk[h] : Line_idx[h] : x : y : point in Q(4,q)" << endl;
		cout << "Where x and y are a basis for the line" << endl;
		for (h = 0; h < nb_lines; h++) {
			cout << setw(4) << h << " : ";
			cout << setw(4) << Lines[h] << " : ";
			cout << setw(4) << Q4_rk[h] << " : ";
			cout << setw(4) << Line_idx[h] << " : ";
			P3->unrank_line(Basis, Lines[h]);
			INT_vec_print(cout, Basis, 4);
			cout << " : ";
			INT_vec_print(cout, Basis + 4, 4);
			Q4->unrank_point(v5, 1, Q4_rk[h], 0);
			cout << " : ";
			INT_vec_print(cout, v5, 5);
			cout << endl;
			}
		}
}

INT W3q::evaluate_symplectic_form(INT *x4, INT *y4)
{
	return F->evaluate_symplectic_form(4, x4, y4);

	/*F->add4(
			F->mult(x4[0], y4[1]), 
			F->negate(F->mult(x4[1], y4[0])), 
			F->mult(x4[2], y4[3]), 
			F->negate(F->mult(x4[3], y4[2]))
		);*/
}

void W3q::isomorphism_Q4q(INT *x4, INT *y4, INT *v)
{
	v[0] = F->Pluecker_12(x4, y4);
	v[1] = F->negate(F->Pluecker_13(x4, y4));
	v[2] = F->Pluecker_42(x4, y4);
	v[3] = F->negate(F->Pluecker_14(x4, y4));
	v[4] = F->Pluecker_23(x4, y4);
}


