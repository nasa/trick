
#ifndef GNUPLOT_COLORS_HH
#define GNUPLOT_COLORS_HH

typedef struct {
   const char * color ;
   const char * hex_color ;
   double red ;
   double green ;
   double blue ;
} COLOR_TRANS ;

extern const COLOR_TRANS color_trans[];
extern const int num_colors;

int color_is_valid( std::string* color );
int color_is_valid( char* color );
int convert_to_hexcolor( std::string* color );
int convert_to_hexcolor( char* color );
int convert_to_rgbcolor( std::string* color );
int convert_to_rgbcolor( char* color );

#endif
