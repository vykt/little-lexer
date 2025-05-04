.RECIPEPREFIX=>

debug:
> g++ -ggdb3 -DDEBUG -O0 -o little-lexer.dbg *.cc

release:
> g++ -ggdb3 -O0 -o little-lexer.rel *.cc

clean:
> rm *.o little-lexer.*
