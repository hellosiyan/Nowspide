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


#ifndef __NSP_JOBS_H__
#define __NSP_JOBS_H__ 1

#include <gtk/gtk.h>
#include "nsp-typedefs.h"

typedef struct _NspJob NspJob;

struct _NspJob {
	NspCallback *worker;
	void *data;
};

NspJob *nsp_job_new(NspCallback *worker, void* data);

typedef struct _NspJobs NspJobs;

struct _NspJobs {
	GThreadPool *pool;
};

NspJobs * nsp_jobs_new();

int nsp_jobs_queue(NspJobs *jobs, NspJob *job);

void nsp_jobs_free(NspJobs *jobs);


#endif /* __NSP_JOBS_H__ */
