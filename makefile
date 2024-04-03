all: librairies archives serveur client
	#  clear
	#  @echo "Compilation terminée avec succès."

serveur: archives
	gcc -DSERVEUR -o mainSrv main.c -L./archives -lUNIX

client: archives
	gcc -o mainClt main.c -L./archives -lUNIX

librairies:
	gcc -c ./sources/data.c -o ./libs/data.o
	gcc -c ./sources/session.c -o ./libs/session.o

archives: librairies
	mkdir -p archives
	mkdir -p libs
	ar rcs ./archives/libUNIX.a ./libs/data.o ./libs/session.o

clean:
	rm -f mainSrv mainClt ./libs/* ./archives/*
	clear
	@echo "Nettoyage terminé avec succès."


