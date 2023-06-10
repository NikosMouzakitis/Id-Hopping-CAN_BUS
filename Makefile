all:
	gcc sender.c -o sender
	gcc receiver.c -o receiver
	gcc injecter.c -o injecter 

clean:
	rm sender receiver injecter

