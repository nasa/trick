#include "tetrad.hh"

Tetrad::Tetrad(double* in_pos, double in_angle, double** in_vertices, int in_vertix_count)
{
	if(in_vertix_count < 3 || in_vertix_count > 12)
	{
		//error: not a valid shape
	}
	else
	{
		pos[0] = in_pos[0];
		pos[1] = in_pos[1];

		angle = in_angle;

		vertix_count = in_vertix_count;

		for(int itr = 0; itr < vertix_count; ++vertix_count)
		{
			vertices[itr] = in_vertices[itr];
		}	
	}

	calc_center_of_mass();
}

//not sure if valid
void Tetrad::calc_center_of_mass()
{
	for(int itr = 0; itr < vertix_count; ++itr)
	{
		com[0] += vertices[itr][0];
		com[1] += vertices[itr][1]; 
	}
	com[0] /= vertix_count;
	com[1] /= vertix_count;	
}
