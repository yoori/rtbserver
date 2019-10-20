#!/usr/bin/perl

use strict;
use warnings;

my $server_path = defined($ENV{'server_root'}) ? $ENV{'server_root'} : $ENV{'TEST_TOP_SRC_DIR'};
my $data_path = defined($ARGV[0]) ? $ARGV[0] : ".";
my $tmp_path = defined($ARGV[1]) ? $ARGV[1] : "./ConfigureUserChunksTest.tmp/";

if (!defined $server_path)
{
  print "Shell variable 'server_root' should be declared\n";
  exit 1;
}

my $user_info_configurator = "PERL5LIB=\$PERL5LIB:$server_path/CMS/Plugin/lib " .
  "$server_path/CMS/Plugin/exec/bin/ConfigureChunks.pl";

sub get_file_structure
{
  my ($process_folder) = @_;
  my $transcoded_folder = $process_folder;

  $transcoded_folder =~ s|\.|\\\.|g;
  $transcoded_folder =~ s|\/|\\\/|g;

  my $command = "find " . $process_folder . " | sort | sed 's/\\.[0-9]\\{8\\}\\.data/\\.data/g' |"
    . " sed 's/\\.[0-9]\\{8\\}\\.index/\\.index/g' | sed 's/" . $transcoded_folder . "//g'";

  my @files = split(/\n/, `$command`);
  return @files;
}

