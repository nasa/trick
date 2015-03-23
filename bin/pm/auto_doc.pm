package auto_doc;

# Note:
# Firefox 3 fails to process XSLT files if they are on the local hard drive plus
# outside of the current XML file's path (or descendant paths). Users may need
# to edit $HOME/.mozilla/firefox/*.default/prefs.js and add a new URI setting:
# - user_pref("security.fileuri.strict_origin_policy", false);
# OR
# - type about:config in the address bar
# - change security.fileuri.strict_origin_policy to false

use Exporter();
@ISA = qw(Exporter);
@EXPORT = qw(print_xml_CP print_xml_mis print_xml_icg);

use File::Basename;
use Text::Balanced qw ( extract_bracketed );
use strict;
use gte;

my(%default_data_files, %object_jobs, @object_names, %jobs, %object_structs, $vt_ref);

sub print_xml_CP($)
{
    my($sim_ref) = @_;
    my $title;

    # Get the title of the sim from the file "Title"
    if (open TITLE, "Title") {
        $title = <TITLE>;
        chomp $title;
    } else {
        $title = "No Title Specified";
    }

    open XML_FILE, ">S_document.xml" or die "Could not open S_document.xml\n";

    # Start the xml code
    print_xml_xml_header("$ENV{TRICK_HOME}/docs/trick_s_define.xsl");
    print XML_FILE "<s_define>\n";
    print XML_FILE " " x 4, "<title>$title</title>\n";

    # Given an array of simulation objects, sort array by 'objname' values
#    foreach my $o (sort { ${$a}{objname} cmp ${$b}{objname} }  @{$$sim_ref{objs}}) {
#
#        print XML_FILE " " x 4, "<sim_object>\n";
#
#        print XML_FILE " " x 8,
#            "<sim_object_name>$$o{objname}</sim_object_name>\n\n";
#
#        # printout the structures found in each object
#        print XML_FILE " " x 8, "<structs>\n";
#        # Given an array of structures per sim_object, sort array by 'strname' values
#        foreach my $s (sort { ${$a}{strname} cmp ${$b}{strname} }  @{$$o{structs}}) {
#            print XML_FILE " " x 12, "<struct>\n";
#            print XML_FILE " " x 16, "<struct_name>$$s{strname}</struct_name>\n";
#            $$sim_ref{icg_types}{$$s{strname}}{files}{$$s{file}}{xml} =~ s/^TRICK_HOME\//$ENV{TRICK_HOME}\// ;
#            print XML_FILE " " x 16, "<link>$$sim_ref{icg_types}{$$s{strname}}{files}{$$s{file}}{xml}</link>\n";
#            print XML_FILE " " x 16, "<default_data>\n";
#            foreach my $d (sort @{$$s{def_data}}) {
#                    print XML_FILE " " x 20, "<file>$d</file>\n";
#            }
#            print XML_FILE " " x 16, "</default_data>\n";
#            print XML_FILE " " x 12, "</struct>\n";
#        }
#        print XML_FILE " " x 8, "</structs>\n\n";
#
#        # print out each job found in this object
#        print XML_FILE " " x 8, "<jobs>\n";
#        # Given an array of jobs per sim_object, sort array by 'jobname' values
#        foreach my $j (sort { ${$a}{jobname} cmp ${$b}{jobname} }  @{$$o{jobs}}) {
#            my ($file_name, $model_dir, $suffix) =
#                fileparse( $$j{file}, ("\.h" , "\.hh" , "\.c" , "\.f", "\.cpp" , "\.cxx" , "\.C" , "\.cc" , "\.c\\+\\+" ));
#            ($model_dir = $$j{file}) =~ s/(\/src)?\/([^\/]+)$// ;
#            if ($model_dir =~ m/\/include$/ ){
#                  $model_dir =~ s/(\/include)?\/([^\/]+)$// ;
#            }
#            print XML_FILE " " x 12, "<job>\n";
#            print XML_FILE " " x 16, "<job_name>$$j{jobname}</job_name>\n";
#            print XML_FILE " " x 16, "<link>${model_dir}/xml/${file_name}${suffix}.xml</link>\n";
#            print XML_FILE " " x 12, "</job>\n";
#        }
#        print XML_FILE " " x 8, "</jobs>\n";
#        print XML_FILE " " x 4, "</sim_object>\n\n";
#    }

    print XML_FILE "</s_define>\n";

    close XML_FILE ;
}

