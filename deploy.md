# cloud or not

## The Smart Way to node(tm)

* Wait for node to be stable enough
* Learn how to host on PaaS
* Learn how to host it yourself

## port 80 process.setuid()

* Start process as root
* Drop privileges once port is open

## iptables

* Forward traffic from port 80 to another port
* sudo iptables -t nat -A PREROUTING -p tcp --dport 80 -j REDIRECT --to-ports 8000
* Safe, simple, flexible

## Reverse Proxy

* Haproxy, Nginx, etc
* Useful for other routing, file serving
* Watch out for buffering, web sockets, etc

## Staying Alive

* Upstart, Monit, etc
* Detects process crashes, restarts them
* Make sure to log this

## The Build

* I recommend to vendor node with your app
* Build server makes sure node is compiled
* *.tar.gz can be deployed to same Kernel / Architecture

## The Deploy

* Another CI job
* Get's list of machines to deploy to
* Uploads .tar.gz, stops old app, starts new one via SSH

## Fork

* cluster.fork()
* ~1 process per CPU
* Load Balancing done by Kernel

## Gather metrics

* node-measured, etc
* Graphite, Librato, etc
* Measure early, measure often
* Requests / Events per Second (Meters)
* Response times (Histograms)
* Number of items in queue (Gauges)

## Configuration

* Use environment variables!
* Bash files to store configs locally
* [12factor](http://www.12factor.net)
