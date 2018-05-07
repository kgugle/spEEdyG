all: ErrorMetric.c
	gcc -g -O3 -Wall cycletimer.c ErrorMetric.c -o ErrorMetric -lm

clean:
	$(RM) ErrorMetric
