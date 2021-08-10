SIM_DIR = models/fluid/src
GRAPHICS_DIR = models/fluid/graphics/src


TRICK_CFLAGS += -Imodels -I/usr/local/cuda/include
TRICK_CXXFLAGS += -Imodels -I/usr/local/cuda/include
TRICK_USER_LINK_LIBS += $(SIM_DIR)/gpuCode.o $(SIM_DIR)/sph_gpu.o $(GRAPHICS_DIR)/iso_values.o $(GRAPHICS_DIR)/opengl_caller.o $(GRAPHICS_DIR)/opengl.o $(GRAPHICS_DIR)/gui.o $(GRAPHICS_DIR)/grid_cell.o $(GRAPHICS_DIR)/marching_cubes.o -L/usr/local/cuda/lib64 -lcudart -lcudadevrt -lglfw -lGL -lGLEW -pthread



$(S_MAIN): $(GRAPHICS_DIR)/opengl_caller.o $(GRAPHICS_DIR)/opengl.o $(GRAPHICS_DIR)/gui.o $(GRAPHICS_DIR)/grid_cell.o $(GRAPHICS_DIR)/marching_cubes.o $(GRAPHICS_DIR)/iso_values.o

$(GRAPHICS_DIR)/grid_cell.o: $(GRAPHICS_DIR)/grid_cell.cpp
	g++ -c $(GRAPHICS_DIR)/grid_cell.cpp -o $(GRAPHICS_DIR)/grid_cell.o

$(GRAPHICS_DIR)/marching_cubes.o: $(GRAPHICS_DIR)/marching_cubes.cpp
	g++ -c $(GRAPHICS_DIR)/marching_cubes.cpp -o $(GRAPHICS_DIR)/marching_cubes.o

$(GRAPHICS_DIR)/opengl_caller.o: $(GRAPHICS_DIR)/opengl_caller.cpp
	g++ -c $(GRAPHICS_DIR)/opengl_caller.cpp -o $(GRAPHICS_DIR)/opengl_caller.o

$(GRAPHICS_DIR)/opengl.o: $(GRAPHICS_DIR)/opengl.cpp
	g++ -c $(GRAPHICS_DIR)/opengl.cpp -o $(GRAPHICS_DIR)/opengl.o

$(GRAPHICS_DIR)/gui.o: $(GRAPHICS_DIR)/gui.cpp
	g++ -c $(GRAPHICS_DIR)/gui.cpp -o $(GRAPHICS_DIR)/gui.o

$(S_MAIN): $(SIM_DIR)/gpuCode.o

$(SIM_DIR)/sph_gpu.o: $(SIM_DIR)/sph_gpu.cu
	nvcc -arch=sm_35 -I. -dc $(SIM_DIR)/sph_gpu.cu -o $(SIM_DIR)/sph_gpu.o

$(GRAPHICS_DIR)/iso_values.o: $(GRAPHICS_DIR)/iso_values.cu
	nvcc -arch=sm_35 -I. -dc $(GRAPHICS_DIR)/iso_values.cu -o $(GRAPHICS_DIR)/iso_values.o

$(SIM_DIR)/gpuCode.o: $(SIM_DIR)/sph_gpu.o
	nvcc -arch=sm_35 -dlink $(SIM_DIR)/sph_gpu.o $(GRAPHICS_DIR)/iso_values.o $(GRAPHICS_DIR)/opengl_caller.o $(GRAPHICS_DIR)/opengl.o $(GRAPHICS_DIR)/gui.o $(GRAPHICS_DIR)/grid_cell.o $(GRAPHICS_DIR)/marching_cubes.o $(S_OBJECTS) -o $(SIM_DIR)/gpuCode.o
	
clean: my_clean
	
my_clean:
	-rm -rf $(SIM_DIR)/sph_gpu.o $(SIM_DIR)/gpuCode.o $(GRAPHICS_DIR)/grid_cell.o $(GRAPHICS_DIR)/gui.o $(GRAPHICS_DIR)/iso_values.o $(GRAPHICS_DIR)/marching_cubes.o $(GRAPHICS_DIR)/opengl_caller.o $(GRAPHICS_DIR)/opengl.o
