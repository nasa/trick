/* Used for dp test */

int extGetNextRecord(double *in, int numIn, double *out, int numOut) ;

int extGetNextRecord(double *in, int numIn __attribute__ ((unused)), double *out, int numOut __attribute__ ((unused)))
{
        out[0] = in[0] + in[1] + in[2];
        return (0);
}
