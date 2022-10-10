COVID_test: COVID_test.c
	$(CC) -pthread $^ -g -Wall -o $@

clean:
	$(RM) COVID_test

submission:
	zip -r proj_five_submission COVID_test.c README.txt Makefile