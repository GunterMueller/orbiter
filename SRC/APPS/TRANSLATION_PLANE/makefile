SRC=../../../../ORBITER/SRC
DISCRETA_CPP = /usr/bin/g++
CPPFLAGS = -Wall -I$(SRC)/LIB -I$(SRC)/LIB/DISCRETA
LIB = \
	$(SRC)/LIB/TOP_LEVEL/libtoplevel.a \
	$(SRC)/LIB/DISCRETA/libdiscreta.a \
	$(SRC)/LIB/SNAKES_AND_LADDERS/libsnakesandladders.a  \
	$(SRC)/LIB/ACTION/libaction.a \
	$(SRC)/LIB/INCIDENCE/libincidence.a \
	$(SRC)/LIB/GALOIS/libgalois.a 
LFLAGS = -lm



MODULES =  \
	extending.o \


all: translation_plane_main.out 



translation_plane_main.out: translation_plane_main.o $(MODULES) ;
	$(DISCRETA_CPP) translation_plane_main.o $(MODULES) $(CPPFLAGS) $(LIB) $(LFLAGS) -o $@

translation_plane_main.o: translation_plane_main.C ; $(DISCRETA_CPP) $(CPPFLAGS) -c translation_plane_main.C -o $@
extending.o: extending.C ; $(DISCRETA_CPP) $(CPPFLAGS) -c extending.C -o $@


clean:
	- rm *.o *.a *.out *~


