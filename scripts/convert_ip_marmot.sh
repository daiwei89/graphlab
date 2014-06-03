if [ $# -ne 2 ]; then
  echo "usage: $0 <host file> <output file>"
  exit
fi

awk '{print $2}' $1 | awk -F. '{print "10.1.1."$4}' | uniq > $2
#awk '{print $2}' $1  > $2