sub print_xml_mis ($$$$) {

    my ($curr_file , $entries , $header_ptr, $includes) = @_ ;
    my ($out_file_basename , $name , $path , $suffix, $file_path_dir ) ;
    my %header = %$header_ptr ;
    my ($xml_file , $xml_dir , $file_version ) ;
    my ($subme , @inc_paths ) ;
    my $i ;

    $out_file_basename = $curr_file ;
    ($name, $path, $suffix) =
        fileparse( $curr_file, ("\.h" , "\.hh" , "\.c" , "\.f", "\.cpp" , "\.cxx" , "\.C" , "\.cc" , "\.c\\+\\+" ));

    $file_path_dir = dirname($curr_file) ;
    if ($file_path_dir eq ".") {
        $file_path_dir = cwd();
    }
    $file_path_dir =~ s/\/+$// ;                 # remove trailing slash
    $path =~ s/\/+$// ;                          # remove trailing slash

    if ($file_path_dir !~ /sim_services\/include/ and $file_path_dir =~ s/\/src$// ) {
        $xml_file = $file_path_dir . "/xml/" . $name . $suffix . ".xml" ;
        $xml_dir = $file_path_dir . "/xml" ;
    }
    else {
        $xml_file = $path . "/xml/" . $name . $suffix . ".xml" ;
        $xml_dir = $path . "/xml" ;
    }

    if ( ! -e $xml_dir ) {
        mkdir $xml_dir, 0775 ;
    }
    $file_version = $file_path_dir ;
    foreach $subme (@inc_paths) {
        if ($file_version =~ s,^$subme/,, ) {              # if found break out
            last ;
        }
    }

    open XML_FILE ,">$xml_file" or die "Couldn't open file for writing $xml_file\n" ;

    print_xml_xml_header( "$ENV{TRICK_HOME}/docs/trick_module.xsl") ;
    print_xml_body_header();
    print_xml_trick_header($curr_file, \%header);

    print XML_FILE " " x 4 , "<jobs>\n" ;
    foreach my $entry (@$entries) {
        if ( !exists $$entry{valid} ) {
            next ;
        }
        print XML_FILE " " x 8 , "<job>\n" ;
        print_xml_child("job_name", $$entry{name}, 12);
        print_xml_child("return_type", $$entry{return_type}, 12);
        print_xml_child("units", $$entry{units}, 12);
        print_xml_child("comment", $$entry{desc}, 12);
        print XML_FILE " " x 12 , "<argument_list>\n" ;
        foreach my $arg ( @{$$entry{arg_list}} ) {
            print XML_FILE " " x 16 , "<arg>\n" ;
            print_xml_child("arg_name", $$arg{name}, 20);
            print_xml_child("type", $$arg{type}, 20);
            print_xml_child("dims", "$$arg{pointers} $$arg{array}", 20);
            print_xml_child("inout", ucfirst lc $$arg{inout}, 20);
            print_xml_child("units", $$arg{unit}, 20);
            print_xml_child("comment", $$arg{desc}, 20);
            print XML_FILE " " x 16 , "</arg>\n" ;
        }
        print XML_FILE " " x 12 , "</argument_list>\n" ;
        print XML_FILE " " x 8 , "</job>\n" ;
    }
    print XML_FILE " " x 4 , "</jobs>\n" ;

    print XML_FILE " " x 4 , "<includes>\n" ;
    foreach (@$includes) {
        s/</&lt;/g;
        s/>/&gt;/g;
        s/"/&quot;/g;
        print XML_FILE " " x 8 , "<file>$_</file>\n";
    }
    print XML_FILE " " x 4 , "</includes>\n" ;

    print_xml_body_footer();

    close XML_FILE ;
}

