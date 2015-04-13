CC 		= @mpicc
CFLAGS 	= -Wall -g -std=c99
ECHO   	= @echo

all: 4_10 4_11

clean:
	@rm -rf bin
	$(ECHO) "All binaries removed"

directories:
	@mkdir -p bin
	$(ECHO) "Output directories created"


4_10: directories
	$(CC) $(CFLAGS) "4_10/mpi_college.c" -o bin/mpi_college
	$(ECHO) "Build 4_10"

4_11: directories
	$(CC) $(CFLAGS) "4_11/mpi_pi.c" -o bin/mpi_pi
	$(ECHO) "Build 4_11"
