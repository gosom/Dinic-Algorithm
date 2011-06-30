"""
Implementation of Dinic algorithm.
Read edges data from standard input and return
the calculated maximum network flow.

"""

import sys
from dinic_logging import *
from collections import deque

S = 0
T = 1

def make_auxiliar_network(edges):
    """Create an auxiliar network from a edges list
    Returns an auxiliar network."""

    s = S;t = T;na = {}
    fl = deque([s]) # fathers layer
    cl = deque([])  # childrens layer
    level = 0
    vl = {s:level} # vertex : level
    while len(fl) > 0:
        key = fl.popleft()
        for e in edges:
            if e['used']: continue
            
            r = e['capacity'] - e['flow']
            if key == e['first'] and  r > 0:
                v = {'id':e['last'], 'direction':'F'}
            elif key == e['last'] and e['flow'] > 0:
                v = {'id':e['first'], 'direction':'B'}
            else: continue
            
            if v['id'] not in cl: 
                cl.append(v['id'])
            if key not in na:
                na[key] = []

            if v['id'] not in vl:
                vl[v['id']] = level
            if vl[v['id']] == level:
                na[key].append(v)
                e['used'] = True

        
        if len(fl) == 0: fl = cl; cl = deque([]); level += 1
        if len(fl) == 0 or t in fl: break

    complete = False
    # remove all vertex except t from last layer
    for n in [k for k in vl if vl[k] == level-1]: 
        if n == t:
            complete = True
            continue
        for k in na:
            na[k] = [v for v in na[k] if v['id'] != n]

                
    return {'na':na, 'complete':complete}


def get_path(na, edges):
    """Returns a list [path, mincost] with a path from s to t 
    in the auxiliar network and his mincost value."""
    
    s = S; t = T
    key = s
    path = [{'id':s}]
    minflow = sys.maxsize
    complete = False
    while True:
        if key in na and len(na[key]) > 0:
            n = na[key][0]
            path.append(n)
        elif key == s:
            break
        elif key == t:
            complete = True
            break
        else:
            path.pop()
            n = path.pop()
            path.append(n)
            for k in na: # remove all nodes entering key
                na[k] = [v for v in na[k] if v['id'] != key]

            key = n['id']
            continue

        if n['direction'] == 'F':
            e = [e for e in edges if e['first'] == key and e['last'] == n['id']][0]
            r = e['capacity'] - e['flow'] 
            if r < minflow:
                minflow = r

        if n['direction'] == 'B':
            e = [e for e in edges if e['last'] == key and e['first'] == n['id']][0]
            if e['flow'] < minflow:
                minflow = e['flow']
        key = n['id']
                
    return {'path':path, 'minflow':minflow, 'complete':complete}

def augment(na, edges, path, mincost):
    """Augment the flow in each edge from path by mincost."""
    
    s = S
    first = s
    path = deque(path)
    path.popleft()
    while len(path) > 0:
        v = path.popleft()
        last = v['id']
        if v['direction'] == 'F':
            e = [e for e in edges if e['first'] == first and e['last'] == last][0]
            e['flow'] += mincost
            
            if e['capacity'] - e['flow'] == 0:
                na[e['first']] = [v for v in na[e['first']] if v['id'] != e['last']]

        if v['direction'] == 'B':
            e = [e for e in edges if e['last'] == first and e['first'] == last][0]
            e['flow'] -= mincost
            
            if e['flow'] == 0:
                na[e['last']] = [v for v in na[e['last']] if v['id'] != e['first']]

        first = last


def dinic(edges):
    """Dinic algorithm"""

    s = S
    na_num = 0
    print_init()
    while True:
        for e in edges:
            e['used'] = False
        data = make_auxiliar_network(edges)
        na = data['na']
        na_num += 1
        complete = data['complete']
        print_na(na, na_num)
        if not complete:
            print_corte(na)
            break
        while True:
            path = get_path(na, edges)
            if path['complete']:
                print_path(path)
                augment(na, edges, path['path'], path['minflow'])
            else:
                break

    print_edges(edges)
    return sum([e['flow'] for e in edges if e['first'] == s])


def read_edges(f=sys.stdin):
    """Read edges data from standart input
    Returns a list of edges with format [first, last, capacity, flow, used]
    """
    edges = []
    k = ['first', 'last', 'capacity', 'flow', 'used']
    lines = f.readlines()
    for line in lines:
        v = [int(s) for s in line.split(" ")] + [0, False]
        edges.append(dict(zip(k,v)))
                      

    return edges

def run_dinic(file_name):
    f = open(file_name, "r")
    edges = read_edges(f)
    maxflow = dinic(edges)
    return maxflow

if __name__ == "__main__":
    maxflow = run_dinic(sys.argv[1])
    print(maxflow)
