#!/bin/sh -
# usage: [VERBOSE=1] fstest.sh fsprog

print ()
{
	echo "$@" 1>&2
}

message ()
{
	if [ "$VERBOSE" ]
		then
			print "$@"
			fi
}

try ()
{
	if [ "$VERBOSE" ]
		then
			sh -c "$1"
	else
		sh -c "$1" 2>/dev/null
			fi
}

should_success ()
{
	message "Executing [$1] (should succeed)"
		try "$1" || {
			print "Command [$1] failed"; exit 1
		}
	message "ok"
}

should_fail ()
{
	message "Executing [$1] (should fail)"
		try "$1" && {
			echo "Command [$1] (which should fail) succeeded" 1>&2 ; exit 1
		}
	message "ok"
}

DIR="tmpdir"

message "Making directory $DIR"
should_success "mkdir $DIR"

message "Starting fs $1 on $DIR"
should_success "$1 $DIR"
cd $DIR

should_success "echo foo > file1"
should_success "cp file1 file2"
should_success "diff file1 file2"
should_success "mv file1 file1_1"
should_fail "rm file1"
should_success "dd if=/dev/zero seek=20 count=1 of=file2"
should_success "echo > file2"
should_success "rm file1_1"
should_fail "mkdir subdir"

message "Killing `basename $1`"
killall `basename "$1"`

print "ok!"
