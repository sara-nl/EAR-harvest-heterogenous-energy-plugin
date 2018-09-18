EAR islands examples
--------------------

This is an example of a cluster of 1.200 nodes, with two different kinds of nodes forming a two islands design. It uses 4 servers and 4 mirror due the huge number of nodes per island.

```
Island=0 Nodes=N[1000-1299] DBIP=DBN0.cluster.com DBSECIP=DBN1.cluster.com
Island=0 Nodes=N[1300-1599] DBIP=DBN2.cluster.com DBSECIP=DBN3.cluster.com
Island=1 Nodes=N[2000-2299] DBIP=DBN4.cluster.com DBSECIP=DBN5.cluster.com
Island=1 Nodes=N[2300-2599] DBIP=DBN6.cluster.com DBSECIP=DBN7.cluster.com
```

Another example with a 4 islands and a circular server-mirror design.

```
Island=0 Nodes=N[1000-1299] DBIP=DBN0.cluster.com DBSECIP=DBN1.cluster.com
Island=1 Nodes=N[2000-2299] DBIP=DBN1.cluster.com DBSECIP=DBN2.cluster.com
Island=2 Nodes=N[2000-2299] DBIP=DBN2.cluster.com DBSECIP=DBN3.cluster.com
Island=3 Nodes=N[2300-2599] DBIP=DBN3.cluster.com DBSECIP=DBN0.cluster.com
```

Another example with an exotic node enumeration in a simper island design:

```
Island=0 Nodes=NA[01-39,50-79],NB[01-39,50-79,90] DBIP=DBN0.cluster.com DBSECIP=DBN1.cluster.com
Island=0 Nodes=NB01,NB10,NB20,NB30 DBIP=DBN0.cluster.com DBSECIP=DBN1.cluster.com
```