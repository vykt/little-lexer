.RECIPEPREFIX=>

debug:
	gcc -ggdb3 -DDEBUG -O0 -o little-lexer.dbg *.c

release:
	gcc -ggdb3 -O0 -o little-lexer.rel *.c

clean:
	rm *.o little-lexer.*
