// representatives.C
// 
// Anton Betten
// started July 3, 2012
//
// 
//
//

#include "orbiter.h"


representatives::representatives()
{
	null();
}

void representatives::null()
{
	count = 0;
	rep = NULL;
	stab = NULL;
	fusion = NULL;
	handle = NULL;
	elt = NULL;
	Elt1 = NULL;
	tl = NULL;
}

representatives::~representatives()
{
	free();
	null();
}

void representatives::free()
{
	INT i;
	INT f_v = TRUE;

	if (f_v) {
		cout << "representatives::free" << endl;
		}
	if (rep) {
		FREE_INT(rep);
		rep = NULL;
		}
	if (stab) {
		for (i = 0; i < count; i++) {
			if (stab[i]) {
				delete stab[i];
				stab[i] = NULL;
				}
			}
		delete [] stab;
		stab = NULL;
		}
	if (fusion) {
		FREE_INT(fusion);
		fusion = NULL;
		}
	if (handle) {
		FREE_INT(handle);
		handle = NULL;
		}
	if (elt) {
		FREE_BYTE(elt);
		elt = NULL;
		}
	if (Elt1) {
		FREE_INT(Elt1);
		Elt1 = NULL;
		}
	if (tl) {
		FREE_INT(tl);
		tl = NULL;
		}
}

void representatives::init(action *A, INT nb_objects, BYTE *prefix, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;

	if (f_v) {
		cout << "representatives::init prefix=" << prefix << endl;
		}
	representatives::A = A;
	representatives::nb_objects = nb_objects;
	strcpy(representatives::prefix, prefix);
	
	
	rep = NEW_INT(nb_objects);
	stab = new psims[nb_objects];
	fusion = NEW_INT(nb_objects);
	handle = NEW_INT(nb_objects);
	elt = NEW_BYTE(A->coded_elt_size_in_char);
	Elt1 = NEW_INT(A->elt_size_in_INT);
	tl = NEW_INT(A->base_len);

	count = 0;
	for (i = 0; i < nb_objects; i++) {
		stab[i] = NULL;
		fusion[i] = -2;
		handle[i] = -1;
		}
	sprintf(fname_rep, "%sclassification_reps.txt", prefix);
	sprintf(fname_stabgens, "%sclassification_stabgens.bin", prefix);
	sprintf(fname_fusion, "%sclassification_fusion.txt", prefix);
	sprintf(fname_fusion_ge, "%sclassification_fusion_ge.bin", prefix);
}

void representatives::write_fusion(INT verbose_level)
// Writes fusion[] and handle[]
// If the object is a chosen representative for an isomorphism type 
// (i.e., if fusion[i] == i) then the identity element is written.
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "representatives::write_fusion" << endl;
		}
	{
	ofstream f1(fname_fusion);
	FILE *f2;
	INT i;
	
	f2 = fopen(fname_fusion_ge, "wb");
	
	for (i = 0; i < nb_objects; i++) {
		if (fusion[i] == -2) {
			cout << "representatives::write_fusion fusion[" << i << "] = -2" << endl;
			exit(1);
			}
		f1 << setw(5) << i << " " << setw(3) << fusion[i] << endl;
		if (fusion[i] == i) {
			//cout << "orbit " << i << " is representative" << endl;
			A->one(Elt1);
			}
		else {
			A->element_retrieve(handle[i], Elt1, FALSE);
			}
		A->element_write_file_fp(Elt1, elt, f2, 0/* verbose_level*/);
		}
	f1 << -1 << endl;
	fclose(f2);
	}
	if (f_v) {
		cout << "representatives::write_fusion finished" << endl;
		cout << "written file " << fname_fusion << " of size " << file_size(fname_fusion) << endl;
		cout << "written file " << fname_fusion_ge << " of size " << file_size(fname_fusion_ge) << endl;
		}
	
}

void representatives::read_fusion(INT verbose_level)
// Reads fusion[] and handle[]
{
	INT f_v = (verbose_level >= 1);
	INT a, b, i;

	if (f_v) {
		cout << "representatives::read_fusion nb_objects=" << nb_objects << endl;
		}
	if (f_v) {
		cout << "representatives::read_fusion reading file " << fname_fusion << " of size " << file_size(fname_fusion) << endl;
		}
	{
		ifstream f1(fname_fusion);
		for (i = 0; i < nb_objects; i++) {
			f1 >> a >> b;
			if (a != i) {
				cout << "representatives::read_fusion a != i" << endl;
				exit(1);
				}
			fusion[i] = b;
			}
		f1 >> a;
		if (a != -1) {
			cout << "representatives::read_fusion problem with end of file marker" << endl;
			exit(1);
			}
	}
	if (f_v) {
		cout << "representatives::read_fusion reading file " << fname_fusion_ge << " of size " << file_size(fname_fusion_ge) << endl;
		}
	{
		FILE *f2;
	
		f2 = fopen(fname_fusion_ge, "rb");
	
		for (i = 0; i < nb_objects; i++) {
			A->element_read_file_fp(Elt1, elt, f2, 0/* verbose_level*/);
			handle[i] = A->element_store(Elt1, FALSE);
			}
	}
	if (f_v) {
		cout << "representatives::read_fusion done" << endl;
		}
}

