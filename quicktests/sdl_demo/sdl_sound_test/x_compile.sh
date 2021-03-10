FILE="gen_program.c"
OPT=" `sdl2-config --cflags --libs` -lGL -lm"
	gcc -fsyntax-only -o prog.tst $FILE $OPT 
if [ $? -ne 0 ]
then
	exit
else
	echo "================================================"
	gcc -Wall -o prog.tst $FILE $OPT 
	echo "		****** Compile DONE ****	"
	echo "================================================"
fi
