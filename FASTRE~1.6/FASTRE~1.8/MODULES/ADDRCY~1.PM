# A circular list of the IP addresses configured on the local machine.
# Written by David MacKenzie <djm@web.us.uu.net>
# Please send comments and bug reports to fastresolve-bugs@web.us.uu.net.

##############################################################################
#   Copyright 1999 UUNET, an MCI WorldCom company.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
##############################################################################

package AddrCycle;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(new next_addr cycle_length);

# Return one of the local machine's configured IP addresses, round-robin,
# in ASCII form.
sub next_addr
{
    my($self) = @_;
    my($addr);

    $addr = $Addrs[$self->{indx}];
    if (++$self->{indx} >= @Addrs) {
	$self->{indx} = 0;
    }
    return $addr;
}

# Return the number of IP addresses in the list.
sub cycle_length
{
    return scalar(@Addrs);
}

sub add_addr
{
    my($addr, $weedout) = @_;

    return if ($addr eq "127.0.0.1");
    return if ($weedout && $addr =~ $weedout);
    push(@Addrs, $addr);
}

sub init
{
    my($weedout) = @_;
    my($iflist);

    @Addrs = ();
    $iflist = `/sbin/ifconfig -a`;
    $iflist =~ s/\sinet\s[a-zA-Z_:]*\s*([\d.]+)\s/&add_addr($1, $weedout)/ges;
    die "AddrCycle: No inet interfaces configured" unless @Addrs;
}

# Optional arg is a regex to match local IP addresses to skip.
sub new
{
    shift;
    my $self = bless {};
    $self->{indx} = 0;
    init(@_) unless defined @Addrs;
    return $self;
}

1;
