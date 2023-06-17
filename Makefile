all:
	gcc sender.c -o sender -lpthread
	gcc receiver.c -o receiver
	gcc injecter.c -o injecter 

clean:
	rm sender receiver injecter

