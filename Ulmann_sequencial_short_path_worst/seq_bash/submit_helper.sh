
for i in $(seq 1 32); do
    qsub seq_1cpu_8_20_01.job
    qsub seq_1cpu_8_20_005.job
done
