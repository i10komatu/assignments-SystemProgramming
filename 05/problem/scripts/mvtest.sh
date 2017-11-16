#!/bin/sh

echo test1 >> tmpdir/file
mv tmpdir/file tmpdir/file1
echo test2 >> tmpdir/file2
mv tmpdir/file1 tmpdir/file2