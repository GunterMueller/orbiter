// klein_correspondence.C
//
// Anton Betten
// 
// January 1, 2016

#include "galois.h"

klein_correspondence::klein_correspondence()
{
	null();
}

klein_correspondence::~klein_correspondence()
{
	freeself();
}

void klein_correspondence::null()
{
	P3 = NULL;
	P5 = NULL;
	Gr63 = NULL;
	Gr62 = NULL;
	Form = NULL;
	Line_to_point_on_quadric = NULL;
	Point_on_quadric_to_line = NULL;
	Point_on_quadric_embedded_in_P5 = NULL;
	coordinates_of_quadric_points = NULL;
	Pt_rk = NULL;
}

void klein_correspondence::freeself()
{
	if (P3) {
		delete P3;
		}
	if (P5) {
		delete P5;
		}
	if (Gr63) {
		delete Gr63;
		}
	if (Gr62) {
		delete Gr62;
		}
	if (Form) {
		FREE_INT(Form);
		}
	if (Line_to_point_on_quadric) {
		FREE_INT(Line_to_point_on_quadric);
		}
	if (Point_on_quadric_to_line) {
		FREE_INT(Point_on_quadric_to_line);
		}
	if (Point_on_quadric_embedded_in_P5) {
		FREE_INT(Point_on_quadric_embedded_in_P5);
		}
	if (coordinates_of_quadric_points) {
		FREE_INT(coordinates_of_quadric_points);
		}
	if (Pt_rk) {
		FREE_INT(Pt_rk);
		}
}

