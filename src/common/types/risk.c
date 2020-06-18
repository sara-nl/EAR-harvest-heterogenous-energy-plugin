/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <common/config.h>
#include <common/states.h>
#include <common/types/risk.h>
state_t set_risk(risk_t *r,risk_t new_r)
{
	*r=new_r;
	return EAR_SUCCESS;
}
int is_risk_set(risk_t r,risk_t value)
{
	return  (r&value);
}
state_t add_risk(risk_t *r,risk_t value)
{
	risk_t new_r;
	new_r=*r;
	*r=new_r|value;
	return EAR_SUCCESS;
}
state_t del_risk(risk_t *r,risk_t value)
{
	risk_t new_r;
	new_r=*r;
	*r=new_r&~value;
	return EAR_SUCCESS;
}

