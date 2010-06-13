FLAGS= -g

elastic : rationals.o utils.o constraints.o toUppaal.o toWatchers.o labStatus.o rationalTreatment.o automata.o avl.o lex.yy.o parser.tab.o preProcessing.tab.o lex.pp.o writeUppaal.o writeHyTech.o tableOfConstants.o system.o enums.h
	gcc  $(FLAGS) -o elastic main.c rationals.o utils.o toUppaal.o toWatchers.o labStatus.o constraints.o rationalTreatment.o automata.o avl.o lex.yy.o parser.tab.o preProcessing.tab.o writeUppaal.o writeHyTech.o lex.pp.o tableOfConstants.o system.o

lex.pp.o : lex.pp.c
	gcc $(FLAGS) -c -pedantic lex.pp.c

toUppaal.o : toUppaal/toUppaal.c toUppaal/toUppaal.h
	gcc $(FLAGS) -c -pedantic -Wall toUppaal/toUppaal.c

toWatchers.o : toWatchers/toWatchers.c toWatchers/toWatchers.h
	gcc $(FLAGS) -c -pedantic -Wall toWatchers/toWatchers.c

preProcessing.tab.o : preProcessing.tab.c
	gcc $(FLAGS) -c -pedantic -Wall preProcessing.tab.c

tableOfConstants.o : preProcessing/tableOfConstants.c preProcessing/tableOfConstants.h
	gcc $(FLAGS) -c -pedantic -Wall preProcessing/tableOfConstants.c

rationals.o : rationals/rationals.c rationals/rationals.h 
	gcc $(FLAGS) -c -pedantic -Wall rationals/rationals.c

labStatus.o : utils/labStatus.c utils/labStatus.h 
	gcc $(FLAGS) -c -pedantic -Wall utils/labStatus.c

utils.o : utils/utils.c utils/utils.h 
	gcc $(FLAGS) -c -pedantic -Wall utils/utils.c

rationalTreatment.o : toUppaal/rationalTreatment.h toUppaal/rationalTreatment.c
	gcc $(FLAGS) -c -pedantic -Wall toUppaal/rationalTreatment.c 

constraints.o : constraints/constraints.c constraints/constraints.h
	gcc $(FLAGS) -c -pedantic -Wall constraints/constraints.c

system.o : system/system.h system/system.c
	gcc $(FLAGS) -c -pedantic -Wall system/system.c

automata.o : automata/automata.h automata/automata.c
	gcc $(FLAGS) -c -pedantic -Wall automata/automata.c

writeUppaal.o : writeUppaal/writeUppaal.h writeUppaal/writeUppaal.c
	gcc $(FLAGS) -c -pedantic  -Wall writeUppaal/writeUppaal.c

writeHyTech.o : writeHyTech/writeHyTech.h writeHyTech/writeHyTech.c
	gcc $(FLAGS) -c -pedantic  -Wall writeHyTech/writeHyTech.c

avl.o : avl/avl.h avl/avl.c
	gcc $(FLAGS) -c -pedantic -Wall avl/avl.c

lex.yy.o : lex.yy.c
	gcc $(FLAGS) -c -pedantic lex.yy.c

parser.tab.o : parser.tab.c
	gcc $(FLAGS) -c -pedantic -Wall parser.tab.c

lex.yy.c : scanner/scanner.lex parser.tab.c
	flex scanner/scanner.lex

parser.tab.c : parser/parser.y parser/parser.h
	bison -d parser/parser.y
	@if [ -e ./parser/parser.tab.c ]; \
		then \
		mv parser/parser.tab.c parser/parser.tab.h . ; \
	fi

lex.pp.c : preProcessing/preProcessing.lex preProcessing.tab.c
	flex -Ppp preProcessing/preProcessing.lex

preProcessing.tab.c : preProcessing/preProcessing.y preProcessing/preProcessing.h
	bison -d -p pp preProcessing/preProcessing.y
	@if [ -e ./preProcessing/preProcessing.tab.c ]; \
		then \
		mv preProcessing/preProcessing.tab.c preProcessing/preProcessing.tab.h . ; \
	fi

clean : 
	rm -f *.o elastic lex.pp.c lex.yy.c parser.tab.c parser.tab.h preProcessing.tab.c preProcessing.tab.h

cleanOld : 
	rm -f *~ 

tgz : 
	make clean
	make cleanOld
	(cd .. ; tar -czvf elastic.tgz elastic/  )
