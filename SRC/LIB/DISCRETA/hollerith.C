// hollerith.C
//
// Anton Betten
// 24.04.2000
// moved from D2 to ORBI Nov 15, 2007

#include "orbiter.h"


#undef HOLLERITH_COPY_VERBOSE


hollerith::hollerith()
{
	k = HOLLERITH;
	self.char_pointer = NULL;
}

hollerith::hollerith(char *p)
{
	k = HOLLERITH;
	self.char_pointer = NULL;
	init(p);
}

hollerith::hollerith(const base &x)
	// copy constructor:    this := x
{
	// cout << "hollerith::copy constructor for object: " << const_cast<base &>(x) << "\n";
	clearself();
	const_cast<base &>(x).copyobject_to(*this);
}

hollerith& hollerith::operator = (const base &x)
	// copy assignment
{
	// cout << "hollerith::operator = (copy assignment)" << endl;
	copyobject(const_cast<base &>(x));
	return *this;
}

void hollerith::settype_hollerith()
{
	OBJECTSELF s;
	
	s = self;
	new(this) hollerith;
	k = HOLLERITH;
	self = s;
}

hollerith::~hollerith()
{
	freeself_hollerith();
}

void hollerith::freeself_hollerith()
{
	// cout << "hollerith::freeself_hollerith()" << endl;
	if (self.char_pointer == NULL)
		return;
	delete[] self.char_pointer;
	self.char_pointer = NULL;
}

kind hollerith::s_virtual_kind()
{
	return HOLLERITH;
}

void hollerith::copyobject_to(base &x)
{
#ifdef HOLLERITH_COPY_VERBOSE
	cout << "in hollerith::copyobject_to()\n";
#endif
	x.freeself();
	hollerith & xx = x.change_to_hollerith();
	if (s() == NULL)
		return;
	xx.init(s());
}

ostream& hollerith::print(ostream& ost)
{
	if (s() == NULL)
		ost << "(null)";
	else
		ost << s();
	return ost;
}

INT hollerith::compare_with(base &a)
{
	BYTE *p1, *p2;
	
	if (a.s_kind() != HOLLERITH) {
		cout << "hollerith::compare_with() a is not a hollerith object" << endl;
		exit(1);
		}
	p1 = s();
	p2 = a.as_hollerith().s();
	return strcmp(p1, p2);
}

void hollerith::init(const char *p)
{
	INT l;
	
	// cout << "hollerith::init() for string \"" << p << "\"" << endl;
	freeself_hollerith();
	l = strlen(p);
	self.char_pointer = (char *) new char[l + 1];
	strcpy(s(), p);
}

void hollerith::append(const char *p)
{
	INT l1, l2, l3;
	char *q;
	
	l1 = strlen(s());
	l2 = strlen(p);
	l3 = l1 + l2;
	q = (char *) new char[l3 + 1];
	strcpy(q, s());
	strcat(q, p);
	delete[] self.char_pointer;
	self.char_pointer = q;
}

void hollerith::append_i(INT i)
{
	BYTE str[1000];
	
	itoa(str, 1000, i);
	append(str);
}

void hollerith::write_mem(memory & m, INT debug_depth)
{
	INT i, l;
	
	if (s()) {
		l = strlen(s());
		for (i = 0; i < l; i++) {
			m.write_char(s()[i]);
			}
		}
	m.write_char((char) 0);
}

//#define BUFSIZE 1000

void hollerith::read_mem(memory & m, INT debug_depth)
{
	INT l;
	char buf[BUFSIZE + 1], c;
	
	freeself();
	l = 0;
	while (TRUE) {
		m.read_char(&c);
		if (c == 0 || l == BUFSIZE) {
			buf[l] = 0;
			append(buf);
			l = 0;
			}
		if (c == 0)
			break;
		buf[l++] = c;
		}	
}

INT hollerith::csf()
{
	if (s())
		return strlen(s()) + 1;
	else
		return 1;
}

void hollerith::chop_off_extension_if_present(BYTE *ext)
{
	char *p = s();
	INT l1 = strlen(p);
	INT l2 = strlen(ext);
	
	if (l1 > l2 && strcmp(p + l1 - l2, ext) == 0) {
		p[l1 - l2] = 0;
		}
}

void hollerith::get_extension_if_present(BYTE *ext)
{
	char *p = s();
	INT i, l = strlen(p);
	
	ext[0] = 0;
	for (i = l - 1; i >= 0; i--) {
		if (p[i] == '.') {
			strcpy(ext, p + i);
			}
		}
}

void hollerith::get_current_date()
{
	BYTE str[1000];

	system("rm a");
	system("date >a");
	{
	ifstream f1("a");
	f1.getline(str, sizeof(str));
	}
	init(str);
}
