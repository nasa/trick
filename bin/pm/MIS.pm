package MIS ;

# $Id: MIS.pm 2384 2012-05-14 20:58:18Z alin $

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(mis_c mis_all_c process_args);

use strict ;
use lib $ENV{"TRICK_HOME"} . "/bin/pm" ;
use File::Basename ;
use Data::Dumper ;
use Cwd ;
use Cwd 'abs_path' ;
use html ;
use mis_dep ;
use trick_print ;
use auto_doc ;

my @includes ;
my %this_dir ;
my $rel_path_curr_file ;
my @files_to_mis_dep ;
my @class_names = qw( initialization restart derivative 
                      integration dynamic_event post_integration
                      automatic environment asynchronous_mustfinish asynchronous
                      random  sensor  sensor_emitter sensor_reflector
                      sensor_receiver  scheduled effector  effector_emitter
                      effector_receiver automatic_last malfunction 
                      malfunction_trigger logging shutdown freeze_init freeze
                      unfreeze checkpoint pre_load_checkpoint
                      monte_master_init monte_master_pre 
                      monte_master_post monte_master_shutdown 
                      monte_slave_init monte_slave_pre 
                      monte_slave_post monte_slave_shutdown 
                      N/A) ;

sub mis_all_c($$) {
    my ( $all_src_files_ref , $sim_ref ) = @_ ;
    my ( @all_lines , $contents ) ;
    my ( $file_path_dir , $xml_file ) ;
    my ( $name , $path ) ;
    my ( $xml_time , $src_time ) ;
 
    foreach my $f ( @$all_src_files_ref ) {
        # determine the xml file name
        $file_path_dir = dirname($f) ;
        ($name, $path) =  fileparse($f, ("h" , "c" , "f", "cpp" , "cxx" , "C" , "cc" , "c\\+\\+" ));
        $file_path_dir =~ s/\/src$// ;
        $xml_file = $file_path_dir . "/xml/" . $name . "xml" ;
  
        # skip this file if the xml file is newer than the source file
        if ( -e $xml_file ) {
                $xml_time = (stat $xml_file)[9] ; 
                $src_time = (stat $f)[9] ;
                next if ( $xml_time > $src_time ) ;
        }
  
        # read in the file and MIS parse it
        open FILE , "$f" ;
        @all_lines = <FILE> ;
        $contents = join "" , @all_lines ;
        mis_c( $f , $contents , $sim_ref ) ;
    }
    return ;
}

