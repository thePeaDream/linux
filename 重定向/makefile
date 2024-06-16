.PHONY:all
all:input_redirect output_redirect append_redirect

input_redirect:input_redirect.c
	gcc -o $@ $^
output_redirect:output_redirect.c
	gcc -o $@ $^
append_redirect:append_redirect.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f input_redirect output_redirect append_redirect
