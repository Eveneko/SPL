CC=gcc
FLEX=flex
BISON=bison

.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d syntax.y
splc: clean .lex .syntax
	@mkdir bin
	touch bin/splc
	@chmod +x bin/splc
	$(CC) ast.c syntax.tab.c -lfl -ly -o bin/splc
clean:
	@rm -rf bin/
	@rm -f lex.yy.c syntax.tab.* *.out
.PHONY: splc