#o "Enter the name of file A:"

file_a="tv_real_in.txt"



#echo "Enter the name of file B:"

file_b="tv_real_out.txt"



# extract all columns from file A and save to tmp_a_tv_data.txt

tail -n +4 $file_a | awk '{for (i=2;i<=NF;i++) printf "%s ", $i; print ""}' > tmp_a_tv_data.txt



# extract all columns from file B and save to tmp_b_tv_data.txt

tail -n +4 $file_b | awk '{for (i=2;i<=NF;i++) printf "%s ", $i; print ""}' > tmp_b_tv_data.txt



# compare each column of tmp_a_tv_data.txt with corresponding column of tmp_b_tv_data.txt

num_cols=$(head -n 1 tmp_a_tv_data.txt | awk '{print NF}')

for ((i=1; i<=num_cols; i++)); do

    col_a=$(awk -v col=$i '{print $col}' tmp_a_tv_data.txt)

    col_b=$(awk -v col=$i '{print $col}' tmp_b_tv_data.txt)

    if [[ "$col_a" == "$col_b" ]]; then

        echo "Match: column $i"

    else

        echo "Mismatch: column $i"

    fi

done



# remove temporary files

rm tmp_a_tv_data.txt tmp_b_tv_data.txt


