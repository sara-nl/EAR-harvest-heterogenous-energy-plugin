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

/**
*    \file projection_old.h
*    \brief Projections are used by time&power models.
*
*/

#ifndef _EAR_TYPES_PROJECTION
#define _EAR_TYPES_PROJECTION

#include <common/types/signature.h>
#include <common/types/coefficient.h>

typedef struct obs_cpi
{
	double obs_cpi;
	double obs_tpi;
} obs_cpi_t;

typedef struct obs_time
{
	double obs_time;
	double obs_cpi;
} obs_time_t;

typedef struct obs_power
{
	double obs_power;
	double obs_tpi;
} obs_power_t;


// Fill
void observed_values_fill_cpi(obs_cpi_t *obs, signature_t *sign);

void observed_values_fill_time(obs_time_t *obs, signature_t *sign);

void observed_values_fill_power(obs_power_t *obs, signature_t *sign);

// Projections
double proj_project_cpi(obs_cpi_t *cpi, coefficient_t *coeff);

double proj_project_time(obs_time_t *time, coefficient_t *coeff, double proj_cpi);

double proj_project_power(obs_power_t *power, coefficient_t *coeff);

#endif