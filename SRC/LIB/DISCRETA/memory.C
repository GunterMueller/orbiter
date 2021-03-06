// memory.C
//
// Anton Betten
// 04.04.2000
// moved from D2 to ORBI Nov 15, 2007

#include "orbiter.h"

#undef MEMORY_COPY_VERBOSE
#undef DEBUG_MEM
#undef DEBUG_WRITE_CHAR
#undef DEBUG_WRITE_INT

#define MEM_OVERSIZE 32
#define MEM_OVERSIZE1 512

/*
 * INT - offset - 3 + 0: alloc_length
 *              - 3 + 1: used_length
 *              - 3 + 2: cur_pointer
 * 
 * alloc_length: allocated length in BYTEs
 * used_length: used length in BYTES
 * cur_pointer:
 *          0 <= pointer < used length. 
 */



memory::memory()
{
	k = MEMORY;
	self.char_pointer = NULL;
}

memory::memory(const base &x)
	// copy constructor:    this := x
{
	// cout << "memory::copy constructor for object: " << const_cast<base &>(x) << "\n";
	clearself();
	const_cast<base &>(x).copyobject_to(*this);
}

memory& memory::operator = (const base &x)
	// copy assignment
{
	// cout << "memory::operator = (copy assignment)" << endl;
	copyobject(const_cast<base &>(x));
	return *this;
}

void memory::settype_memory()
{
	OBJECTSELF s;
	
	s = self;
	new(this) memory;
	self = s;
	k = MEMORY;
}

memory::~memory()
{
	// cout << "memory::~memory()\n";
	freeself_memory();
}

void memory::freeself_memory()
{

	BYTE *pc;
	INT *pi;
	
	if (self.char_pointer == NULL) {
		// cout << "returning\n";
		return;
		}
	if (s_kind() != MEMORY) {
		cout << "memory::freeself(): kind != MEM" << endl;
		exit(1);
		}
	// cout << "memory::freeself_memory():"; cout << *this << endl;
	pc = self.char_pointer;
	if (pc) {
		pi = (INT *) pc;
		pi -= 3;
		delete [] pi;
		self.char_pointer = NULL;
		}
}

kind memory::s_virtual_kind()
{
	return MEMORY;
}

void memory::copyobject_to(base &x)
{
	INT l;
	
#ifdef MEMORY_COPY_VERBOSE
	cout << "in memory::copyobject_to()\n";
#endif
	x.freeself();
	if (x.s_kind() != MEMORY) {
#ifdef MEMORY_CHANGE_KIND_VERBOSE
		cout << "warning: memory::copyobject_to x not a memory\n";
#endif
		x.c_kind(MEMORY);
		// x.printobjectkindln();
		}
	memory m = x.as_memory();
	
	l = used_length();
	m.init(l, self.char_pointer);
	m.cur_pointer() = cur_pointer();
}

ostream& memory::print(ostream& ost)
{
	if (self.char_pointer == NULL) {
		ost << "memory not allocated";
		}
	cout << "memory, used_length=" << used_length() 
		<< ", alloc_length=" << alloc_length() 
		<< ", cur_pointer=" << cur_pointer() << endl;
	return ost;
}

void memory::init(INT length, char *d)
{
	INT i;
	
	alloc(length);
	for (i = 0; i < length; i++) {
		s_i(i) = d[i];
		}
}

void memory::alloc(INT length)
// sets alloc_length to length + MEM_OVERSIZE, 
// sets used_length to length, 
// sets cur_pointer to 0.
{
	INT size, mem_oversize;
	INT *pi;
	
#ifdef DEBUG_MEM
	cout << "memory::alloc()" << endl;
#endif
	if (length >= MEM_OVERSIZE) {
		mem_oversize = MEM_OVERSIZE1;
		}
	else {
		mem_oversize = MEM_OVERSIZE;
		}
	freeself_memory();
	size = length + mem_oversize + 3 * sizeof(INT);
#ifdef DEBUG_MEM
	cout << "memory::alloc() allocating " << size << " bytes" << endl;
#endif

	pi = (INT *) new char[size];
	if (pi == NULL) {
		cout << "memoy::alloc() out of memory" << endl;
		exit(1);
		}
	self.char_pointer = (char *) (pi + 3);
#ifdef DEBUG_MEM
	cout << "memory::alloc() setting alloc_length()" << endl;
#endif
	alloc_length() = length + mem_oversize;
	used_length() = length;
	cur_pointer() = 0;
	c_kind(MEMORY);
#ifdef DEBUG_MEM
	cout << "memory::alloc() " << used_length() << " bytes allocated." << endl;
#endif
}

