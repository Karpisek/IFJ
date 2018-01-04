#
#  Fakutla informacnich technologii, Vysoke uceni technicke v Brne
#	Projekt IFJ
#  Implementace prekladace imperativniho jazyka IFJ17
#
#  Tym:    Jakub Trubka        (xtrubk00)
#          Roman Zajic         (xzajic16)
#          Miroslav Karpisek   (xkarpi05)
#          Jakub Samek         (xsamek07)
#
#  Soubor: Makefile
#  Autori: Jakub Samek
#
#  Datum:  21-11-2017
#  Uprava: 02-12-2017


CC=gcc
FLAGS=-std=c99 -Wall -Wextra -pedantic
PROJ_NAME=ifj
DEBUG=-DDEBUG_LEX -DDEBUG_PARSER -DDEBUG_LIST
GDB=-g

all:
	$(CC) $(FLAGS) *c -o $(PROJ_NAME)

debug:
	$(CC) $(FLAGS) *c -o $(PROJ_NAME) $(DEBUG)

gdb:
	$(CC) $(FLAGS) *c -o $(PROJ_NAME) $(GDB)

clean:
	rm $(PROJ_NAME)
	rm -rf tests_results
	rm -rf tests_results_single
	rm -rf tests_single
	rm xsamek07.zip

test:
	./tester --interpret

zip:
	zip -q xsamek07.zip *.c *.h Makefile rozdeleni rozsireni dokumentace.pdf -x "*.DS_Store"

zip-2:
	zip -qr xsamek07.zip *.c *.h Makefile rozdeleni rozsireni dokumentace.pdf tests tester ic17int -x "*.DS_Store"

final-test:
	./is_it_ok.sh xsamek07.zip tmp
	rm -rf tmp
