 call "data/common.gnuplot" "7.4in,1.6in"

set output "`echo $OUT`"


mp_startx=0.045
mp_starty=0.0
mp_height=0.850
mp_colgap=0.07

eval mpSetup(2, 1)

set xlabel '\#core' offset 0,0.5
set ylabel 'messages/sec' offset .5
set yrange [0:14.5]
set xrange [0:80]

eval mpNext
unset key
set format y "%2.0fk"
set title '(a) Different file systems'
set key default
set key spacing 1.25
set key at 77,13.5 vertical maxrows 1 width 3.8
plot    "data/app-sc/exim/mem:btrfs:no-fsync.dat" using 1:($2/1000) index 0 title "\\btrfs" with linespoints ls 1, \
        "data/app-sc/exim/mem:ext4:no-fsync.dat" using 1:($2/1000) index 0 title "\\ext" with linespoints ls 2, \
        "data/app-sc/exim/mem:f2fs:no-fsync.dat" using 1:($2/1000) index 0 title "\\ffs" with linespoints ls 3, \
        "data/app-sc/exim/mem:xfs:no-fsync.dat" using 1:($2/1000) index 0 title "\\xfs" with linespoints ls 4


eval mpNext
set format y "%2.0f"
set title '(b) CPU utilization (80-core)'
set style data histogram
set style histogram rowstacked
set style fill solid 0.95 border -2
set yrange [0:*]
unset ylabel
set border 11 lw 1
set boxwidth 0.75 absolute
set style fill transparent solid 0.5 noborder
set xrange [-0.5:3.5]
unset xlabel
set key outside vertical width 1 maxrows 1
set key at 2.8,114
set yrange [0:115]
set style fill pattern
set ylabel "CPU utilization (\\%)"
set xtics ("\\btrfs" 0, \
	 "\\ext" 1, \
	 "\\ffs" 2, \
	 "\\xfs" 3) nomirror

plot    \
    "data/app-sc/exim/cpu-util.dat" u 1 t "usr" linecolor rgb "#008800" lw 8 lt 1 fs pattern 1, \
    "" using 2 t "sys" linecolor rgb "#FF4D4D" lw 8 lt 1 fs pattern 6, \
    "" using 3 t "idle" linecolor rgb "#5060D0" lw 8 lt 1 fs patter 2
