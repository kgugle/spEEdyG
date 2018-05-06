all: ErrorMetric.c
	gcc -g -O3 -Wall ErrorMetric.c -o ErrorMetric -lm

clean:
	$(RM) ErrorMetric
