TRICK_CFLAGS += -Imodels -I/usr/local/cuda/include
TRICK_CXXFLAGS += -Imodels -I/usr/local/cuda/include
TRICK_USER_LINK_LIBS += models/fluid/src/gpuCode.o models/fluid/src/sph_gpu.o models/fluid/graphics/src/iso_values.o models/fluid/graphics/src/opengl_caller.o models/fluid/graphics/src/opengl.o models/fluid/graphics/src/gui.o models/fluid/graphics/src/grid_cell.o models/fluid/graphics/src/marching_cubes.o -L/usr/local/cuda/lib64 -lcudart -lcudadevrt -lglfw -lGL -lGLEW -pthread

$(S_MAIN): models/fluid/graphics/src/opengl_caller.o models/fluid/graphics/src/opengl.o models/fluid/graphics/src/gui.o models/fluid/graphics/src/grid_cell.o models/fluid/graphics/src/marching_cubes.o

models/fluid/graphics/src/grid_cell.o: models/fluid/graphics/src/grid_cell.cpp
	g++ -c models/fluid/graphics/src/grid_cell.cpp -o models/fluid/graphics/src/grid_cell.o

models/fluid/graphics/src/marching_cubes.o: models/fluid/graphics/src/marching_cubes.cpp
	g++ -c models/fluid/graphics/src/marching_cubes.cpp -o models/fluid/graphics/src/marching_cubes.o

models/fluid/graphics/src/opengl_caller.o: models/fluid/graphics/src/opengl_caller.cpp
	g++ -c models/fluid/graphics/src/opengl_caller.cpp -o models/fluid/graphics/src/opengl_caller.o

models/fluid/graphics/src/opengl.o: models/fluid/graphics/src/opengl.cc
	g++ -c models/fluid/graphics/src/opengl.cc -o models/fluid/graphics/src/opengl.o

models/fluid/graphics/src/gui.o: models/fluid/graphics/src/gui.cc
	g++ -c models/fluid/graphics/src/gui.cc -o models/fluid/graphics/src/gui.o

$(S_MAIN): models/fluid/src/gpuCode.o

models/fluid/src/sph_gpu.o: models/fluid/src/sph_gpu.cu
	nvcc -arch=sm_35 -I. -dc models/fluid/src/sph_gpu.cu -o models/fluid/src/sph_gpu.o

models/fluid/graphics/src/iso_values.o: models/fluid/graphics/src/iso_values.cu
	nvcc -arch=sm_35 -I. -dc models/fluid/graphics/src/iso_values.cu -o models/fluid/graphics/src/iso_values.o

models/fluid/src/gpuCode.o: models/fluid/src/sph_gpu.o
	nvcc -arch=sm_35 -dlink models/fluid/src/sph_gpu.o models/fluid/graphics/src/iso_values.o models/fluid/graphics/src/opengl_caller.o models/fluid/graphics/src/opengl.o models/fluid/graphics/src/gui.o models/fluid/graphics/src/grid_cell.o models/fluid/graphics/src/marching_cubes.o $(S_OBJECTS) -o models/fluid/src/gpuCode.o
	#nvcc -arch=sm_35 -dlink $(TRICK_SYSTEM_LDFLAGS) $(S_OBJECTS) $(LINK_LISTS) $(TRICK_LDFLAGS) $(TRICK_USER_LINK_LIBS) $(READ_ONLY_LIBS) $(LD_WHOLE_ARCHIVE) $(TRICK_LIBS) $(LD_NO_WHOLE_ARCHIVE) $(TRICK_EXEC_LINK_LIBS)
	
clean: my_clean
	
	
my_clean:
	-rm -rf models/fluid/src/sph_gpu.o models/fluid/src/gpuCode.o
	