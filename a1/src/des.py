#!/usr/bin/env python
# Discrete Event Simulator

from __future__ import division
import taskgraph
from collections import namedtuple

Event = namedtuple('Event', ['task_id', 'proc', 'start_time', 'time'])

class Schedule(object):
    def __init__(self, tgfile, schedfile, nprocs = 0):
        self.tg = taskgraph.TaskGraph(tgfile)
        self.sf = schedfile
        self.nprocs = nprocs

        self.tg.read()

    def read(self):
        out = {}
        tasks_seen = set()

        with open(self.sf, "r") as f:
            for l in f:
                ls = l.strip().split()

                # task-id processor start-time
                tid, proc, start = [int(x) for x in ls]

                assert tid not in tasks_seen, "Duplicate task event %d" % (tid,)
                assert proc >= 0, "Invalid processor %d" % (proc,)
                if self.nprocs != 0:
                    assert proc < self.nprocs, "Processor must be from 0 to %d, is currently %d for task %d" % (self.nprocs - 1, proc, tid)
                    
                assert start >= 0, "Start time for task %d must be non-negative" % (start,)

                tasks_seen.add(tid)
                out[tid] = Event(task_id = tid, proc = proc, 
                                 start_time = start, 
                                 time = self.tg.graph[tid]['time'])
                
        assert len(out) == self.tg.num_nodes, "Tasks in graph (%d) not equal to tasks in schedule (%d)" % (len(out), self.tg.num_nodes,)

        self.events = out

    def simulate(self):
        busy_time = {}
        next_free_time = {}
        tasks_per_proc = {}
        
        ordered = sorted([x for x in self.events.itervalues()], 
                         key=lambda x: x.start_time)

        for ev in ordered:
            # check all source dependences have completed
            parents = [self.events[p] for p in self.tg.graph[ev.task_id]['src']]
            assert all([p.start_time + p.time <= ev.start_time for p in parents]), "Dependence sources (%s) for task %s have not completed." % (parents, ev)

            for x in [busy_time, next_free_time, tasks_per_proc]:
                if ev.proc not in x:
                    x[ev.proc] = 0

            # check processor is free
            assert ev.start_time >= next_free_time[ev.proc], "Processor %d only free at %d, but task %d scheduled at %d" % (ev.proc, next_free_time[ev.proc], ev.task_id, ev.start_time)

            # mark processor unavailable
            next_free_time[ev.proc] = ev.start_time + ev.time

            # update statistics
            busy_time[ev.proc] += ev.time
            tasks_per_proc[ev.proc] += 1

        max_proc = max(self.nprocs -1, max(busy_time.keys()))
        total_time = max(next_free_time.values())
        total_tasks = len(ordered)

        print "total execution time: ", total_time, "time-units"
        print "total tasks scheduled: ", total_tasks, "tasks"
        print "max processors used: %d (unique: %d)" % (max_proc + 1, 
                                                        len(set(busy_time.keys())))

        # hacks for division below ...
        if total_time == 0: total_time = 1
        if total_tasks == 0: total_tasks = 1

        for i in range(0, max_proc+1):
            print "processor %d: " % (i,)
            print "\tbusy: %d time-units (utilization: %0.2f%%)" % (busy_time.get(i, 0), 
                                                                    100 * busy_time.get(i, 0) / total_time)

            print "\ttasks: %d tasks (%0.2f%%)" % (tasks_per_proc.get(i, 0), 
                                                   100 * tasks_per_proc.get(i, 0) / total_tasks)

if __name__ == "__main__":
    import argparse

    p = argparse.ArgumentParser(description="Simulate a schedule trace")
    p.add_argument("taskgraph")
    p.add_argument("schedule")
    p.add_argument("-n", dest="nproc", help="Processors, 0 for infinite", default=0, type=int)

    args = p.parse_args()

    sched = Schedule(args.taskgraph, args.schedule, args.nproc)
    sched.read()
    sched.simulate()

