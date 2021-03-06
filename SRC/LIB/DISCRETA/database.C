// database.C
//
// Anton Betten
// 27.11.2000
// moved from D2 to ORBI Nov 15, 2007

#include "orbiter.h"

#include <stdlib.h> // for system




database::database() : Vector()
{
	k = DATABASE;
}

database::database(const base &x)
	// copy constructor:    this := x
{
	cout << "database::copy constructor for object: " << const_cast<base &>(x) << "\n";
	const_cast<base &>(x).copyobject_to(*this);
}

database& database::operator = (const base &x)
	// copy assignment
{
	cout << "database::operator = (copy assignment)" << endl;
	copyobject(const_cast<base &>(x));
	return *this;
}

void database::settype_database()
{
	OBJECTSELF s;
	
	s = self;
	new(this) database;
	self = s;
	k = DATABASE;
}

database::~database()
{
	freeself_database();
}

void database::freeself_database()
{
	// cout << "group_selection::freeself_database()\n";
	freeself_vector();
}

kind database::s_virtual_kind()
{
	return DATABASE;
}

void database::copyobject_to(base &x)
{
#ifdef COPY_VERBOSE
	cout << "database::copyobject_to()\n";
	print_as_vector(cout);
#endif
	Vector::copyobject_to(x);
	x.as_database().settype_database();
#ifdef COPY_VERBOSE
	x.as_database().print_as_vector(cout);
#endif
}

ostream& database::print(ostream& ost)
{
	
	return ost;
}

void database::init(const BYTE *filename, INT objectkind, INT f_compress)
{
	init_with_file_type(filename, objectkind, f_compress, DB_FILE_TYPE_STANDARD);
}

void database::init_with_file_type(const BYTE *filename, 
	INT objectkind, INT f_compress, INT file_type)
{
	m_l(8);
	c_kind(DATABASE);
	s_i(0).change_to_vector();
	s_i(1).change_to_hollerith();
	btree_access().m_l(0);
	database::filename().init(filename);
	database::objectkind() = objectkind;
	database::f_compress() = f_compress;
	f_open() = FALSE;
	database::file_type() = file_type;
	file_size() = 0;
}

void database::create(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;
	BYTE *buf;
	
	if (f_v) {
		cout << "database::create" << endl;
		}
	buf = NEW_BYTE(size_of_header());
	file_create(0 /*verbose_level - 1*/);
	file_size() = size_of_header();
	for (i = 0; i < size_of_header(); i++)
		buf[i] = 0;
	file_seek(0);
	file_write(buf, size_of_header(), 1);
	put_file_size();
	for (i = 0; i < btree_access().s_l(); i++) {
		btree_access_i(i).create(verbose_level - 1);
		}
	FREE_BYTE(buf);
}

void database::open(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;
	
	if (f_v) {
		cout << "database::open, verbose_level=" << verbose_level << endl;
		}
	file_open(0 /*verbose_level - 1*/);
	for (i = 0; i < btree_access().s_l(); i++) {
		btree_access_i(i).open(verbose_level - 1);
		}
}

void database::close(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;
	
	if (f_v) {
		cout << "database::close" << endl;
		}
	put_file_size();
	file_close(0 /*verbose_level - 1*/);
	for (i = 0; i < btree_access().s_l(); i++) {
		btree_access_i(i).close(verbose_level);
		}
}

void database::delete_files()
{
	hollerith cmd;
	INT i;
	
	cmd.init("rm ");
	cmd.append(filename().s());
	system(cmd.s());
	for (i = 0; i < btree_access().s_l(); i++) {
		cmd.init("rm ");
		cmd.append(btree_access_i(i).filename().s());
		system(cmd.s());
		}
}

void database::put_file_size()
{
	INT4 l, l1;
	INT f_v = FALSE;

	if (f_v) {
		cout << "database::put_file_size" << endl;
		}
	file_seek(DB_POS_FILESIZE);
	l = l1 = file_size();
	block_swap_bytes((SCHAR *)&l, sizeof(INT4), 1);
	file_write(&l, 4, 1);
}

void database::get_file_size()
{
	INT4 l;
	INT f_v = FALSE;

	if (f_v) {
		cout << "database::get_file_size" << endl;
		}
	file_seek(DB_POS_FILESIZE);
	file_read(&l, 4, 1);
	block_swap_bytes((SCHAR *)&l, sizeof(INT4), 1);
	file_size() = l;
}

