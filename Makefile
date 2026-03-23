all : servbeuip clibeuip

servbeuip : servbeuip.c
	cc -o servbeuip servbeuip.c -Wall -Werror
	
clibeuip : clibeuip.c
	cc -o clibeuip clibeuip.c -Wall -Werror
	
clean :
	rm -f clibeuip servbeuip