sub print_xml_icg ($$$$$) {

    my ($file_name, $xml_file, $header, $enums_info, $structs_info) = @_ ;

    open XML_FILE ,">$xml_file" or die "Couldn't open file for writing $xml_file\n" ;

    print_xml_xml_header( "$ENV{TRICK_HOME}/docs/trick_struct.xsl") ;
    print_xml_body_header();
    print_xml_trick_header( $file_name, $header)  ;

    # print the enums
    print XML_FILE " " x 4 , "<enumerations>\n" ;
    foreach my $pe_ret ( @$enums_info ) {
        foreach my $enum ( @{$$pe_ret{enum_names}} ) {
            print XML_FILE " " x 8 , "<enum>\n" ;
            print XML_FILE " " x 12 , "<enum_name>$enum</enum_name>\n" ;
            foreach my $name_val ( @{$$pe_ret{enums}} ) {
                print XML_FILE " " x 12 , "<member>\n" ;
                print XML_FILE " " x 16 , "<mname>$$name_val{name}</mname>\n" ;
                print XML_FILE " " x 16 , "<value>$$name_val{value}</value>\n" ;
                $$name_val{comment} =~ s/&/&amp;/g ;
                $$name_val{comment} =~ s/\\?"/&quot;/g ;
                $$name_val{comment} =~ s/</&lt;/g ;
                $$name_val{comment} =~ s/>/&gt;/g ;
                print XML_FILE " " x 16 , "<comment>$$name_val{comment}</comment>\n" ;
                print XML_FILE " " x 12 , "</member>\n" ;
            }
            print XML_FILE " " x 8 , "</enum>\n" ;
        }
    }
    print XML_FILE " " x 4 , "</enumerations>\n\n" ;

    # print the structs
    print XML_FILE " " x 4 , "<structures>\n" ;
    foreach my $ps_ret ( @$structs_info ) {
        foreach my $str ( @{$$ps_ret{struct_names}} ) {
            print XML_FILE " " x 8 , "<struct>\n" ;
            print XML_FILE " " x 12 , "<struct_name>$str</struct_name>\n" ;
            foreach my $param ( @{$$ps_ret{param_info}} ) {
                print XML_FILE " " x 12 , "<member>\n" ;
                print XML_FILE " " x 16 , "<pname>$$param{p_name}</pname>\n" ;
                print XML_FILE " " x 16 , "<type>$$param{p_type}</type>\n" ;
                if ( $$param{p_link} ne "" ) {
                    $$param{p_link} =~ s/^TRICK_HOME\//$ENV{TRICK_HOME}\// ;
                    print XML_FILE " " x 16 , "<link>$$param{p_link}</link>\n" ;
                }
                if ( $$param{p_is_enum} ) {
                    print XML_FILE " " x 16 , "<is_enum>$$param{p_is_enum}</is_enum>\n" ;
                }
                print XML_FILE " " x 16 , "<dims>$$param{p_dim_xml}</dims>\n" ;
                print XML_FILE " " x 16 , "<units>$$param{p_unit}</units>\n" ;
                $$param{p_desc} =~ s/&/&amp;/g ;
                $$param{p_desc} =~ s/\\?"/&quot;/g ;
                $$param{p_desc} =~ s/</&lt;/g ;
                $$param{p_desc} =~ s/>/&gt;/g ;
                print XML_FILE " " x 16 , "<comment>$$param{p_desc}</comment>\n" ;
                print XML_FILE " " x 12 , "</member>\n" ;
            }
            print XML_FILE " " x 8 , "</struct>\n\n" ;
        }
    }
    print XML_FILE " " x 4 , "</structures>\n\n" ;

    print_xml_body_footer();
    close XML_FILE ;
}

sub print_xml_xml_header($)
{
    my ($xsl_file) = @_ ;
    print XML_FILE "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" ;
    print XML_FILE "<?xml-stylesheet type=\"text/xsl\" href=\"$xsl_file\"?>\n";

    print XML_FILE "\n" ;
    print XML_FILE "<!--Note:\n" ;
    print XML_FILE " " x 4 , "Firefox 3 fails to process XSLT files if they are on the local hard drive\n" ;
    print XML_FILE " " x 4 , "plus outside of the current XML file's path (or descendant paths). Users\n" ;
    print XML_FILE " " x 4 , "may need to edit \$HOME/.mozilla/firefox/*.default/prefs.js and add a new\n" ;
    print XML_FILE " " x 4 , "URI setting in order to properly view this file in a firefox browser:\n" ;
    print XML_FILE " " x 4 , "- user_pref(\"security.fileuri.strict_origin_policy\", false);\n" ;
    print XML_FILE " " x 4 , "OR\n" ;
    print XML_FILE " " x 4 , "- type about:config in the address bar\n" ;
    print XML_FILE " " x 4 , "- change security.fileuri.strict_origin_policy to false\n" ;
    print XML_FILE "-->\n" ;
    print XML_FILE "\n" ;
}

sub print_xml_body_header()
{
    print XML_FILE "<file>\n" ;
}

sub print_xml_body_footer()
{
    print XML_FILE "</file>\n" ;
}