void database::user2total(INT user, INT *total, INT *pad)
{
	INT r, r1, sz;
	
	sz = size_of_header();
	if (file_type() == DB_FILE_TYPE_STANDARD) {
		r = user % sz;
		if (r != 0) {
			r1 = sz - r;
			}
		else {
			r1 = 0;
			}
		*pad = r1 + sz;
		*total = sz + sz + user + *pad;
		}
	else if (file_type() == DB_FILE_TYPE_COMPACT) {
		r = user % sz;
		*pad = sz - r;
		*total = sizeof(INT4) * 2 + user + *pad;
		}
}

INT database::size_of_header()
{
	if (file_type() == DB_FILE_TYPE_STANDARD) {
		return 16;
		}
	else if (file_type() == DB_FILE_TYPE_COMPACT) {
		return 8;
		}
	else {
		cout << "database::size_of_header() unknown file_type" << endl;
		cout << "file_type()=" << file_type() << endl;
		exit(1);
		}
}

INT database::size_of_header_log()
{
	if (file_type() == DB_FILE_TYPE_STANDARD) {
		return 4;
		}
	else if (file_type() == DB_FILE_TYPE_COMPACT) {
		return 3;
		}
	else {
		cout << "database::size_of_header_log() unknown file_type" << endl;
		cout << "file_type()=" << file_type() << endl;
		exit(1);
		}
	
}

void database::add_object_return_datref(Vector &the_object, UINT4 &datref, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	KEYTYPE *key_type = NULL;
	DATATYPE data_type;

	if (f_v) {
		cout << "database::add_object_return_datref" << endl;
		}
	if (!f_open()) {
		cout << "database::add_object_return_datref() database not open" << endl;
		exit(1);
		}
	key_type = new KEYTYPE;
	if (the_object.s_kind() != objectkind()) {
		cout << "database::add_object_return_datref() wrong kind of object" << endl;
		exit(1);
		}
	
	memory M;

	if (FALSE) {
		cout << "database::add_object_return_datref(): packing object" << endl;
		}
	the_object.pack(M, FALSE, 0/*debug_depth*/);

	if (f_compress()) {
		if (FALSE) {
			cout << "database::add_object_return_datref(): compressing object" << endl;
			}
		M.compress(FALSE);
		}
	INT i, size;
	//UINT4 datref;
	BYTE *pc;
	size = M.used_length();
	pc = (BYTE *) M.self.char_pointer;
	if (FALSE) {
		cout << "database::add_object_return_datref(): saving data via add_data_DB()" << endl;
		}
	add_data_DB((void *)pc, size, &datref, verbose_level - 4);
	if (FALSE) {
		cout << "finished with add_data_DB()" << endl;
		}
	data_type.datref = datref;
	data_type.data_size = size;

	for (i = 0; i < btree_access().s_l(); i++) {
		btree & bt = btree_access_i(i);
		bt.key_fill_in(key_type->c, the_object);
		
		if (f_vv) {
			cout << "database::add_object_return_datref(): calling insert_key for btree #" << i << ": ";
			bt.key_print(key_type->c, cout);
			cout << endl;
			}
		bt.insert_key(key_type, &data_type, verbose_level - 2);
		if (f_vv) {
			cout << "database::add_object_return_datref(): after insert_key for btree #" << i << endl;
			}
		}

	delete key_type;
	if (f_v) {
		cout << "database::add_object_return_datref done" << endl;
		}
}

void database::add_object(Vector &the_object, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	UINT4 datref;

	if (f_v) {
		cout << "database::add_object" << endl;
		}
	add_object_return_datref(the_object, datref, verbose_level);
	if (f_v) {
		cout << "database::add_object done" << endl;
		}
}

