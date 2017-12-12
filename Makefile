
CC = mpicc
#gcc

#TARGET=test_knn
TARGET=test_mpi

CFLAGS= -fopenmp -g

BIN=./out
SRC=./src

OBJ = $(BIN)/knn_utilities.o $(BIN)/array_utilities.o

all: $(TARGET)

$(TARGET): clean knn_utilities array_utilities $(SRC)/$(TARGET).c 
	@echo [MAKE]: Building and Linking $(TARGET)
	@$(CC) $(SRC)/$@.c -o $(BIN)/$(TARGET) $(OBJ) $(CFLAGS)
	mpirun -np 2 ./out/test_mpi 

knn_utilities: array_utilities $(SRC)/knn_utilities.c
	@echo [MAKE]: Building knn_utilities
	@$(CC) -c $(SRC)/$@.c $(CFLAGS) 
	@mkdir -p $(BIN)
	@mv $@.o $(BIN)
	
array_utilities: $(SRC)/array_utilities.c
	@echo [MAKE]: Building array_utilities
	@$(CC) -c $(SRC)/$@.c $(CFLAGS) 
	@mkdir -p $(BIN)
	@mv $@.o $(BIN)

clean:
	rm -rf $(BIN)  