sub mis_c($$$) {

    my ($curr_file , $contents, $sim_ref) = @_ ;
    my ($object_file , @entry_names);
    my (@comments , @entries ) ;
    my ($num_valid_entries ,$num_args_left) ;
    my @all_lines ;
    my $nm ;
    my %header ;
    my $i ;
    my $entry ;
    my @split_args ;
    my $language ;
 
    trick_print($$sim_ref{fh}, "   Processing $curr_file\n", "debug_cyan" , $$sim_ref{args}{v}) ;
 
    ($object_file = $curr_file) =~ s/(?:src\/)?(\w+)\.c$/object_$ENV{"TRICK_HOST_CPU"}\/$1.o/ ;
 
    ($rel_path_curr_file) = $curr_file =~ /([^\/]+)$/ ;
 
    %header = extract_trick_header($curr_file , $contents , $sim_ref , 0 );
    
    if ( exists $header{language} ) {
        # if the language specified is c++ cpp cxx c<something> then set the language as cpp
        if ( $header{language} =~ /c[\w\+]+/i ) {
            $language = "CPP" ;
        }
        else {
            $language = "C" ;
        }
    }
    elsif ($curr_file =~ /\.c$/ ) {
        # else if the current file ends in .c set the language to c
        $language = "C" ;
    }
    else {
        # all other extensions are cpp
        $language = "CPP" ;
    }
 
    #Pull out comments
    $i = 0;
    while ($contents =~ /(?:\/\*(.*?)\*\/)|(?:\/\/(.*?)\n)/sg) {
        push @comments , $+ ;
    }
    $contents =~ s/\/\*(.*?)\*\/|\/\/(.*?)(\n)/" " . $i++ . " " . $3/esg ;
    
    @includes = $contents =~ /(#include\s*["<][^">]+[">])/g  ;

    # The next loop finds all of the function entry points in the file.
    while ( $contents =~ /
                     (?:(auto|register|static|extern)\s+)?                                           # optional keywords = $1
                     (?:(enum|class|struct|const|volatile|signed|unsigned|long|unsigned\s+long)\s+)? # optional keywords = $2
                     (\#?[A-Za-z]\w*|(?:[A-Za-z]\w*\s*<\s+[A-Za-z]\w*\s+>))                # type name = $3
                     ([\*\s]+)                    # pointers or spaces = $4 (at least 1 space or pointer)
                     (\w[\:\-\w]*)\s*             # function name = $5
                     \(\s*                        # open paren         
                     ([^)]*)                      # everything to close paren = $6
                     \)                           # close paren        
                     \s*(\d+)?\s*                 # optional comment = $7
                     (?:(const|volatile)\s+)?\s*  # optional const|volatile = $8
                     (throw\s*\(.*?\))?\s*        # optional throw
                     (?:\d+\s*)*                  # extra comments discarded
                     {                            # open brace , balance }
                   /sgx ) {
                           

        my $return_type = $1 ;
        $return_type .= ( $return_type eq "" ) ? $2 : " $2" ;
        $return_type .= ( $return_type eq "" ) ? $3 : " $3" ;
        my $pointers = $4 ;
        if ( $pointers ne "" ) {
            $pointers =~ s/(^\s+|\s+$)//g ;# remove leading & trailing whitespaces
            $pointers = " $pointers" ;
        }
        $return_type .= $pointers ;
        my $name = $5 ;
        my $raw_args = $6  ;
        $raw_args .= " $7" if ( $7 ne "" ) ;
  
        # skip this entry point because it's actulally a macro
        next if ( $return_type =~ /\#/ ) ;

        if ( $name eq "if" or $name eq "for" or
             $name eq "while" or $name eq "switch" or
             $name eq "do" or $name =~ /operator[^\w]/ ) {
            next ;
        }
        else {
            push @entries , { 'return_type' => $return_type ,
                                     'name' => $name ,
                                'file_name' => $curr_file ,
                                 'raw_args' => $raw_args ,
                                 'language' => $language } ;
        }
    }

    $num_valid_entries = 0 ;
 
    foreach $entry (@entries) {
        trick_print($$sim_ref{fh}, "      Processing $$entry{name}\n" , "debug_cyan" , $$sim_ref{args}{v}) ;
        if ( process_args ($entry, \@comments, 6 , $sim_ref) == 0 ) {
            add_proto_to_cat( $entry , $sim_ref , 6 ) ;
            $num_valid_entries++;
        }
    }
 
    if ( $num_valid_entries eq 0 ) {
        trick_print($$sim_ref{fh}, "      WARNING: No entry points found\n", "debug_yellow" , $$sim_ref{args}{v}); 
    }
    else {
        print_xml_mis($curr_file , \@entries , \%header, \@includes) ;
    }
   
}

sub process_args($$$$) {

    my ($entry, $comments, $indent , $sim_ref) = @_ ;
    my ($entry_comment , $hdr_comment ) ;
    my @arg_list ;
 
    $hdr_comment = -1 ;
 
    $$entry{valid} = 0 ;
 
    # clear out leading spaces
    $$entry{raw_args} =~ s/^\s+//s ;
 
    $$entry{raw_args} =~ s/\s+(\*+)/$1 /g ; 
    if ($$entry{raw_args} =~ s/^\s*(\d+)\s*//s ) {
        $entry_comment = $1 ;
        $hdr_comment = $entry_comment - 1 ;
        ($$entry{units} , $$entry{desc}) = 
                @$comments[$entry_comment] =~ /\s*RETURN:\s*(?:((?:[-*\/\w^|\.]+)|(?:\(.*?\))))\s+\s*(.*)/six ;
    }
    else {
        $$entry{units} = "--" ;
        $$entry{desc} = "Not Specified" ;
    }
    $$entry{raw_args} =~ s/^\s*void\s*$// ;
 
    $$entry{class} = "" ;
    while ( $$entry{class} eq "" and $hdr_comment >= 0 ) {
        ($$entry{class}) = @$comments[$hdr_comment] =~ /CLASS:.*?\(\s*([\w\/]+)/si ;
        $hdr_comment -= 1 ;
    }
    if ( !(grep /^$$entry{class}$/ , @class_names )) {
        $$entry{class} = "N/A" ;
    }

    my $arg_count = 1 ;
    while ( $$entry{raw_args} =~ s/^(?:
                                (?:
                                 ((?:(?:enum|class|struct|const|volatile|signed|unsigned|long|unsigned\s+long)\s+)?.*?) # argument type = $1
                                 (<.*?>)?\s*                    # c++ template = $2
                                 (\**)\s+                       # argument pointers = $3
                                 (?:(const)\s+)?\s*             # c++ const keyword = $4 
                                 (\&)?\s*                       # c++ reference symbol = $5 
                                 (\w+)?\s*                      # name (optional in c++ method decl.) = $6
                                 (\[[^,=]*\])?\s*               # optional array specs = $7
                                 (\s*=\s*[\w\.]+)?[\s,]*        # optional default value = $8
                                )
                                |
                                (\.\.\.)\s*                     # variable argument spec = $9
                               )
                               (\d+)?\s*                        # in|inout|out comment = $10
                               (?:\d+\s*)*                      # optional extra comments discarded
                             //six ) {
        my %func_arg ;
        my $comment_index ;
  
        $func_arg{type} = $1 ;
        $func_arg{cpp_template} = ( $2 ne "" ) ? $2 : "" ;
        $func_arg{pointers} = ( $3 ne "" ) ? $3 : "" ;
        $func_arg{cpp_const} = $4 ;
        $func_arg{cpp_ref} = $5 ;
  
        $func_arg{name} = ( $6 ne "" ) ? $6 : "" ;
        $func_arg{array} = ( $7 ne "" ) ? $7  : "" ;
        $func_arg{optional} = ( $8 ne "" ) ? 1 : 0 ;
        $func_arg{type} = $9 if ( $9 ne "" ) ;
        $comment_index = $10 ;
  
        if ( $comment_index ne "" ) {
            ($func_arg{comment} = @$comments[$comment_index]) =~ s/(IN|INOUT|OUT):\s*// ;
            ( $func_arg{inout} , $func_arg{unit} , $func_arg{desc} ) = 
                    @$comments[$comment_index] =~ /^\s*(?:(IN|INOUT|OUT):)?\s*
                                                    (?:((?:[-*\/\w^|\.]+)|(?:\(.*?\))))\s+
                                                    \s*(.*)
                                                    /six ;
        }
        else {
            $func_arg{comment} = "-- Not specified" ;
            $func_arg{inout} = "" ;
            $func_arg{unit} = "--" ;
            $func_arg{desc} = "Not specified" ;
        }
  
        trick_print($$sim_ref{fh}, " "x($indent+3) . "Arg $arg_count\n" , "debug_white" , $$sim_ref{args}{v} ) ;
        trick_print($$sim_ref{fh}, " "x($indent+6) . "type = $func_arg{type} $func_arg{pointers} $func_arg{array}\n" ,
         "debug_white" , $$sim_ref{args}{v} ) ;
        trick_print($$sim_ref{fh}, " "x($indent+6) . "name = $func_arg{name}\n" , "debug_white" , $$sim_ref{args}{v} ) ;
        trick_print($$sim_ref{fh}, " "x($indent+6) . "comm = $func_arg{comment}\n" , "debug_white" , $$sim_ref{args}{v} ) ;
        $arg_count++ ;
  
        push @arg_list , \%func_arg ;
    }

    if ( $$entry{raw_args} ne "" ) {
        trick_print($$sim_ref{fh}, "Could not process argument for $entry->{name}\n" , "title_red" , $$sim_ref{args}{v} ) ;
        trick_print($$sim_ref{fh}, "Last Read:\n$entry->{args_list}\n" , "title_red" , $$sim_ref{args}{v} ) ;
        return -1 ;
    }
    else {
 
        # set the values to return
        $$entry{valid} = 1 ;
        @{$$entry{arg_list}} = @arg_list ;
    }
 
    return 0 ;

}

1;
