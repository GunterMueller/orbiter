MY_PATH=../../../../../DEV.15
SRC=$(MY_PATH)/ORBITER/SRC


GRAPH_PATH=$(SRC)/APPS/GRAPH
TOOLS_PATH=$(SRC)/APPS/TOOLS

clean:
	- rm graph_*

g:
	$(GRAPH_PATH)/graph.out -n 5 -v 4 \
		-W -draw_poset  -t \
		-draw_graphs
	pdflatex graph_5_poset_lvl_10.tex
	open graph_5_poset_lvl_10.pdf



poset:
	$(TOOLS_PATH)/layered_graph_main.out -v 4 \
		-file graph_5_poset_lvl_10.layered_graph \
		-draw graph_5_poset_lvl_10 \
		-rad 8000 \
		-as_graph \
		-xin 1000000 \
		-yin 1000000 \
		-xout 1000000 \
		-yout 1000000 \
		-embedded\
		-scale .30 \
		-line_width 1.0 \
		-y_stretch 1.0 
	pdflatex graph_5_poset_lvl_10.tex
	open graph_5_poset_lvl_10.pdf


#		-text_underneath


g_full:
	$(GRAPH_PATH)/graph.out -n 5 -v 4 \
		-W -draw_full_poset -t
	pdflatex graph_5_poset_full_lvl_10.tex
	open graph_5_poset_full_lvl_10.pdf
 


full:
	$(TOOLS_PATH)/layered_graph_main.out -v 4 \
		-file graph_5_poset_full_lvl_10.layered_graph \
		-draw test \
		-rad 8000 \
		-as_graph \
		-xin 1000000 \
		-yin 1000000 \
		-xout 1000000 \
		-yout 1000000 \
		-embedded \
		-scale .30 \
		-line_width 1.0 \
		-y_stretch 0.8 
	pdflatex test.tex
	open test.pdf


detailed:
	$(TOOLS_PATH)/layered_graph_main.out -v 4 \
		-file graph_5_poset_detailed_lvl_10.layered_graph \
		-draw graph_5_poset_detailed_lvl_10 \
		-rad 8000 \
		-as_graph \
		-xin 1000000 \
		-yin 1000000 \
		-xout 1000000 \
		-yout 1000000 \
		-boxed 3 \
		-embedded\
		-scale .60 \
		-line_width 1.0 \
		-y_stretch 2.0 \
		-text_underneath
	pdflatex graph_5_poset_detailed_lvl_10.tex
	open graph_5_poset_detailed_lvl_10.pdf


