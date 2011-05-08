## Copyright (C) 2007 Muthiah Annamalai <muthiah.annamalai@uta.edu>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

## -*- texinfo -*-
## @deftypefn {Function File} {@var{coefs}=} legendrepoly (@var{order},@var{x})
## 
## Compute the coefficients of the Legendre polynomial, given the 
## @var{order}. We calculate the Legendre polynomial using the recurrence
## relations, Pn+1(x) = inv(n+1)*((2n+1)*x*Pn(x) - nPn-1(x)).
## 
## If the value @var{x} is specified, the polynomial is also evaluated,
## otherwise just the return the coefficients of the polynomial are returned.
## 
## This is NOT the generalized Legendre polynomial.
##
## @end deftypefn

function h=legendrepoly(order,val)
	if nargin < 1, error('usage: legendrepoly(order,val)'), end
	
	h_prev=[0 1];
        h_now=[1 0];

	if order == 0
	   h=h_prev;
        else
           h=h_now;
        end

	for ord=2:order
         x=[];y=[];
         if (length(h_now) < (1+ord))
	   x=0;
         end;
	 y=zeros(1,(1+ord)-length(h_prev));
	 p1=[h_now, x];
         p3=[y, h_prev];
         h=((2*ord -1).*p1  -(ord -1).*p3)./(ord);
	 h_prev=h_now;
	 h_now=h;
	end

        if nargin == 2
    	 h=polyval(h,val);
	end
	
	return
end
