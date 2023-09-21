factorial: factorial.c
	gcc -o factorial -g factorial.c

leaktest: leaktest.c
	gcc -o leaktest -g leaktest.c

memtest: test
	valgrind --leak-check=full --log-file=valgrind-out.txt ./leaktest