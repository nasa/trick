
"""
Allocate and initialize first simplex to begin Amoeba algorithm
"""

optimizer.amoeba.debug = 0 ;

"""Allocate a 5x5 matrix that holds simplex and function results"""
optimizer.amoeba.init_simplex = trick.alloc_type(5, "double *") 
optimizer.amoeba.init_simplex[0] = trick.alloc_type(5, "double") 
optimizer.amoeba.init_simplex[1] = trick.alloc_type(5, "double") 
optimizer.amoeba.init_simplex[2] = trick.alloc_type(5, "double") 
optimizer.amoeba.init_simplex[3] = trick.alloc_type(5, "double") 
optimizer.amoeba.init_simplex[4] = trick.alloc_type(5, "double") 

"""
Note that function results are in the 5th column.  They will be
calculated by the simulation.  For now we set them to zero.
"""
optimizer.amoeba.init_simplex[0] =  [4.0, 4.5, 5.0, 6.0, 0.0]
optimizer.amoeba.init_simplex[1] =  [3.5, 4.0, 4.5, 5.0, 0.0]
optimizer.amoeba.init_simplex[2] =  [4.5, 5.0, 5.5, 6.5, 0.0]
optimizer.amoeba.init_simplex[3] =  [3.0, 3.5, 4.0, 4.5, 0.0]
optimizer.amoeba.init_simplex[4] =  [5.0, 5.5, 6.5, 7.0, 0.0]

#print "value is ", optimizer.amoeba.init_simplex
