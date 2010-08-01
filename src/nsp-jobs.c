/*
 * Copyright © 2009-2010 Siyan Panayotov <xsisqox@gmail.com>
 *
 * This file is part of Nowspide.
 *
 * Nowspide is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nowspide is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nowspide.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#include "nsp-jobs.h"
#include <stdlib.h>
#include <assert.h>
#include <sys/sysinfo.h>
#include <gtk/gtk.h>

NspJob *
nsp_job_new(NspCallback *worker, void *data)
{
	NspJob *job = malloc(sizeof(NspJob));
	assert(job != NULL);
	
	job->worker = worker;
	job->data = data;
	
	return job;
}

void
nsp_jobs_main_loop(gpointer data, gpointer user_data)
{
	NspJob *job = (NspJob*) data;
	if ( job != NULL && job->worker != NULL) {
		job->worker(job->data);
	}
}

NspJobs *
nsp_jobs_new()
{
	NspJobs *jobs = malloc(sizeof(NspJobs));
	
	assert(jobs != NULL);
	
	jobs->pool = g_thread_pool_new ((GFunc)nsp_jobs_main_loop, NULL, get_nprocs()+1, FALSE, NULL );
	
	return jobs;
}

void
nsp_jobs_free(NspJobs *jobs)
{
	if (jobs == NULL) {
		return;
	}
	
	g_thread_pool_free(jobs->pool, FALSE, FALSE);
}


int nsp_jobs_queue(NspJobs *jobs, NspJob *job)
{
	g_thread_pool_push(jobs->pool, (gpointer)job, NULL);
	return 0;
}




