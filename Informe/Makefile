all:
	pdflatex bare_jrnl.tex
	bibtex bare_jrnl.aux
	pdflatex bare_jrnl.tex
	pdflatex bare_jrnl.tex
.PHONY: clean

clean:
	rm -f bare_jrnl.aux bare_jrnl.bbl bare_jrnl.blg bare_jrnl.log bare_jrnl.pdf bare_jrnl.tex.backup bare_jrnl.synctex.gz
	