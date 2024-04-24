all: librairies archives serveur client
	# clear
	# @echo "Compilation terminée avec succès."

serveur: archives
	gcc -DSERVEUR -o mainSrv main.c -L./archives -lUNIX -lncurses
client: archives
	gcc -o mainClt main.c -L./archives -lUNIX -lncurses

librairies:
	mkdir -p archives
	mkdir -p libs
	gcc -c ./sources/data.c -o ./libs/data.o
	gcc -c ./sources/session.c -o ./libs/session.o
	gcc -c ./sources/users.c -o ./libs/users.o
	gcc -c ./sources/channels.c -o ./libs/channels.o
	gcc -c ./sources/display.c -o ./libs/display.o
	gcc -c ./sources/messages.c -o ./libs/messages.o
	gcc -c ./sources/ncurses.c -o ./libs/ncurses.o

archives: librairies
	ar rcs ./archives/libUNIX.a ./libs/data.o ./libs/session.o ./libs/users.o ./libs/channels.o ./libs/display.o ./libs/messages.o ./libs/ncurses.o

clean:
	rm -f mainSrv mainClt ./libs/* ./archives/* ./tmp/*
	clear
	@echo "Nettoyage terminé avec succès."


