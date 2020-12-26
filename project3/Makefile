CC=gcc
FLEX=flex
BISON=bison

.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d syntax.y
lex: .lex
	$(CC) lex.yy.c -lfl
parser: .lex .syntax
	$(CC) syntax.tab.c -lfl -ly -o bin/jp.out

splc: clean
	@mkdir bin
	touch bin/splc
	flex lex.l && bison -t -d -y -v syntax.y
	g++ -lfl -ly ast.cpp semantic.cpp ir.cpp y.tab.c -o ./bin/splc
	@chmod +x bin/splc
clean:
	@rm -rf bin/
	@rm -f lex.yy.* y.tab.*
.PHONY: splc