sub print_xml_trick_header($$) {

    my ($full_file_name, $header) = @_ ;
    my ($tail_name, $path, $suffix) =
        fileparse( $full_file_name, ("\.h" , "\.hh" , "\.c" , "\.f", "\.cpp" , "\.cxx" , "\.C" , "\.cc" , "\.c\\+\\+" ));

    print_xml_child("file_name", "$tail_name$suffix", 4);
    print_xml_child("full_path_file_name", $full_file_name, 4);
    print XML_FILE " " x 4 , "<trick_header>\n" ;
    print_xml_references($header);
    print_xml_parent_children($header, "assumptions", "assumption", "assumptions and limitations");
    print_xml_parent_children($header, "requirements", "requirement", "requirements");
    print_xml_parent_children($header, "library_dependencies", "library_dependency", "libdep");
    print_xml_parent_children($header, "purpose", "", "purpose");
    print_xml_parent_children($header, "class", "", "class");
    print_xml_programmers($header, "programmers", "programmer", "programmers");
    print XML_FILE " " x 4 , "</trick_header>\n\n" ;
}

sub print_xml_parent_children($$$$) {
    #
    # Print:
    # <xml parent name>
    #       <children xml nodes>
    # </xml parent name>

    my ($header_ptr, $xml_parent_name, $xml_child_name, $trick_name) = @_ ;

    my %header = %$header_ptr ;

    print XML_FILE " " x 8 , "<$xml_parent_name>" ;
    if ( $xml_child_name ne "" ) {
        print XML_FILE "\n" ;
    }
    foreach my $h ( keys %header ) {
        my @list ;
        @list = split /\)[ \t\n]*\(/ , $header{$h} ;
        foreach my $ll ( @list ) {
            if ( $h eq $trick_name ) {
                print_xml_child($xml_child_name, $ll, 12);
            }
        }
    }
    if ( $xml_child_name ne "" ) {
        print XML_FILE " " x 8 , "</$xml_parent_name>\n" ;
    } else {
        print XML_FILE "</$xml_parent_name>\n" ;
    }
}

sub print_xml_child($$$) {

    # Print a child xml node
    # <child_name>
    #      value
    # </child_name>

    my ($xml_name, $value, $indent_level) = @_ ;

    $value =~ s/\(|\)//g ;
    $value =~ s/&/&amp;/g ;
    $value =~ s/\\?"/&quot;/g ;
    $value =~ s/</&lt;/g ;
    $value =~ s/>/&gt;/g ;
    $value =~ s/\n/ /g ;
    $value =~ s/[ ]+/ /g ;
    $value =~ s/\s\s+/ /g ;

    if ( $xml_name ne "" ) {
            print XML_FILE " " x $indent_level , "<$xml_name>" ;
    }

    print XML_FILE " $value " ;

    if ( $xml_name ne "" ) {
            print XML_FILE "</$xml_name>\n" ;
    }
}

sub print_xml_references($) {

    my ($header_ptr) = @_ ;
    my %header = %$header_ptr ;
    my $idx = 0 ;

    print XML_FILE " " x 8 , "<references>\n" ;

    my @list ;
    my ($temp , $ref) ;
    $temp = $header{"reference"} ;
    $temp =~ s/^\(// ;

    do {
        ($ref, $temp) = extract_bracketed($temp,"()");
        if ( $ref ne "" ) {
            @list = split /\)[ \t\n]*\(/ , $ref ;
            print XML_FILE " " x 12 , "<reference>\n" ;
            print_xml_child("author",   @list[0], 16) ;
            print_xml_child("source",   @list[1], 16) ;
            print_xml_child("ident",    @list[2], 16) ;
            print_xml_child("location", @list[3], 16) ;
            print_xml_child("date",     @list[4], 16) ;
            print_xml_child("notes",    @list[5], 16) ;
            print XML_FILE " " x 12 , "</reference>\n" ;
            $idx = $idx + 6 ;
        }
    } while ( $ref ne "" ) ;
    print XML_FILE " " x 8 , "</references>\n" ;

}

sub print_xml_programmers($) {

    my ($header_ptr) = @_ ;
    my %header = %$header_ptr ;
    my $idx = 0 ;

    print XML_FILE " " x 8 , "<modifications>\n" ;
    foreach my $h ( keys %header ) {
        my @list ;
        @list = split /\)[ \t\n]*\(/ , $header{$h} ;
        if ( $h eq "programmers" ) {
            while ( $idx <= $#list ) {
                print XML_FILE " " x 12 , "<modification>\n" ;
                print_xml_child("programmer", @list[$idx + 0], 16) ;
                print_xml_child("employer",   @list[$idx + 1], 16) ;
                print_xml_child("date",       @list[$idx + 2], 16) ;
                print_xml_child("ident",      @list[$idx + 3], 16) ;
                print_xml_child("notes",      @list[$idx + 4], 16) ;
                print XML_FILE " " x 12 , "</modification>\n" ;
                $idx = $idx + 5 ;
            }
        }
    }
    print XML_FILE " " x 8 , "</modifications>\n" ;
}


1;