void database::delete_object(Vector& the_object, 
	UINT4 datref, INT verbose_level)
{
	INT i, j, len;
	INT idx;
	KEYTYPE key_type;
	DATATYPE data_type;
	
	if (!f_open()) {
		cout << "database::delete_object() database not open" << endl;
		exit(1);
		}
	if (the_object.s_kind() != objectkind()) {
		cout << "database::delete_object() wrong kind of object" << endl;
		exit(1);
		}
	INT size = get_size_from_datref(datref, verbose_level - 1);
	data_type.datref = datref;
	data_type.data_size = size;
	len = btree_access().s_l();
	for (i = 0; i < len; i++) {
		for (j = 0; j < BTREEMAXKEYLEN; j++) {
			key_type.c[j] = 0;
			}
		btree & bt = btree_access_i(i);
		bt.key_fill_in(key_type.c, the_object);
		if (!bt.search(key_type.c, &data_type, &idx, 0, verbose_level)) {
			cout << "database::delete_object() WARNING: btree entry not found" << endl;
			continue;
			}
		bt.delete_ith(idx, verbose_level);
		}
	free_data_DB(datref, size, verbose_level - 2);
}

void database::get_object(UINT4 datref, 
	Vector &the_object, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT size;
	DATATYPE data_type;
	
	if (f_v) {
		cout << "database::get_object" << endl;
		}
	size = get_size_from_datref(datref, verbose_level - 1);
	data_type.datref = datref;
	data_type.data_size = size;
	get_object(&data_type, the_object, verbose_level - 1);
}

void database::get_object(DATATYPE *data_type, Vector &the_object, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT size, total, pad, i;
	
	if (f_v) {
		cout << "database::get_object" << endl;
		}
	if (!f_open()) {
		cout << "database::get_object(data_type) database not open" << endl;
		exit(1);
		}
	size = data_type->data_size;
	user2total(size, &total, &pad);
	memory M;
	
	M.alloc(size);
	BYTE *pc = M.self.char_pointer;
	BYTE *d = NULL;
	BYTE *pc1;
	
	d = new char[total];
	
	file_seek(((UINT)data_type->datref) << size_of_header_log());
	file_read(d, 1, total);
	
	if (file_type() == DB_FILE_TYPE_STANDARD) {
		INT4 *header = (INT4 *) d;
		INT4 *header2 = header + 4;
		
		pc1 = d + 8 * 4;
		block_swap_bytes((SCHAR *)header, sizeof(INT4), 4);
		block_swap_bytes((SCHAR *)header2, sizeof(INT4), 4);
		if (header[0] != MAGIC_SYNC) {
			cout << "database::get_object()|header: no MAGIC_SYNC" << endl;
			cout << "data_type->datref=" << data_type->datref << endl;
			exit(1);
			}
		if (!header[1]) {
			cout << "database::get_object()|header: data is not used" << endl;
			exit(1);
			}
		if (header[2] != size) {
			cout << "database::get_object()|header: header[2] != size" << endl;
			exit(1);
			}
		if (header[3] != total) {
			cout << "database::get_object()|header: header[3] != total" << endl;
			exit(1);
			}
		if (header2[0] != MAGIC_SYNC) {
			cout << "database::get_object()|header2: no MAGIC_SYNC" << endl;
			exit(1);
			}
		if (!header2[1]) {
			cout << "database::get_object()|header2: data is not used" << endl;
			exit(1);
			}
		if (header2[2] != size) {
			cout << "database::get_object()|header2: header[2] != size" << endl;
			exit(1);
			}
		if (header2[3] != total) {
			cout << "database::get_object()|header2: header[3] != total" << endl;
			exit(1);
			}
		}
	else if (file_type() == DB_FILE_TYPE_COMPACT) {
		INT4 *header = (INT4 *) d;
		
		pc1 = d + 4 * 2;
		block_swap_bytes((SCHAR *)header, sizeof(INT4), 2);
		if (header[0] != MAGIC_SYNC) {
			cout << "database::get_object()|header: no MAGIC_SYNC" << endl;
			cout << "data_type->datref=" << data_type->datref << endl;
			exit(1);
			}
		if (header[1] != size) {
			cout << "database::get_object()|header: header[1] != size" << endl;
			exit(1);
			}
		}
	else {
		cout << "database::get_object() unknown file_type" << endl;
		cout << "file_type()=" << file_type() << endl;
		exit(1);
		}
	for (i = 0; i < size; i++)
		pc[i] = pc1[i];
	// M.alloc_length();
	M.used_length() = size;
	if (f_compress()) {
		if (f_vv) {
			cout << "database::get_object(): decompressing object" << endl;
			}
		M.decompress(f_vv);
		}
	M.cur_pointer() = 0;
	
	the_object.freeself();
	the_object.unpack(M, FALSE, 0/*debug_depth*/);
	
	delete [] d;
}

