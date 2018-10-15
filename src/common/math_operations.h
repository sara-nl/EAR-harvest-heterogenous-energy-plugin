/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

/** Given two doubles a and b, checks if they are equal within a margin of th.*/
unsigned int equal_with_th(double a, double b, double th);

/** Given two unsigned long's, one before and one after overflow, returns the
*   value added to the first to obtain the second.*/
unsigned long ulong_diff_overflow(unsigned long a, unsigned long b);

/** Given two unsigned long's, one before and one after overflow, returns the
*   value added to the first to obtain the second.*/
unsigned long long ullong_diff_overflow(unsigned long long a, unsigned long long b);

/** Given two unsigned long's, one before and one after overflow, returns the
*   value added to the first to obtain the second. This uses the normal 48 bit max*/
unsigned long long uncore_ullong_diff_overflow(unsigned long long a, unsigned long long b);
