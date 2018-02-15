class TaskGraph(object):
    def __init__(self, f):
        self.filename = f

    def read(self):
        with open(self.filename) as f:
            header = f.readline()
            nodes, edges = [int(x) for x in header.split()]

            seen_task_ids = set()
            num_edges = 0

            graph = {}

            for l in f:
                ls = l.strip().split()

                assert len(ls) >= 3, "Line '%s' malformed" % (l,)

                task_id = int(ls[0])

                assert task_id not in seen_task_ids, "Duplicate task id: %d" % (task_id,)
                seen_task_ids.add(task_id)

                task_time = int(ls[1])

                assert task_time > 0, "Invalid task time: %d" % (task_time,)

                num_dependences = int(ls[2])

                dependences = [int(x) for x in ls[3:]]
                assert len(dependences) == num_dependences, "Line '%s' dependence count mismatch (%d != %d)" % (l, num_dependences, len(dependences))
                assert len(dependences) == len(set(dependences)), "Duplicate dependences: %s" % (dependences,)

                assert all([x >= 0 and x < nodes for x in dependences]), "Task %d has dependences to task beyond number of tasks (%s)" % (task_id, dependences)

                num_edges += num_dependences
                assert num_edges <= edges, "Dependences (%d) exceed header (%d)" % (num_edges, edges)

                if task_id not in graph:
                    graph[task_id] = {'src': set()}

                graph[task_id].update({'time': task_time, 'dependent': dependences})

                for t in dependences:
                    if t not in graph:
                        graph[t] = {'src': set()}

                    graph[t]['src'].add(task_id)

            assert num_edges == edges, "Fewer dependences read (%d) than indicated in header (%d)" % (num_edges, edges)
            assert len(seen_task_ids) == nodes, "Fewer tasks read (%d) than indicated in header (%d)" % (len(seen_task_ids), nodes)
            
            self.graph = graph
            self.num_nodes = nodes
            self.num_edges = edges
    
    def write(self, outfile=None):
        with open(outfile if outfile is not None else self.f, "w") as f:
            # this currently assumes the internal state of the graph is valid

            f.write("%d %d\n" % (self.num_nodes, self.num_edges))

            # not necessary, but makes it easier to verify
            nodes = sorted(self.graph.keys())

            for n in nodes:
                gn = self.graph[n]

                f.write('{taskid} {time} {ndep} {dep}\n'.format(taskid = n, time=gn['time'], 
                                                                ndep = len(gn['dependent']),
                                                                dep = " ".join([str(x) for x in gn['dependent']])))
                

    def draw(self, dotfile):
        with open(dotfile, "w") as f:
            f.write("digraph {\n")

            for n in self.graph:
                f.write('{taskid} [label="T{taskid}: {time}"];\n'.format(taskid = n, time=self.graph[n]['time']))
                
                for e in self.graph[n]['dependent']:
                    f.write('{src} -> {dst};\n'.format(src = n, dst = e))
                
            f.write("}\n")
        
    

if __name__ == "__main__":
    import argparse
    import sys

    p = argparse.ArgumentParser(description="Read a task graph and print out statistics")
    p.add_argument("taskgraph")
    p.add_argument("-d", dest="dotgraph", help="Generate a graphviz dot file")
    p.add_argument("-o", dest="output", help="Write a file to output")

    args = p.parse_args()
    
    tg = TaskGraph(args.taskgraph)

    tg.read()

    if args.dotgraph: 
        print >>sys.stderr, "Drawing graphviz: %s" % (args.dotgraph,)
        tg.draw(args.dotgraph)

    if args.output: 
        print >>sys.stderr, "Writing taskgraph: %s" % (args.output,)
        tg.write(args.output)