void database::get_object_by_unique_INT4(INT btree_idx, INT id, 
	Vector& the_object, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	//INT f_vv = (verbose_level >= 2);
	btree& B = btree_access_i(btree_idx);
	INT datref;
	
	if (f_v) {
		cout << "database::get_object_by_unique_INT4 calling search_datref_of_unique_INT4" << endl;
		}
	datref = B.search_datref_of_unique_INT4(id, verbose_level - 1);
	if (f_v) {
		cout << "datref=" << datref << " calling get_object" << endl;
		}
	get_object(datref, the_object, verbose_level - 1);
}

INT database::get_object_by_unique_INT4_if_there(INT btree_idx, INT id, 
	Vector& the_object, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	//INT f_vv = (verbose_level >= 2);
	btree& B = btree_access_i(btree_idx);
	INT datref;
	
	if (f_v) {
		cout << "database::get_object_by_unique_INT4 calling search_datref_of_unique_INT4" << endl;
		}
	datref = B.search_datref_of_unique_INT4_if_there(id, verbose_level - 1);
	if (f_v) {
		cout << "datref=" << datref << endl;
		}
	if (datref == -1)
		return FALSE;
	if (f_v) {
		cout << "calling get_object" << endl;
		}
	get_object(datref, the_object, verbose_level - 1);
	return TRUE;
}

INT database::get_highest_INT4(INT btree_idx)
{
	btree & B = btree_access_i(btree_idx);
	
	return B.get_highest_INT4();
}

void database::ith_object(INT i, INT btree_idx, 
	Vector& the_object, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	KEYTYPE key_type;
	DATATYPE data_type;
	
	if (f_v) {
		cout << "database::ith_object i=" << i << endl;
		}
	ith(i, btree_idx, &key_type, &data_type, verbose_level);
	get_object(&data_type, the_object, verbose_level - 1);
}

void database::ith(INT i, INT btree_idx, 
	KEYTYPE *key_type, DATATYPE *data_type, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "database::ith i=" << i << endl;
		}
	btree & bt = btree_access_i(btree_idx);
	bt.ith(i, key_type, data_type, verbose_level);
}

void database::print_by_btree(INT btree_idx, ostream& ost)
{
	btree&B = btree_access_i(btree_idx);
	INT i, len;
	Vector the_object;
	INT verbose_level = 0;
	
	open(verbose_level);
	len = B.length(verbose_level);
	cout << "database " << filename().s() << ", btree " << B.filename().s() << " of length " << len << endl;
	for (i = 0; i < len; i++) {
		ith_object(i, btree_idx, the_object, verbose_level);
		cout << i << " : " << the_object << endl;
		}
	close(verbose_level);
}

void database::print_by_btree_with_datref(INT btree_idx, ostream& ost)
{
	btree &B = btree_access_i(btree_idx);
	INT i, len;
	Vector the_object;
	INT verbose_level = 0;
	KEYTYPE key_type;
	DATATYPE data_type;
	
	open(verbose_level);
	len = B.length(verbose_level);
	cout << "database " << filename().s() << ", btree " << B.filename().s() << " of length " << len << endl;
	for (i = 0; i < len; i++) {
		B.ith(i, &key_type, &data_type, verbose_level);
		get_object(&data_type, the_object, verbose_level - 1);
		cout << i << " : " 
			<< data_type.datref << " " 
			<< data_type.data_size << " " 
			<< the_object << endl;
		}
	close(verbose_level);
}

void database::print_subset(Vector& datrefs, ostream& ost)
{
	INT i, len;
	Vector the_object;
	INT verbose_level = 0;
	
	len = datrefs.s_l();
	for (i = 0; i < len; i++) {
		get_object(datrefs.s_ii(i), the_object, verbose_level);
		cout << i << " : " << the_object << endl;
		}
}

