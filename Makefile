
#TARGET=test_knn
#TARGET=test_mpi
#TARGET=mpi_knn
TARGET=mpi_knn_blocking


ifeq ($(TARGET),test_knn)
OBJ = $(BIN)/knn_utilities.o $(BIN)/array_utilities.o 
CC=gcc
else
OBJ = $(BIN)/knn_utilities.o $(BIN)/array_utilities.o $(BIN)/mpi_utilities.o	
CC=mpicc
endif

CFLAGS= -fopenmp -O3


BIN=./out
SRC=./src


all: $(TARGET)

$(TARGET): clean knn_utilities mpi_utilities array_utilities $(SRC)/$(TARGET).c 
	@echo [MAKE]: Building and Linking $(TARGET)
	$(CC) $(SRC)/$@.c -o $(BIN)/$(TARGET) $(OBJ) $(CFLAGS)
	
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
	clear
	rm -rf $(BIN) 

run: all
ifeq ($(TARGET),test_knn)
	./out/$(TARGET)
else
	mpirun -np 2 ./out/$(TARGET) 3 2
endif
