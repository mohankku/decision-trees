MAIN ?= p
DIFF ?= HEAD^
CODE := $(addsuffix .tex,$(filter-out %.tex,$(wildcard code/*)))
FIGS := $(patsubst %.svg,%.pdf,$(wildcard fig/*.svg))
ODGS := $(patsubst %.odg,%.pdf,$(wildcard fig/*.odg))
PLOT := $(patsubst %.gp,%.tex,$(wildcard data/*.gp))
DEPS := rev.tex code/fmt.tex $(CODE) $(FIGS) $(ODGS) $(PLOT)
BTEX := --bibtex-args="-min-crossrefs=99"
LTEX := --latex-args="-shell-escape"

all: $(DEPS)
	@TEXINPUTS="sty:" bin/latexrun $(LTEX) $(BTEX) $(MAIN)

submit: $(DEPS)
	@for f in $(wildcard submit-*.tex); do \
		TEXINPUTS="sty:" bin/latexrun $$f; \
	done
	mkdir -p submit/
	cp -f submit-*.pdf submit/
	cp -f bio/*.pdf submit/
	cp -f ../admin/intel-letter.pdf submit/
	zip -r submit.zip submit/

diff: $(DEPS)
	@bin/diff.sh $(DIFF)

help:
	echo "..."

rev.tex: FORCE
	@printf '\\gdef\\therev{%s}\n\\gdef\\thedate{%s}\n' \
	   "$(shell git rev-parse --short HEAD)"            \
	   "$(shell git log -1 --format='%ci' HEAD)" > $@

code/%.tex: code/%
	pygmentize -P tabsize=4 -P mathescape -f latex $^ > $@

code/fmt.tex:
	pygmentize -f latex -S default > $@

fig/%.pdf: fig/%.svg
	inkscape --without-gui -f $^ -D -A $@

fig/%.pdf: fig/%.odg
	soffice --convert-to pdf $< --outdir $(@D)
	pdfcrop $@ $@

data/%.tex: data/%.gp
	OUT=$@ TARGET=paper-epslatex gnuplot $<

draft: $(DEPS)
	echo -e '\\newcommand*{\\DRAFT}{}' >> rev.tex
	@TEXINPUTS="sty:" bin/latexrun $(BTEX) $(MAIN)

watermark: $(DEPS)
	echo -e '\\usepackage[firstpage]{draftwatermark}' >> rev.tex
	@TEXINPUTS="sty:" bin/latexrun $(BTEX) $(MAIN)

spell:
	@for i in *.tex fig/*.tex; do bin/aspell.sh $$i; done
	@for i in *.tex; do bin/double.pl $$i; done
	@for i in *.tex; do bin/abbrv.pl  $$i; done
	@bin/hyphens.sh *.tex

clean:
	@bin/latexrun --clean

distclean: clean
	rm -f code/*.tex
	rm -f data/*.tex
	rm -f data/*.eps
	rm -f data/*-eps-converted-to.pdf
	rm -f fig/*.pdf

init:
	rm -f {code,fig,data}/ex-*
	perl -pi -e 's/^\\input{ex}/% \\input{ex}/g' $(MAIN).tex

abstract.txt: abstract.tex $(MAIN).tex
	@bin/mkabstract $(MAIN).tex $< > $@

.PHONY: all help FORCE draft clean spell distclean init