void database::extract_subset(Vector& datrefs, BYTE *out_path, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 1);
	INT i, len;
	Vector the_object;
	
	if (f_v) {
		cout << "database::extract_subset()" << endl;
		}
	database D;
	
	design_parameter p;

	p.init_database(D, out_path);
	D.create(verbose_level - 1);


	len = datrefs.s_l();
	if (f_v) {
		cout << "copying " << len << " datasets into database " << out_path << endl;
		}
	for (i = 0; i < len; i++) {
		if (f_v && !f_vv) {
			cout << i << " ";
			if ((i % 10) == 0)
				cout << endl;
			}
		get_object(datrefs.s_ii(i), the_object, verbose_level - 2);
		if (f_vv) {
			cout << i << " : " << the_object << endl;
			}
		D.add_object(the_object, verbose_level - 2);
		}
	D.close(verbose_level - 1);
}

void database::search_INT4(INT btree_idx, INT imin, INT imax, 
	Vector &datrefs, INT verbose_level)
{
	Vector Btree_idx, Imin, Imax;
	
	Btree_idx.m_l(1);
	Imin.m_l(1);
	Imax.m_l(1);
	Btree_idx.m_ii(0, btree_idx);
	Imin.m_ii(0, imin);
	Imax.m_ii(0, imax);
	search_INT4_multi_dimensional(Btree_idx, Imin, Imax, datrefs, verbose_level);
}

void database::search_INT4_2dimensional(INT btree_idx0, INT imin0, INT imax0, 
	INT btree_idx1, INT imin1, INT imax1, 
	Vector &datrefs, INT verbose_level)
{
	Vector Btree_idx, Imin, Imax;
	
	Btree_idx.m_l(2);
	Imin.m_l(2);
	Imax.m_l(2);
	Btree_idx.m_ii(0, btree_idx0);
	Imin.m_ii(0, imin0);
	Imax.m_ii(0, imax0);
	Btree_idx.m_ii(1, btree_idx1);
	Imin.m_ii(1, imin1);
	Imax.m_ii(1, imax1);
	search_INT4_multi_dimensional(Btree_idx, Imin, Imax, datrefs, verbose_level);
}

void database::search_INT4_multi_dimensional(Vector& btree_idx, 
	Vector& i_min, Vector &i_max, Vector& datrefs, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT i, l, bi, imin, imax, first, len, db_length;
	Vector v1, v2, First, Len, Len_sorted;
	
	if (!f_open()) {
		cout << "database::search_INT4_multi_dimensional() database not open" << endl;
		exit(1);
		}
	datrefs.m_l(0);
	l = btree_idx.s_l();
	First.m_l_n(l);
	Len.m_l_n(l);
	for (i = 0; i < l; i++) {
		bi = btree_idx.s_ii(i);
		imin = i_min.s_ii(i);
		imax = i_max.s_ii(i);
		if (f_v) {
			cout << "database::search_INT4_multi_dimensional() i=" << i 
				<< " bi=" << bi << " imin=" << imin << " imax=" << imax << endl;
			}
		btree &B = btree_access_i(bi);
		B.search_interval_INT4(imin, imax, first, len, verbose_level - 1);
		if (f_v) {
			cout << "after search_interval_INT4() first = " << first << " len=" << len << endl;
			}
		if (len == 0)
			return;
		First.m_ii(i, first);
		Len.m_ii(i, len);
		}
	if (f_v) {
		cout << "First = " << First << endl;
		cout << "Len = " << Len << endl;
		}
	permutation p;
	
	Len_sorted = Len;
	Len_sorted.sort_with_logging(p);
	if (f_v) {
		cout << "Len (sorted) = " << Len_sorted << endl;
		}
	INT j, h, l2;
	j = p[0];
	bi = btree_idx.s_ii(j);
	btree &B = btree_access_i(bi);
	db_length = B.length(verbose_level);
	B.get_datrefs(First.s_ii(j), Len.s_ii(j), v1);
	v1.sort();
	if (f_v) {
		cout << "db_length = " << db_length << endl;
		cout << "datrefs after 0-th btree " << j << " : " << v1 << " (length=" << v1.s_l() << ")" << endl;
		}
	if (f_vv) {
		print_subset(v1, cout);
		}
	for (i = 1; i < l; i++) {
		KEYTYPE key;
		DATATYPE data;
		INT datref, idx;
		integer datref_object;
		
		v2.m_l_n(v1.s_l());
		l2 = 0;
		j = p[i];
		bi = btree_idx.s_ii(j);
		first = First.s_ii(j);
		len = Len.s_ii(j);
		if (len == db_length) {
			if (f_v) {
				cout << i << "th-btree selects all datasets, no restriction here." << endl;
				}
			continue;
			}
		btree &B = btree_access_i(bi);
		for (h = 0; h < len; h++) {
			B.ith(first + h, &key, &data, verbose_level - 1);
			datref = data.datref;
			datref_object.m_i(datref);
			if (v1.search(datref_object, &idx)) {
				v2.m_ii(l2++, datref);
				}
			}
		v2.realloc(l2);
		v2.sort();
		v1.swap(v2);
		if (f_v) {
			cout << "datrefs after " << i << "th-btree " << j << " : " << v1 << " (length=" << v1.s_l() << ")" << endl;
			}
		if (f_vv) {
			print_subset(v1, cout);
			}
		}
	v1.swap(datrefs);
	if (f_v) {
		print_subset(datrefs, cout);
		}
}