void memory::append(INT length, char *d)
{
	BYTE *pc;
	INT i, old_length, new_length;
	
	old_length = used_length();
	new_length = old_length + length;
	if (new_length > alloc_length()) {
		realloc(new_length);
		}
	else {
		used_length() = new_length;
		}
	pc = self.char_pointer;
	for (i = 0; i < length; i++) {
		pc[old_length + i] = d[i];
		}
}

void memory::realloc(INT new_length)
{
	INT old_length;
	INT old_cur_pointer;
	INT i;
	BYTE *old_pc, *pc;
	INT *old_pi;
	
	old_pc = self.char_pointer;
	old_pi = (INT *)old_pc - 3;
	old_length = used_length();
	old_cur_pointer = cur_pointer();
	if (new_length < old_length) {
		cout << "memory::realloc() warning: new_length < old_length" << endl;
		}
	self.char_pointer = NULL;
	alloc(new_length);
	pc = self.char_pointer;
	for (i = 0; 
		i < MINIMUM(old_length, new_length); 
		i++) {
		pc[i] = old_pc[i];
		}
	for (i = old_length; i < new_length; i++) {
		pc[i] = 0;
		}
	delete [] old_pi;
	cur_pointer() = old_cur_pointer;
#ifdef DEBUG_MEM
	cout << "memory::realloc() to " << used_length() << " bytes" << endl;
#endif
}

void memory::write_char(char c)
{	
#ifdef DEBUG_WRITE_CHAR
	cout << "memory::write_char(), at " << used_length() << ", writing char " << (INT) c << endl;
#endif
	append(1, &c);
}

void memory::read_char(char *c)
{
	INT l1, cur_p, l;
	char *cp;
	
	cur_p = cur_pointer();
	l = used_length();
	l1 = l - cur_p;
	if (l1 < 1) {
		cout << "memory::read_char() error: l1 < 1" << endl;
		exit(1);
		}
	cp = self.char_pointer + cur_p;
	*c = *cp;
#ifdef DEBUG_WRITE_CHAR
	cout << "memory::read_char(), at " << cur_pointer() << ", reading char " << (INT) c << endl;
#endif
	cur_pointer()++;
}

void memory::write_int(INT i)
{
	INT4 i1 = (INT4) i;
	
#ifdef DEBUG_WRITE_INT
	cout << "memory::write_int(), at " << used_length() << ", writing int " << i1 << endl;
#endif
	block_swap_bytes((SCHAR *) &i1, 4, 1);
	append(4, (BYTE *) &i1);
}

void memory::read_int(INT *i)
{
	INT4 i1;
	INT l1, j, cur_p, l;
	BYTE *cp, *cp1;
	
	cur_p = cur_pointer();
	l = used_length();
	l1 = l - cur_p;
	if (l1 < 4) {
		cout << "memory::read_int() error: l1 < 4\n";
		exit(1);
		}
	cp = self.char_pointer + cur_p;
	cp1 = (BYTE *) &i1;
	for (j = 0; j < 4; j++) {
		*cp1 = *cp;
		cp1++;
		cp++;
		}
	/* i1 = *(INT *) (cp + cur_p); */
	block_swap_bytes((SCHAR *) &i1, 4, 1);
#ifdef DEBUG_WRITE_INT
	cout << "memory::read_int(), at " << cur_pointer() << ", reading " << i1 << endl;
#endif
	cur_pointer() = cur_p + 4;
	*i = (INT) i1;
}

#include <cstdio>

void memory::read_file(BYTE *fname, INT f_v)
{
	FILE *fp;
	INT fsize;
	BYTE *pc;

	fsize = file_size(fname);
	alloc(fsize);
	pc = self.char_pointer;
	fp = fopen(fname, "r");
	if ((INT) fread(pc, 1 /* size */, fsize /* nitems */, fp) != fsize) {
		cout << "memory::read_file() error in fread" << endl;
		}
	fclose(fp);
	used_length() = fsize;
	cur_pointer() = 0;
	if (f_v) {
		cout << "memory::read_file() read file " 
			<< fname << " of size " << fsize << endl;
		}
}

void memory::write_file(BYTE *fname, INT f_v)
{
	FILE *fp;
	INT size;
	BYTE *pc;

	size = used_length();
	pc = self.char_pointer;
	
	fp = fopen(fname, "wb");

	fwrite(pc, 1 /* size */, size /* items */, fp);
	
	fclose(fp);
	if (file_size(fname) != size) {
		cout << "memory::write_file() error: file_size(fname) != size\n";
		exit(1);
		}
	if (f_v) {
		cout << "memory::write_file() wrote file " 
			<< fname << " of size " << size << endl;
		}
}

