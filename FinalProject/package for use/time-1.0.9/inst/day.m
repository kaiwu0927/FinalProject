## Copyright (C) 2008 Bill Denney
##
## This software is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## This software is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this software; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {Function File} {dom =} day (Date)
##
## Returns the day of the month from a serial date number or a date
## string.
##
## @seealso{date, datevec, now, month, year}
## @end deftypefn

## Author: Bill Denney <bill@denney.ws>
## Created: 21 Jan 2008

function t = day (dates)

  t = datevec (dates);
  t = t (:,3);

endfunction

