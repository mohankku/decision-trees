call "data/common.gnuplot" "3.0in,1.2in"

set output "`echo $OUT`"

mp_startx=0.08
mp_starty=-0.07
mp_height=0.9
mp_rowgap=0.04
mp_colgap=0
mp_width=0.86

eval mpSetup(1, 1)

set ylabel "RMSE"
set xlabel "\\# leaf_size" offset 0,0.6

set grid y
set auto x

eval mpNext
set key default
set key spacing 1.5
set xrange[0:60]
set yrange[0.002:0.012]

plot 'data/rmse_interval_train.data' using 1:($2/1) title  "Training data" with lp linecolor rgb "#FF0000", \
  'data/rmse_interval_train.data' using 1:($3/1) title  "Test data" with lp linecolor rgb "#0000FF"