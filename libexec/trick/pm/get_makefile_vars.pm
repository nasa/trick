package get_makefile_vars;

use Cwd;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(dump_makefile_vars);

use strict;
use warnings;

#Utility to dump specified variables from a makefile
sub dump_makefile_vars($$$$@)
{
    my ($file, $build_dir, $env_ref, $build_dir, @vars) = @_;

    open my $fh, ">", "${build_dir}/get_vars.mk" or die "Could not open ${build_dir}/get_vars.mk\n";

    print $fh "include $file\n\n";
    print $fh "print_vars:\n";
    print $fh "\t\@> ${build_dir}/var_dump.mk\n";
    foreach my $var (@vars)
    {
        print $fh "\t\@echo $var += \$($var) >> ${build_dir}/var_dump.mk\n";
    }
    close $fh;
    
    {
        local %ENV = %$env_ref;
        $ENV{'AM_I_TRICKIFYING'} = 1;
        system("make -f ${build_dir}/get_vars.mk print_vars");
    }

    my @validlines;
    open (my $fi, "${build_dir}/var_dump.mk") or die "Could not open ${build_dir}/var_dump.mk: $!" ;

    my $cwd = getcwd ;
    while (my $line = <$fi>) 
    {
        chomp $line;
        if(($line =~ /\Q+=\E$/) ne 1)
        {
            #Identify instances of CWD, and replace with the path of the S_define
            my $b4_line = $line;
            $line =~ s/$build_dir/$cwd/g;
            push @validlines, $line;
        }
    }

    open my $fj, ">", "${build_dir}/var_dump.mk" or die "Could not open ${build_dir}/var_dump.mk\n";
    foreach my $line (@validlines)
    {
        print $fj "$line\n";
    }
}

1;
