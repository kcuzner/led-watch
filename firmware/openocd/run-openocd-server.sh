#!/bin/bash

# Usage: run-openocd-server <openocd command> <pidfile>

# Important: This must be run with "bash" executed as "bash", not "sh". I'm apparently
# doing something weird and strange behavior results when bash decides to go POSIX.

DIR=$(dirname $0)
CFGFILE=$DIR/openocd.cfg

if [ -e $2 ]; then
    pid=`cat $2`
    if kill -0 $pid 2>&1 > /dev/null; then
        echo 'OpenOCD Already Running'
        touch $2
        exit 1
    else
        rm $2
    fi
fi

run_server() {
    # Runs the openocd debugger in the background
    $1 -f $DIR/openocd.cfg &> $DIR/../bin/openocd.log & OCDPID=$!

    # Write the PID to a file
    echo $OCDPID > "$2"

    # Wait for openocd to die
    wait

    # Delete the PID file
    rm "$2"
}

# Run the server
run_server $1 $2 & BASHPID=$!

# Wait for the PID file to appear (or for our child to die) before we exit
while [[ ! -f $2 ]]
do
    sleep 1
done

