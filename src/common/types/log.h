/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_LOG_H
#define _EAR_LOG_H

typedef struct ear_event{
	uint job_id;
	uint event;
	ulong freq;
}ear_event_t;

#define ENERGY_POLICY_NEW_FREQ	0
#define GLOBAL_ENERGY_POLICY	1
#define ENERGY_POLICY_FAILS		2
#define DYNAIS_OFF				3

/** Creates the log file and starts it with the current time. If it can't
*   create the file it reports it to stderr */
void init_log();
/** Finishes the log with the current time and closes the log file */
void end_log();

/** Given an event, it reports it ot the log file*/
void report_new_event(ear_event_t *event);

/** Given a job id and a frequency value, reports to the log file the change
*   of frequency because of the energy policy */
void log_report_new_freq(int job,ulong newf);
/** Given a job id, reports to the log file that the DynAIS has been turned off */
void log_report_dynais_off(int job);
/** Given a job id and a frequency value, reports to the log file the change
*   of frequency because of a policy projections failure */
void log_report_max_tries(int job,ulong newf);
/** Given a job id and a frequency value, reports to the log file the change
*   of frequency because of Energy Budget*/
void log_report_global_policy_freq(int job,ulong newf);
#endif
