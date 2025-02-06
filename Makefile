calc: parser.c 
	gcc -g -o calc parser.c tokenizer.c
tokenizer: tokenizer.c 
	gcc -g -o tokenizer tokenizer.c
clean:
	rm -f *.o