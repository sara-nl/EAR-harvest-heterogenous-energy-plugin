#!/bin/bash
if [[ $# -ne 2 ]]
then
        echo -e "Usage: $0 job_name pid_first_rank"
        exit 1
fi

echo "Creating $2 folder"
mkdir $2
echo "Merging prv file in $2/$1.prv"
mv 1_$1.*.prv $2/$1.prv
cat ?_$1.*.prv >> $2/$1.prv
echo "Deleting prv files"
rm -rf ?_$1.*.prv
echo "Moving pcf file to $2/$1.pcf"
mv 1_$1.*.pcf $2/$1.pcf
echo "Merging row files to $2/$1.row"
cat ?_$1.*.row >> $2/$1.row
echo "Deleting row files"
rm -rf ?_$1.*.row
