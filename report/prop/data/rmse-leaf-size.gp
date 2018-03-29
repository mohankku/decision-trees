call "data/common.gnuplot" "7.6in,2in"

set output "`echo $OUT`"

mp_startx=0.045
mp_starty=0.0
mp_height=0.850
mp_colgap=0.07                                                                                                                              
eval mpSetup(2, 1)

set ylabel "RMSE"
set xlabel "\\# leaf_size" offset 0,0.6

set grid y
set auto x

eval mpNext
set key default
set key spacing 2
set xrange[0:60]
set yrange[0.002:0.012]
set title '(a) With leaf_size interval 3'

plot 'data/rmse_train.data' using 1:($2/1) title  "Training data" with lp linecolor rgb "#FF0000", \
  'data/rmse_train.data' using 1:($3/1) title  "Test data" with lp linecolor rgb "#0000FF"

eval mpNext
set key default
set key spacing 2
set xrange[0:30]
set yrange[0.002:0.012]
set title '(b) With leaf_size interval 1'

plot 'data/rmse_interval_train.data' using 1:($2/1) title  "Training data" with lp linecolor rgb "#FF0000", \
  'data/rmse_interval_train.data' using 1:($3/1) title  "Test data" with lp linecolor rgb "#0000FF"