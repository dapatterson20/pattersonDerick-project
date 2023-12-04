OBJECTS = paging.o

paging: $(OBJECTS)
	gcc -o $@ $(OBJECTS) 

paging.o:paging.c
	gcc -c paging.c

clean:
	-rm *.o core paging
