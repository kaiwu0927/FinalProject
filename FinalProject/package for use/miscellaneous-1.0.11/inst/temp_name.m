## Copyright (C) 2002 Etienne Grossmann.  All rights reserved.
##
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the
## Free Software Foundation; either version 2, or (at your option) any
## later version.
##
## This is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.

## -*- texinfo -*-
## @deftypefn{Function File} {@var{n} = } temp_name ( rootname, quick ) 
## @cindex  
## name = temp_name(rootname, quick=1) - Return a name that is not used
##
## Returns a name, suitable for defining a new function, script or global
## variable, of the form
##
##                 [rootname,number]
##
## Default rootname is "temp_name_"
##
## "quick" is an optional parameter, which defaults to 1. If it is false,
## temp_name() will find the smallest acceptable number for the name.
## Otherwise, a hopefully quicker method is used.
##
## @seealso{}
## @end deftypefn
function n = temp_name (rootname, quick)

### Keep track of previously asked names
static cnt = struct ("dummy",0);

if nargin<1 || !length(rootname), rootname = "temp_name_" ; end

if nargin<2, quick = 1; end

if quick
  if ! isfield (cnt, rootname)
    cnt.(rootname) = 0;
    c = 0 ;
  else
    c = cnt.(rootname) ;
  end
else
  c = 0;
end

n = sprintf ([rootname,"%i"], c);

while exist (n),
  c++ ;
  n = sprintf ([rootname,"%i"], c);
end

if quick
  cnt.(rootname) = c ;
end