INT memory::multiplicity_of_character(BYTE c)
{
	BYTE *pc;
	INT i, l = 0, len;
	
	pc = self.char_pointer;
	len = used_length();
	for (i = 0; i < len; i++)
		if (pc[i] == c)
			l++;
	return l;
}

static void code(UBYTE *pc, INT l, UBYTE *pc2, UBYTE code_char);
static INT decode(UBYTE *pc2, INT l2, UBYTE *pc, UBYTE code_char);

static void code(UBYTE *pc, INT l, UBYTE *pc2, UBYTE code_char)
/* Wolfgang Boessenecker 940919 */
{
	UBYTE cc;
	INT pos = 0, pos2 = 0, pos2h = 0, i;

	while (pos < l) {
		pos2++;
		cc = 0;
#if 0
		if ((posf % 100000) == 0) {
			cout << posf << endl;
			}
#endif
		for (i = 0; i < 8; i++) {
			cc <<= 1;
			if (pos < l) {
				if (pc[pos] == code_char)
					cc = cc | 0X1U;
				else {
					pc2[pos2] = pc[pos];
					pos2++;
					}
				pos++;
				}
			}
		pc2[pos2h] = cc;
		pos2h = pos2;
		}
}

static INT decode(UBYTE *pc2, INT l2, UBYTE *pc, UBYTE code_char)
// returns length of decompressed data 
// pc may be NULL */
{
	UBYTE cc = 0;
	INT pos = 0, pos2 = 0, i = 8;
	
	while (TRUE) {
	/* for (; pos2 < l2; ) { */
		if (pos2 >= l2 && i >= 8)
			break;
		if (i == 8) {
			cc = pc2[pos2];
			pos2++;
			i = 0;
			}
		if (cc & (UBYTE) 128U) {
			if (pc) {
				pc[pos] = code_char;
				}
			pos++;
			}
		else {
			if (pos2 < l2) {
				if (pc) {
					pc[pos] = pc2[pos2];
					}
				pos2++;
				pos++;
				}
			}
		cc <<= 1;
		i++;
		}
	return pos;
}

void memory::compress(INT f_v)
// Wolfgang Boessenecker 9/94 
{
	memory mem2;
	BYTE *pc, *pc2;
	INT l, l2, l_c;

	pc = self.char_pointer;
	l = used_length();
	if (f_v) {
		cout << "compressing from " << l << " to ";
		}
	l_c = multiplicity_of_character((BYTE) 0);
	l2 = l - l_c + ((l + 7) >> 3);
	mem2.alloc(l2); // sets used_length to l2 
	pc2 = mem2.self.char_pointer;
	code((UBYTE *) pc, l, (UBYTE *) pc2, (UBYTE) 0);
#if 0
	if (l3 != l2) {
		cout << "memory::compress() warning: l2 = " << l2 << " != l3 = " << l3 << endl;
		}
#endif
	swap(mem2);
	if (f_v) {
		cout << l2 << " bytes." << endl;
		print(cout);
		}
}

void memory::decompress(INT f_v)
{
	memory mem;
	BYTE *pc, *pc2;
	INT l, l2;
	
	pc2 = self.char_pointer;
	l2 = used_length();
	if (f_v) {
		cout << "decompressing from " << l2 << " to ";
		}
	l = decode((UBYTE *) pc2, l2, NULL, (UBYTE) 0);
	mem.alloc(l);
	pc = mem.self.char_pointer;
	decode((UBYTE *) pc2, l2, (UBYTE *) pc, (UBYTE) 0);
	swap(mem);
	if (f_v) {
		cout << l << " bytes." << endl;
		}
}

INT memory::csf()
{
	INT l;
	INT size = 0;
	
	l = used_length();
	size += 4; /* l */
	size += l;
	return size;
}

void memory::write_mem(memory & M, INT debug_depth)
{
	INT i, l, a;
	
	l = used_length();
	M.write_int(l);
	for (i = 0; i < l; i++) {
		a = s_i(i);
		M.write_char((BYTE) a);
		}
}

void memory::read_mem(memory & M, INT debug_depth)
{
	INT i, l;
	char c;
	BYTE *mem;
	
	M.read_int(&l);	
	mem = new BYTE[l];
	
	for (i = 0; i < l; i++) {
		M.read_char(&c);
		mem[i] = c;
		}
	M.init(l, mem);
	delete [] mem;
}



