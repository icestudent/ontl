
$columns = 16;

foreach $arg (0 .. $#ARGV)
{
  $file = $ARGV[$arg];
  print "Processing file `$file'\n";
  open(IN, $file) or die "Can't open file: $!";
  binmode(IN);

  open(OUT, ">$file.c") or die "Can't open output file: $!";
  $file =~ s/\./_/g;

  print OUT "extern unsigned char $file [] =\n{\n";
  $size = 0;

  $column = 0;
  while ( ! eof(IN) )
  {
    ++$size;
    print  OUT '    '   unless $column; 
    printf OUT "0x%02X, ", ord(getc(IN));
    $column = 0         if ++$column >= $columns;
    print OUT "\n"      unless $column;
  }
  print OUT "\n};\n";
  print OUT "extern unsigned $file"."_size = $size;";

  close(OUT);
  close(IN);
}