INT database::get_size_from_datref(UINT4 datref, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT size;
	INT4 *header = NULL;
	
	if (f_v) {
		cout << "database::get_size_from_datref" << endl;
		}
	if (!f_open()) {
		cout << "database::get_size_from_datref() database not open" << endl;
		exit(1);
		}
	if (file_type() == DB_FILE_TYPE_STANDARD) {
		header = new INT4[8];
		file_seek(((UINT)datref) << size_of_header_log());
		file_read((BYTE *)header, 1, 8 * 4);
		block_swap_bytes((SCHAR *)header, sizeof(INT4), 8);
		if (header[0] != MAGIC_SYNC) {
			cout << "database::get_size_from_datref()|header: no MAGIC_SYNC, probably the datref is wrong" << endl;
			cout << "datref=" << datref << endl;
			exit(1);
			}
		if (!header[1]) {
			cout << "database::get_size_from_datref()|header: data is not used" << endl;
			exit(1);
			}
		size = header[2];
		delete [] header;
		}
	else if (file_type() == DB_FILE_TYPE_COMPACT) {
		header = new INT4[2];
		file_seek(((UINT)datref) << size_of_header_log());
		file_read((BYTE *)header, 1, 4 * 2);
		block_swap_bytes((SCHAR *)header, sizeof(INT4), 2);
		if (header[0] != MAGIC_SYNC) {
			cout << "database::get_size_from_datref()|header: no MAGIC_SYNC, probably the datref is wrong" << endl;
			cout << "datref=" << datref << endl;
			exit(1);
			}
		size = header[1];
		delete [] header;
		}
	else {
		cout << "database::size_of_header() unknown file_type" << endl;
		cout << "file_type()=" << file_type() << endl;
		exit(1);
		}
	if (f_v) {
		cout << "database::get_size_from_datref size = " << size << endl;
		}
	
	return size;
}

void database::add_data_DB(void *d, 
	INT size, UINT4 *datref, INT verbose_level)
{
	if (file_type() == DB_FILE_TYPE_STANDARD) {
		add_data_DB_standard(d, size, datref, verbose_level);
		}
	else if (file_type() == DB_FILE_TYPE_COMPACT) {
		add_data_DB_compact(d, size, datref, verbose_level);
		}
}

