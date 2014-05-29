# Find other Petuum paths by using the script's path
script_path=`readlink -f $0`
script_dir=`dirname $script_path`
project_root=`dirname $script_dir`

output_llh_file=$project_root/log/nytimes.K1000_run2.llh
#data_file=datasets/20news.ssv
data_file=datasets/nytimes.ssv
machinefile=machinefiles/cogito-2
num_machines=`wc -l $machinefile | awk '{print $1}'`
num_threads=16
num_topics=1000
alpha=1
beta=0.01

run_time_in_sec=1500

cmd="mpiexec --hostfile $machinefile -n $num_machines \
./release/toolkits/topic_modeling/cgs_lda \
--alpha $alpha \
--beta $beta \
--ncpus=$num_threads \
--ntopics $num_topics \
--corpus $data_file \
--output_llh_file $output_llh_file \
--burnin $run_time_in_sec"

echo $cmd
eval $cmd
