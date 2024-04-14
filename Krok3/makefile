CC=gcc
CFLAGS= -Wall -Wextra

puzzle_solver: puzzle_solver.o puzzle_processing.o linked_list.o bitmap.o contour.o describe_side.o corners.o puzzle_match.o solve_border.o
	$(CC) -pthread -o puzzle_solver puzzle_solver.o puzzle_processing.o linked_list.o bitmap.o contour.o describe_side.o corners.o puzzle_match.o solve_border.o -lm

clean:
	rm *.o

distclean:
	rm *.o puzzle_solver
