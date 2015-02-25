#!/usr/bin/perl
# Copyright 2012, 2015 pooler@litecoinpool.org
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.  See COPYING for more details.
#
# nomacro.pl - expand assembler macros.

use strict;

foreach my $f (<*.S>) {
	rename $f, "$f.orig" unless -e "$f.orig";
	open FIN, "$f.orig";
	open FOUT, ">$f";
	my %macros = ();
	my %m = ();
	while (<FIN>) {
		if (m/^\.macro\s+(\w+)\s*(.*)$/) {
			$m{name} = $1;
			@m{args} = [split /\s*,\s*/, $2];
			$m{body} = "";
			next;
		}
		if (m/^\.endm/) {
			$macros{$m{name}} = {%m};
			%m = ();
			next;
		}
		for my $n (keys %macros) {
			if (m/^\s*$n\b\s*(.*)$/) {
				my @a = split /\s*,\s*/, $1;
				$_ = $macros{$n}{body};
				for my $i (0 .. $#{$macros{$n}{args}}) {
					s/\\$macros{$n}{args}[$i]\b/$a[$i]/g;
				}
				last;
			}
		}
		if (%m) {
			$m{body} .= $_;
			next;
		}
		print FOUT;
	}
	close FOUT;
	close FIN;
}
