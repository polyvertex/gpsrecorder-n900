#!/usr/bin/perl -w
#
# Author     : Jean-Charles Lefebvre
# Created On : 2010-03-10 19:49:54
#
# $Id$
#

use strict;
use warnings;


#----------------------------------------------------------------------------
# Variables
#----------------------------------------------------------------------------
my $b_alwaysyes = 0;
my $b_include_altm = 0;



#----------------------------------------------------------------------------
# path_basename
#----------------------------------------------------------------------------
sub path_basename($)
{
  my $path = shift;
  my $res;

  if (($res = rindex($path, '/')) >= 0 or
      ($res = rindex($path, '\\')) >= 0)
  {
    return substr($path, $res + 1);
  }

  return $path;
}

#----------------------------------------------------------------------------
# path_replace_extension
#----------------------------------------------------------------------------
sub path_replace_extension($$)
{
  my ($path, $new_extension) = @_;
  my $res;
  my $dirname;
  my $basename;

  if (($res = rindex($path, '/')) >= 0 or
      ($res = rindex($path, '\\')) >= 0)
  {
    $dirname  = substr($path, 0, $res + 1);
    $basename = substr($path, $res + 1);
  }
  else
  {
    $dirname  = '';
    $basename = $path;
  }

  if (($res = rindex($basename, '.')) > 0)
  {
    $basename = substr($basename, 0, $res + 1) . $new_extension;
  }
  else
  {
    $basename .= ".$new_extension";
  }

  return $dirname . $basename;
}



#----------------------------------------------------------------------------
# convert_csv2kml
#----------------------------------------------------------------------------
sub convert_csv2kml($$)
{
  my ($srcfile, $destfile) = @_;
  my $line;
  my $lineid = 0;
  my @lines;

  # fix;devstatus;satview;satuse;mode;fields;time;ept;latitude;longitude;eph;altitude;epv;track;epd;speed;eps;climb;epc
  # fix;fx;11;6;3d;0x3F;1268201730.000000;0.000000;48.859874;2.309336;1614.000000;110.000000;29.500000;272.080000;359.990000;0.216000;2.268000;-0.110000;1.250000
  my @fixkeys = ('fix','devstatus','satview','satuse','mode','fields','time','ept','latitude','longitude','eph','altitude','epv','track','epd','speed','eps','climb','epc');


  open(FH, $srcfile) or die "Could not open source file '$srcfile' !";
  while ($line = <FH>)
  {
    ++$lineid;

    chomp $line;       # rtrim
    $line =~ s/^\s+//; # ltrim

    next unless length($line) > 0;
    next unless substr($line, 0, 4) eq 'fix;';

    my @linevalues = split(/\s*;\s*/, $line);
    if (scalar(@linevalues) != scalar(@fixkeys))
    {
      warn "$srcfile($lineid): Incorrect numbers of fields (".scalar(@linevalues)." != ".scalar(@fixkeys).")\n";
      next;
    }

    my %fixfields;
    @fixfields{@fixkeys} = @linevalues;

    next unless
      $fixfields{devstatus} eq 'fx' or # fixed
      $fixfields{devstatus} eq 'dx';   # dgps fixed (deprecated)

    next unless
      $fixfields{mode} eq '2d' or
      $fixfields{mode} eq '3d';

    next unless
      $fixfields{satuse} > 0;

    $fixfields{fields} = hex $fixfields{fields}
      if $fixfields{fields} =~ /^0x[A-Fa-f0-9]+\z/;

    next unless (int($fixfields{fields}) & (1 << 4)) != 0; # is latlong set ?
    next unless (int($fixfields{fields}) & (1 << 5)) != 0; # is time set ?

    push(@lines, \%fixfields);
  }
  close FH;


  open(FH, "> $destfile") or die "Could not open destination file '$destfile' !";
  print FH
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n".
    "<kml xmlns=\"http://earth.google.com/kml/2.0\">\n".
    "<Folder>\n".
    "  <name>GpsRecorder Tracks</name>\n".
    "  <visibility>1</visibility>\n".
    "  <open>1</open>\n".
    "  <Placemark>\n".
    "    <description></description>\n".
    "    <name>".path_basename($srcfile)." - ".scalar(@lines)." pts</name>\n".
    "    <visibility>1</visibility>\n".
    "    <open>1</open>\n".
    "    <Style>\n".
    "      <LineStyle>\n".
    "        <color>FF00FF00</color>\n". # green
    "        <width>1</width>\n".
    "      </LineStyle>\n".
    "    </Style>\n".
    "    <LineString>\n".
    "      <extrude>".($b_include_altm ? '1' : '0')."</extrude>\n".
    "      <tesselate>1</tesselate>\n".
    "      <altitudeMode>".($b_include_altm ? 'absolute' : 'clampToGround')."</altitudeMode>\n". # clampToGround, relativeToGround, absolute
    "      <coordinates>\n";
  while (@lines > 0)
  {
    my $ref_fixfields = shift @lines;
    my $has_altm = !$b_include_altm ? 0 : (int($ref_fixfields->{fields}) & (1 << 0)) != 0; # is altitude set ?
    my $altm = $has_altm ? $ref_fixfields->{altitude} : 1;

    print FH $ref_fixfields->{longitude}.",".$ref_fixfields->{latitude}.",$altm\n";
  }
  print FH
    "      </coordinates>\n".
    "    </LineString>\n".
    "  </Placemark>\n".
    "</Folder>\n".
    "</kml>\n";
  close FH;
}



#----------------------------------------------------------------------------
# usage
#----------------------------------------------------------------------------
sub usage
{
  die <<USAGE;
Usage :
  $0 [OPTIONS] {FILES}

Description :
  * Converts every specified files into KML format.

Options :
  -y,--yes
    Overwrite destination files silently.

USAGE
}

#----------------------------------------------------------------------------
# m a i n
#----------------------------------------------------------------------------
my %files;
my $srcfile;
my $destfile;

$| = 1;
usage if @ARGV == 0;

# parse parameters
while (@ARGV > 0)
{
  my $arg = shift @ARGV;

  if ($arg =~ /^\-\-?(\w+)\z/)
  {
    my $optname = $1;

    if ($optname eq 'y' or $optname eq 'yes')
    {
      $b_alwaysyes = 1;
    }
    elsif ($optname eq 'a' or $optname eq 'alt' or $optname eq 'altm')
    {
      $b_include_altm = 1;
    }
    else
    {
      die "Unknown option '$arg' !";
    }
  }
  else # arg is a file to convert
  {
    $files{$arg} = path_replace_extension($arg, 'kml');
  }
}

# first pass : we do not overwrite any file so check it out
while (($srcfile, $destfile) = each %files)
{
  die "Invalid file name '$srcfile' !" if $srcfile eq '.' or $srcfile eq '..';
  die "File '$srcfile' does not exist or is not a file !" unless -f $srcfile;

  die "Output file '$destfile' already exists (source file '$srcfile') !"
    if !$b_alwaysyes and -e $destfile;
}
keys %files; # reset


# second pass : convert !
while (($srcfile, $destfile) = each %files)
{
  print
    "Convert $srcfile\n",
    "     to $destfile...\n";

  convert_csv2kml($srcfile, $destfile);
}
keys %files; # reset


print "\n".scalar(keys(%files))." files converted.\n";
exit 0;