sub check_test_case
{
  my ($test_case) = @_;

  my $base_folder = "$tmp_path/Base";
  my $process_folder = "$tmp_path/Work";

  my $clean_command = "rm -rf $base_folder $process_folder";
  system($clean_command);

  system("mkdir -p $tmp_path && cp -r $test_case->{folder}/Src $base_folder && " .
    "cp -r $base_folder $process_folder");

  my $test_name = $test_case->{folder};
  my $new_conf = $test_case->{folder} . "/UIChunksConfig";
  my $old_conf = $test_case->{folder} . "/OldUIChunksConfig";

  {
    my $command = "$user_info_configurator check " .
      "--chunks-root=Users1 " .
      "--transport 'test:$process_folder' " .
      "--modifier 'UserInfo::Modifier' " .
      "-v " .
      "--xml-out $process_folder/distribution.xml " .
      $test_case->{args} . " 2>&1";

    my $ret = `$command`;
    my $ret_code = $? >> 8;
#   print $ret;

    if($ret_code < 0)
    {
      print STDERR "Error($test_name): configure script exit code = $?: '$command'.\n";
      return 1;
    }

    if($ret_code != $test_case->{check_result})
    {
      print STDERR "Error($test_name): incorrect check result: $ret_code instead " .
        $test_case->{check_result} . ": command = '$command'\n";
      return 0;
    } 

    # check that 'check' has no affect to source folder
    my $diff_command = "diff -qr -x distribution.xml $base_folder $process_folder 2>&1";
    my $diff_ret = system($diff_command);
    if($diff_ret)
    {
      print STDERR "Error($test_name): found changes after check command: " .
        "diff command = '$diff_command'\n";
      return 0;
    }
  }

  my $expected_error = exists $test_case->{error} && $test_case->{error} > 0 ? 1 : 0;

  {
    # do redistribution and check result by etalon
    my $command = "PATH=\$PATH:$server_path/build/bin && $user_info_configurator reconf " .
      "--chunks-root=Users1 " .
      "--transport 'test:$process_folder' " .
      "--modifier 'UserInfo::Modifier' " .
      "-v " .
      $test_case->{args} . " 2>&1";

    my $ret = system($command);
    if($ret && $expected_error == 0)
    {
      print STDERR "Error($test_name): configure script exit code = $ret: '$command'.\n";
      return 0;
    }
    elsif($expected_error == 1 && $ret == 0)
    {
      print STDERR "Error($test_name): expected error exit code, but received success: '$command'.\n";
      return 0;
    }

    if(exists $test_case->{compare_file_structure} && $test_case->{compare_file_structure} > 0)
    {
      my @list_src = get_file_structure($process_folder);
      my @list_dst = get_file_structure($test_case->{folder} . "/Tgt");
      
      my $not_equal = 0;
      if ($#list_src != $#list_dst)
      {
        $not_equal = 1;
      }
      else
      {
        foreach my $i (0 .. $#list_src)
        {
          if ($list_src[$i] ne $list_dst[$i])
          {
            $not_equal = 1;
            print STDERR "The differnce has been found: $list_src[$i], $list_dst[$i]\n";
            last;
          }
        }
      }

      if ($not_equal)
      {
        print STDERR "Error($test_name): file structures are not identical: '$process_folder' '$test_case->{folder}/Tgt'\n";
        return 0;
      }
    }
    else
    {
      my $diff_command = "diff -x distribution.xml -qr $process_folder $test_case->{folder}/Tgt 2>&1";
      $ret = system($diff_command);
      if($ret)
      {
        print STDERR "Error($test_name): found difference, '$diff_command'\n";
        return 0;
      }
    }
  }

  if($expected_error == 0)
  {
    # check that result isn't require redistribution
    my $command = "$user_info_configurator check " .
      "--chunks-root=Users1 " .
      "--transport 'test:$process_folder' " .
      "--modifier 'UserInfo::Modifier' " .
      "-v " .
      "--xml-out $process_folder/distribution.xml " .
      $test_case->{args} . " 2>&1";

    my $ret = `$command`;
    my $ret_code = $? >> 8;
#   print $ret;

    if($ret_code < 0)
    {
      print STDERR "Error($test_name): configure script exit code = $?: '$command'.\n";
      return 1;
    }

    if($ret_code != 0)
    {
      print STDERR "Error($test_name): incorrect check result after redistribution " .
        "(redistribution required): " .
        "command = '$command'\n";
      return 0;
    } 
  }

  system($clean_command);

  return 1;
}

# MAIN
  my $unpack_etalons_command =
    "rm -rf $tmp_path/Etalons/ 2>/dev/null ; " .
    "mkdir -p $tmp_path/Etalons/ && " .
    "tar -xf $data_path/uim_etalons.tar.bz2 -j -C $tmp_path/Etalons/";

  system($unpack_etalons_command) &&
    die "can't unpack etalons: '$unpack_etalons_command'";

  my @test_cases = (
     { folder => "$tmp_path/Etalons/ChangeChunksNum1",
       args => '--chunks-count=20 --target-hosts=A,B --check-hosts=C --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/ChangeChunksNum2",
       args => '--chunks-count=20 --target-hosts=A,B --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/ChangeChunksNumUpgrade",
       args => '--chunks-count=20 --target-hosts=A,B --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/ChangeChunksNumContinuation1",
       args => '--chunks-count=20 --target-hosts=A,B --check-hosts=C --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/ChangeChunksNumContinuation2",
       args => '--chunks-count=20 --target-hosts=A,B --check-hosts=C --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/NewCluster",
       args => '--chunks-count=10 --target-hosts=A,B --force=1', check_result => 1 },
     { folder => "$tmp_path/Etalons/NoChangesEtalon", args => '--chunks-count=10 --target-hosts=A,B', check_result => 0 },
     { folder => "$tmp_path/Etalons/Etalon1", args => '--chunks-count=10 --target-hosts=A --check-hosts=B', check_result => 1 },
     { folder => "$tmp_path/Etalons/Etalon2", args => '--chunks-count=10 --target-hosts=A,B', check_result => 1 },
     { folder => "$tmp_path/Etalons/Etalon3", args => '--chunks-count=10 --target-hosts=A,B,C', check_result => 1 },
     { folder => "$tmp_path/Etalons/Etalon4", args => '--chunks-count=10 --target-hosts=A,B --force', check_result => 1 },
     { folder => "$tmp_path/Etalons/Etalon5", args => '--chunks-count=10 --target-hosts=A,B', check_result => 1, error => 1 },
     { folder => "$tmp_path/Etalons/CompatibilityEtalon",
       args => '--chunks-count=10 --target-hosts=nnode0,nnode1,nnode2,nnode3 --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/Migration_2.3_3.4",
       args => '--chunks-count=10 --target-hosts=A --check-hosts=B --force', check_result => 1 , compare_file_structure => 1},
     { folder => "$tmp_path/Etalons/CheckHostsEtalon",
       args => '--chunks-count=10 --target-hosts=A,B --check-hosts=C', check_result => 1 },
     { folder => "$tmp_path/Etalons/ErrorDetectionEtalon",
       args => '--chunks-count=10 --target-hosts=A,B --force', check_result => 1, error => 1 , compare_file_structure => 1},
  );

  my $ret = 1;

  foreach my $test_case(@test_cases)
  {
    print "========= " . $test_case->{folder} . " =========\n";
    $ret = $ret && check_test_case(\%$test_case);
  }

  if($ret)
  {
    print "Test Success.\n";
  }
  else
  {
    die "Test Detected Errors\n";
  }

0;
