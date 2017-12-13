
CC = mpicc
#gcc

#TARGET=test_knn
#TARGET=test_mpi
TARGET=mpi_knn

CFLAGS= -fopenmp -g

BIN=./out
SRC=./src

OBJ = $(BIN)/knn_utilities.o $(BIN)/array_utilities.o $(BIN)/mpi_utilities.o
 
all: $(TARGET)

$(TARGET): clean knn_utilities mpi_utilities array_utilities $(SRC)/$(TARGET).c 
	@echo [MAKE]: Building and Linking $(TARGET)
	@$(CC) $(SRC)/$@.c -o $(BIN)/$(TARGET) $(OBJ) $(CFLAGS)
	mpirun -np 5 ./out/$@ 

knn_utilities: array_utilities $(SRC)/knn_utilities.c
	@echo [MAKE]: Building $@
	@$(CC) -c $(SRC)/$@.c $(CFLAGS) 
	@mkdir -p $(BIN)
	@mv $@.o $(BIN)
	
mpi_utilities: array_utilities $(SRC)/mpi_utilities.c
	@echo [MAKE]: Building $@
	@$(CC) -c $(SRC)/$@.c $(CFLAGS)
	@mkdir -p $(BIN)
	@mv $@.o $(BIN)

array_utilities: $(SRC)/array_utilities.c
	@echo [MAKE]: Building $@
	@$(CC) -c $(SRC)/$@.c $(CFLAGS) 
	@mkdir -p $(BIN)
	@mv $@.o $(BIN)

clean:
	rm -rf $(BIN) 
	clear 