void representatives::write_representatives_and_stabilizers(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "representatives::write_representatives_and_stabilizers" << endl;
		}
	{
	ofstream f1(fname_rep);
	FILE *f2;
	INT i, j, cnt = 0;

	f2 = fopen(fname_stabgens, "wb");
	
	
	f1 << count << " " << setw(3) << A->base_len << " ";
	for (i = 0; i < A->base_len; i++) {
		f1 << setw(3) << A->base[i] << " ";
		}
	f1 << endl;
	
	for (i = 0; i < count; i++) {
		sims *Stab;
		longinteger_object go;
		vector_ge SG;
		
		Stab = stab[i];
		Stab->group_order(go);

		Stab->extract_strong_generators_in_order(SG, tl, 0 /* verbose_level */);

		f1 << setw(3) << i << " " 
			<< setw(7) << rep[i] << " " 
			<< setw(5) << cnt << " " 
			<< setw(5) << SG.len << " ";
		go.print_width(f1, 10);
		f1 << " ";
		for (j = 0; j < A->base_len; j++) {
			f1 << setw(3) << tl[j] << " ";
			}
		f1 << endl;
		
		
		for (j = 0; j < SG.len; j++) {
			A->element_write_file_fp(SG.ith(j), elt, f2, 0/* verbose_level*/);
			cnt++;
			}
		}
	f1 << -1 << endl;
	fclose(f2);
	}
	if (f_v) {
		cout << "representatives::write_representatives_and_stabilizers finished" << endl;
		cout << "written file " << fname_rep << " of size " << file_size(fname_rep) << endl;
		cout << "written file " << fname_stabgens << " of size " << file_size(fname_stabgens) << endl;
		}
	
}

void representatives::read_representatives_and_stabilizers(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = FALSE;//(verbose_level >=2);
	
	if (f_v) {
		cout << "representatives::read_representatives_and_stabilizers" << endl;
		cout << "reading files " << fname_rep << " and " << fname_stabgens << endl;
		}
	{
	ifstream f1(fname_rep);
	FILE *f2;
	INT i, j, first, len, a, b, c, d, e;
	
	f2 = fopen(fname_stabgens, "rb");
	
	f1 >> count >> a;
	if (a != A->base_len) {
		cout << "representatives::read_representatives_and_stabilizers base_len does not match" << endl;
		exit(1);
		}
	for (j = 0; j < A->base_len; j++) {
		f1 >> a;
		if (a != A->base[j]) {
			cout << "representatives::read_representatives_and_stabilizers base point does not match" << endl;
			exit(1);
			}
		}
	for (i = 0; i < count; i++) {
		sims *Stab;
		longinteger_object go;
		vector_ge gens;
		
		stab[i] = new sims;
		Stab = stab[i];
		f1 >> a >> b >> c >> d >> e;
		if (a != i) {
			cout << "representatives::read_representatives_and_stabilizers a != i" << endl;
			exit(1);
			}
		rep[i] = b;
		first = c;
		len = d;
		gens.init(A);
		gens.allocate(len);
		for (j = 0; j < A->base_len; j++) {
			f1 >> tl[j];
			}
		for (j = 0; j < len; j++) {
			A->element_read_file_fp(gens.ith(j), elt, f2, 0/* verbose_level*/);
			}
		if (f_vv) {
			cout << "representative of orbit " << i << " read" << endl;
			cout << "stabilizer is generated by" << endl;
			for (j = 0; j < len; j++) {
				cout << "generator " << j << ":" << endl;
				A->print(cout, gens.ith(j));
				cout << endl;
				}
			cout << "transversal lengths:" << endl;
			INT_vec_print(cout, tl, A->base_len);
			cout << endl;
			}
		Stab->init(A);
		Stab->init_generators(gens, FALSE);
		Stab->compute_base_orbits(0/*verbose_level - 5*/);
		Stab->group_order(go);
		if (f_v) {
			cout << "representatives::read_representatives_and_stabilizers stabilizer " << i << " has order " << go << endl;
			}
		}
	f1 >> a;
	if (a != -1) {
		cout << "representatives::read_representatives_and_stabilizers problems reading end of file marker" << endl;
		exit(1);
		}
	fclose(f2);
	}
	if (f_v) {
		cout << "representatives::read_representatives_and_stabilizers finished" << endl;
		}
}

void representatives::save(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "representatives::save" << endl;
		}
	write_fusion(verbose_level - 1);
	write_representatives_and_stabilizers(verbose_level - 1);
	if (f_v) {
		cout << "representatives::save done" << endl;
		}
}

void representatives::load(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "representatives::load" << endl;
		}
	read_fusion(verbose_level - 1);
	read_representatives_and_stabilizers(verbose_level - 1);
	if (f_v) {
		cout << "representatives::load done found " << count << " orbit representatives" << endl;
		}
}

void representatives::calc_fusion_statistics()
{
	INT i;
	
	nb_open = 0;
	nb_reps = 0;
	nb_fused = 0;
	for (i = 0; i < nb_objects; i++) {
		if (fusion[i] == -2) {
			nb_open++;
			}
		if (fusion[i] == i) {
			nb_reps++;
			}
		else if (fusion[i] >= 0) {
			nb_fused++;
			}
		}
	
}

void representatives::print_fusion_statistics()
{
	cout << "nb_reps = " << nb_reps << endl;
	cout << "nb_fused = " << nb_fused << endl;
	cout << "nb_open = " << nb_open << endl;
}




