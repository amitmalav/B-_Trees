

amlayer.o : am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o amscan.o amprint.o
	ld -r am.o amfns.o amsearch.o aminsert.o  amstack.o amglobals.o amscan.o amprint.o  -o amlayer.o

am.o : am.c am.h pf.h
	cc -c am.c

amfns.o : amfns.c am.h pf.h
	cc -c amfns.c

amsearch.o : amsearch.c am.h pf.h
	cc -c amsearch.c

aminsert.o : aminsert.c am.h pf.h
	cc -c aminsert.c

amscan.o : amscan.c am.h pf.h
	cc -c amscan.c

amstack.o : amstack.c am.h pf.h
	cc -c amstack.c

amglobals.o : amglobals.c am.h
	cc -c amglobals.c

amprint.o : amprint.c am.h pf.h 
	cc -c amprint.c
	
main.o : main.c am.h pf.h 
	cc -c main.c
main1.o : main1.c am.h pf.h
	cc -c main1.c

all: test1 test2

test1 : am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o ../pflayer/pflayer.o main.o amscan.o amprint.o
	cc -o test1 am.o amfns.o amsearch.o aminsert.o  amstack.o amglobals.o ../pflayer/pflayer.o main.o amscan.o amprint.o 
test2 : am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o ../pflayer/pflayer.o main1.o amscan.o amprint.o
	cc -o test2 am.o amfns.o amsearch.o aminsert.o  amstack.o amglobals.o ../pflayer/pflayer.o main1.o amscan.o amprint.o

clean:
	rm -f *~ *.o test1 test2 testrel.0 test1rel.0