void database::add_data_DB_standard(void *d, 
	INT size, UINT4 *datref, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT total, pad;
	BYTE *data2 = NULL;
	BYTE *pc, *pc0;
	INT i;
	INT4 *pi;
	INT4 header[4];
	INT4 new_header[4];
		/* 0: SYNC
		 * 1: f_used
		 * 2: length user data
		 * 3: total length (header inclusive), 
		 *    a multiple of 16, 
		 *    one unused full 16 byte block guaranteed.
		 */
	INT old_file_size;
	
	if (f_v) {
		cout << "database::add_data_DB()" << endl;
		}
	user2total(size, &total, &pad);
	data2 = (BYTE *) new BYTE[total];
	header[0] = MAGIC_SYNC;
	header[1] = TRUE;
	header[2] = size;
	header[3] = total;
	block_swap_bytes((SCHAR *)header, sizeof(INT4), 4);
	pi = (INT4 *)data2;
	pi[0] = header[0];
	pi[1] = header[1];
	pi[2] = header[2];
	pi[3] = header[3];
	pi[4] = header[0];
	pi[5] = header[1];
	pi[6] = header[2];
	pi[7] = header[3];
	block_swap_bytes((SCHAR *)header, sizeof(INT4), 4);
		// swap header back, there will be another test 
	pc = (BYTE *)(pi + 8);
	pc0 = (BYTE *)d;
	if (f_vv) {
		cout << "size = " << size << " pad = " << pad << " total = " << total << endl;
		}
	for (i = 0; i < size; i++)
		pc[i] = pc0[i];
	for (i = 0; i < pad; i++)
		pc[size + i] = 0;
	old_file_size = file_size();
	file_seek(old_file_size);
	file_write(data2, 1, total);
	*datref = (UINT4)(old_file_size >> size_of_header_log());
	if (((INT)((UINT)*datref << size_of_header_log())) != old_file_size) {
		cout << "database::add_data_DB ((UINT)*datref << size_of_header_log()) != old_file_size" << endl;
		cout << "old_file_size=" << old_file_size << endl;
		cout << "*datref=" << *datref << endl;
		cout << "size_of_header_log()=" << size_of_header_log() << endl;
		exit(1);
		}
	file_size() += total;
	//put_file_size();
	
	file_seek(old_file_size);
	file_read(new_header, 4, 4);
	block_swap_bytes((SCHAR *)new_header, sizeof(INT4), 4);
	if (header[0] != new_header[0]) {
		cout << "header[0] != new_header[0]\n";
		}
	if (header[1] != new_header[1]) {
		cout << "header[1] != new_header[1]\n";
		}
	if (header[2] != new_header[2]) {
		cout << "header[2] != new_header[2]\n";
		}
	if (header[3] != new_header[3]) {
		cout << "header[3] != new_header[3]\n";
		}
	delete [] data2;
}

void database::add_data_DB_compact(void *d, 
	INT size, UINT4 *datref, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT total, pad;
	BYTE *data2 = NULL;
	BYTE *pc, *pc0;
	INT i;
	INT4 *pi;
	INT4 header[2];
	INT4 new_header[2];
		// 0: SYNC
		// 1: size of user data are
	INT old_file_size;
	
	if (f_v) {
		cout << "database::add_data_DB_compact()" << endl;
		}
	user2total(size, &total, &pad);
	data2 = (BYTE *) new BYTE[total];
	header[0] = MAGIC_SYNC;
	header[1] = size;
	block_swap_bytes((SCHAR *)header, sizeof(INT4), 2);
	pi = (INT4 *)data2;
	pi[0] = header[0];
	pi[1] = header[1];
	block_swap_bytes((SCHAR *)header, sizeof(INT4), 2);
		// swap header back, there will be another test 
	pc = (BYTE *)(pi + 2);
	pc0 = (BYTE *)d;
	if (f_vv) {
		cout << "size = " << size << " pad = " << pad << " total = " << total << endl;
		}
	for (i = 0; i < size; i++)
		pc[i] = pc0[i];
	for (i = 0; i < pad; i++)
		pc[size + i] = 0;
	old_file_size = file_size();
	file_seek(old_file_size);
	file_write(data2, 1, total);
	*datref = (UINT4)(old_file_size >> size_of_header_log());
	if (((INT)((UINT)*datref << size_of_header_log())) != old_file_size) {
		cout << "database::add_data_DB ((UINT)*datref << size_of_header_log()) != old_file_size" << endl;
		cout << "old_file_size=" << old_file_size << endl;
		cout << "*datref=" << *datref << endl;
		cout << "size_of_header_log()=" << size_of_header_log() << endl;
		exit(1);
		}
	file_size() += total;
	//put_file_size();
	
	file_seek(old_file_size);
	file_read(new_header, 4, 2);
	block_swap_bytes((SCHAR *)new_header, sizeof(INT4), 2);
	if (header[0] != new_header[0]) {
		cout << "header[0] != new_header[0]\n";
		}
	if (header[1] != new_header[1]) {
		cout << "header[1] != new_header[1]\n";
		}
	delete [] data2;
}

