/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/

#ifndef EAR_PATHS_H
#define EAR_PATHS_H

// Library paths
#define CPUPOWER_LIB_PATH   ""
#define FREEIPMI_LIB_PATH   ""
#define SLURM_LIB_PATH      ""
#define PAPI_LIB_PATH       ""
#define GSL_LIB_PATH        ""
#define EAR_INSTALL_PATH    "/usr/local"

// File paths
#define EAR_DAEMON_PATH     ""
#define EAR_CONF_FILE		""
#define IMPI_C_LIB_PATH		"lib/libear.so"
#define OMPI_C_LIB_PATH		"lib/libear_ompi.so"
#define IMPI_F_LIB_PATH		""
#define OMPI_F_LIB_PATH		""

// Database
#define DB_FILES			0
#define DB_MYSQL			0

#endif //EAR_PATHS_H
