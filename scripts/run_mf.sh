# Find other Petuum paths by using the script's path
script_path=`readlink -f $0`
script_dir=`dirname $script_path`
project_root=`dirname $script_dir`

#data_path=$project_root/datasets/smallnetflix/
#data_path=$project_root/datasets/smallnetflix/
#data_path="/l0/netflix/"
data_path=$project_root/datasets/netflix/

run_id=0
if [ $# -eq 1 ]; then
  run_id=$1
fi
machinefile=machinefiles/marmot-64
num_machines=`wc -l $machinefile | awk '{print $1}'`
num_threads=2
num_iterations=100
rank=100
init_step_size=1e-7
# two times the init_step_size in petuum.
gamma=2e-7
lambda=50
step_dec=0.9
output_file=$project_root/output/mf_M${num_machines}_step${init_step_size}_gamma${gamma}_lambda${lambda}_run${run_id}

if [ -e "$output_file" ]; then
  echo ======= Output file already exist. Make sure not to overwrite previous experiment. =======
  echo $output_file
  echo Will proceed in 3 seconds
  sleep 3
fi

#ssh_options="-oStrictHostKeyChecking=no -oUserKnownHostsFile=/dev/null -oLogLevel=quiet"

cmd="GRAPHLAB_SUBNET_ID=10.2.0.0 \
GRAPHLAB_SUBNET_MASK=255.255.255.0 \
mpiexec  --hostfile $machinefile -n $num_machines \
./release/toolkits/collaborative_filtering/sgd \
$data_path  \
--ncpus=$num_threads \
--max_iter=$num_iterations \
--gamma=$gamma \
--lambda=$lambda \
--output_file=$output_file \
--step_dec=$step_dec \
--D=$rank \
--minval=0 \
--maxval=5 "

echo $cmd
eval $cmd