void database::free_data_DB(UINT4 datref, INT size, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	//INT f_vv = (verbose_level >= 2);
	INT total;
	INT4 header[8];
	
	if (f_v) {
		cout << "database::free_data_DB()" << endl;
		}
	if (file_type() == DB_FILE_TYPE_COMPACT)
		return;
	file_seek(((UINT)datref) << size_of_header_log());
	total = 8 * 4;
	file_read(header, 1, total);
	block_swap_bytes((SCHAR *)header, 4, 8);
	if (header[0] != MAGIC_SYNC) {
		cout << "database::free_data_DB()|header: no MAGIC_SYNC\n";
		exit(1);
		}
	if (!header[1]) {
		cout << "database::free_data_DB()|header: data is not used\n";
		exit(1);
		}
	if (header[2] != size) {
		cout << "database::free_data_DB()|header: header[2] != size\n";
		exit(1);
		}
	if (header[4] != MAGIC_SYNC) {
		cout << "database::free_data_DB()|header2: no MAGIC_SYNC\n";
		exit(1);
		}
	if (!header[5]) {
		cout << "database::free_data_DB()|header2: data is not used\n";
		exit(1);
		}
	if (header[6] != size) {
		cout << "database::free_data_DB()|header2: header[6] != size\n";
		exit(1);
		}
	if (header[7] != header[3]) {
		cout << "database::free_data_DB()|header2: header[7] != header[3]\n";
		exit(1);
		}
	header[1] = FALSE;
	header[5] = FALSE;
	block_swap_bytes((SCHAR *)header, 4, 8);
	file_seek(((UINT)datref) << size_of_header_log());
	file_write(header, 1, total);
}

void database::file_open(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT idx = fstream_table_get_free_entry();
	fstream *f = new fstream(filename().s(), ios::in | ios::out | ios::binary);
	fstream_table[idx] = f;
	fstream_table_used[idx] = TRUE;
	stream() = idx;
	f_open() = TRUE;
	get_file_size();
	if (f_v) {
		cout << "database::file_open() file " << filename().s() << " opened" << endl;
		}
}

void database::file_create(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	hollerith cmd;
	
	cmd.init("rm ");
	cmd.append(filename().s());
	system(cmd.s());
	
	INT idx = fstream_table_get_free_entry();
	{
	fstream *f = new fstream(filename().s(), ios::out | ios::binary);
	if (!*f) {
		cout << "database::file_create() file " << filename().s() << " could not be created" << endl;
		exit(1);
		}
	f->close();
	delete f;
	}
	fstream *f = new fstream(filename().s(), ios::in | ios::out | ios::binary);
	if (!*f) {
		cout << "database::file_create() file " << filename().s() << " could not be opened" << endl;
		exit(1);
		}
	fstream_table[idx] = f;
	fstream_table_used[idx] = TRUE;
	stream() = idx;
	f_open() = TRUE;
	if (f_v) {
		cout << "database::file_create() file " << filename().s() << " created" << endl;
		}
}

void database::file_close(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	int idx = stream();
	if (!fstream_table_used[idx]) {
		cout << "database::file_close() !fstream_table_used[idx]" << endl;
		cout << "idx=" << idx << endl;
		exit(1);
		}
	delete fstream_table[idx];
	fstream_table_used[idx] = FALSE;
	stream() = 0;
	f_open() = FALSE;
	if (f_v) {
		cout << "database::file_close() file " << filename().s() << " closed" << endl;
		}
}

void database::file_seek(INT offset)
{
	if (!f_open()) {
		cout << "database::file_seek() file not open" << endl;
		exit(1);
		}
	int idx = stream();
	if (!fstream_table_used[idx]) {
		cout << "database::file_seek() !fstream_table_used[idx]" << endl;
		cout << "idx=" << idx << endl;
		exit(1);
		}
	fstream_table[idx]->seekg(offset);
}

void database::file_write(void *p, INT size, INT nb)
{
	if (!f_open()) {
		cout << "database::file_write() file not open" << endl;
		exit(1);
		}
	int idx = stream();
	if (!fstream_table_used[idx]) {
		cout << "database::file_write() !fstream_table_used[idx]" << endl;
		cout << "idx=" << idx << endl;
		exit(1);
		}
	fstream_table[idx]->write((BYTE *) p, size * nb);
}

void database::file_read(void *p, INT size, INT nb)
{
	if (!f_open()) {
		cout << "database::file_read() file not open" << endl;
		exit(1);
		}
	int idx = stream();
	if (!fstream_table_used[idx]) {
		cout << "database::file_read() !fstream_table_used[idx]" << endl;
		cout << "idx=" << idx << endl;
		exit(1);
		}
	fstream_table[idx]->read((BYTE *) p, size * nb);
}