void klein_correspondence::init(finite_field *F, orthogonal *O, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT d = 6;
	INT i, u, v;

	if (f_v) {
		cout << "klein_correspondence::init" << endl;
		}
	
	klein_correspondence::F = F;
	klein_correspondence::O = O;
	q = F->q;


	nb_Pts = O->nb_points;
	
	P3 = new projective_space;
	
	P3->init(3, F, 
		TRUE /* f_init_incidence_structure */, 
		0 /* verbose_level - 2 */);

	P5 = new projective_space;
	
	P5->init(5, F, 
		TRUE /* f_init_incidence_structure */, 
		0 /* verbose_level - 2 */);

	
	Gr63 = new grassmann;
	Gr62 = new grassmann;

	Gr63->init(6, 3, F, 0 /* verbose_level */);
	Gr62->init(6, 2, F, 0 /* verbose_level */);


	Form = NEW_INT(d * d);
	INT_vec_zero(Form, d * d);
	// the matrix with blocks
	// [0 1]
	// [1 0]
	// along the diagonal:
	for (i = 0; i < 3; i++) {
		u = 2 * i + 0;
		v = 2 * i + 1;
		Form[u * d + v] = 1;
		Form[v * d + u] = 1;
		}
	if (f_v) {
		cout << "klein_correspondence::init Form matrix:" << endl;
		INT_matrix_print(Form, d, d);
		}
	Line_to_point_on_quadric = NEW_INT(P3->N_lines);
	Point_on_quadric_to_line = NEW_INT(P3->N_lines);
	Point_on_quadric_embedded_in_P5 = NEW_INT(P3->N_lines);

	INT basis_line[8]; // [2 * 4]
	INT v6[6];
	INT *x4, *y4, a, b, c, val, j;

	//basis_line = NEW_INT(8);
	for (i = 0; i < P3->N_lines; i++) {
		Point_on_quadric_to_line[i] = -1;
		}
	for (i = 0; i < P3->N_lines; i++) {
		P3->unrank_line(basis_line, i);
		x4 = basis_line;
		y4 = basis_line + 4;
		v6[0] = F->Pluecker_12(x4, y4);
		v6[1] = F->Pluecker_34(x4, y4);
		v6[2] = F->Pluecker_13(x4, y4);
		v6[3] = F->Pluecker_42(x4, y4);
		v6[4] = F->Pluecker_14(x4, y4);
		v6[5] = F->Pluecker_23(x4, y4);
		a = F->mult(v6[0], v6[1]);
		b = F->mult(v6[2], v6[3]);
		c = F->mult(v6[4], v6[5]);
		val = F->add3(a, b, c);
		//cout << "a=" << a << " b=" << b << " c=" << c << endl;
		//cout << "val=" << val << endl;
		if (val) {
			cout << "klein_correspondence::init point does not lie on quadric" << endl;
			exit(1);
			}
		//j = P5->rank_point(v6);
		j = O->rank_point(v6, 1, 0 /* verbose_level */);
		if (FALSE) {
			cout << "klein_correspondence::init i=" << i << " / " << P3->N_lines << " v6 : ";
			INT_vec_print(cout, v6, 6);
			cout << " : j=" << j << endl;
			}
		Line_to_point_on_quadric[i] = j;
		Point_on_quadric_to_line[j] = i;
		}
	for (i = 0; i < P3->N_lines; i++) {
		if (Point_on_quadric_to_line[i] == -1) {
			cout << "Something is wrong with Point_on_quadric_to_line" << endl;
			exit(1);
			}
		}
	for (i = 0; i < P3->N_lines; i++) {
		O->unrank_point(v6, 1, i, 0);
		Point_on_quadric_embedded_in_P5[i] = P5->rank_point(v6);
		}

	coordinates_of_quadric_points = NEW_INT(P3->N_lines * d);
	Pt_rk = NEW_INT(P3->N_lines);

	for (i = 0; i < P3->N_lines; i++) {
		O->unrank_point(coordinates_of_quadric_points + i * d, 1, i, 0);
		INT_vec_copy(coordinates_of_quadric_points + i * d, v6, 6);
		PG_element_rank_modified(*F, v6, 1, d, a);
		Pt_rk[i] = a;
		}

	if (f_vv) {
		cout << "Points on the Klein quadric:" << endl;
		if (nb_Pts < 50) {
			for (i = 0; i < nb_Pts; i++) {
				cout << i << " : ";
				INT_vec_print(cout, coordinates_of_quadric_points + i * d, d);
				cout << " : " << Pt_rk[i] << endl;
				}
			}
		else {
			cout << "too many points to print" << endl;
			}
		}

	nb_pts_PG = nb_PG_elements(d - 1, q);
	if (f_v) {
		cout << "klein_correspondence::init  nb_pts_PG = " << nb_pts_PG << endl;
		}
	Pt_idx = NEW_INT(nb_pts_PG);
	for (i = 0; i < nb_pts_PG; i++) {
		Pt_idx[i] = -1;
		}
	for (i = 0; i < nb_Pts; i++) {
		a = Pt_rk[i];
		Pt_idx[a] = i;
		}


	if (f_v) {
		cout << "klein_correspondence::init done" << endl;
		}
}

void klein_correspondence::plane_intersections(INT *lines_in_PG3, INT nb_lines, 
	longinteger_object *&R,
	INT **&Pts_on_plane, 
	INT *&nb_pts_on_plane, 
	INT &nb_planes, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT *pts;
	INT i;

	if (f_v) {
		cout << "klein_correspondence::plane_intersections" << endl;
		}
	pts = NEW_INT(nb_lines);
	
	P3->klein_correspondence(P5, 
		lines_in_PG3, nb_lines, pts, 0/*verbose_level*/);

	P5->plane_intersection_type_fast(Gr63, pts, nb_lines, 
		R, Pts_on_plane, nb_pts_on_plane, nb_planes, 
		verbose_level - 3);


	if (f_vv) {
		cout << "klein_correspondence::plane_intersections: We found " << nb_planes << " planes." << endl;
#if 1
		for (i = 0; i < nb_planes; i++) {
			cout << setw(3) << i << " : " << R[i] 
				<< " : " << setw(5) << nb_pts_on_plane[i] << " : ";
			INT_vec_print(cout, Pts_on_plane[i], nb_pts_on_plane[i]);
			cout << endl; 
			}
#endif
		}
	
	FREE_INT(pts);
	if (f_v) {
		cout << "klein_correspondence::plane_intersections done" << endl;
		}
}

