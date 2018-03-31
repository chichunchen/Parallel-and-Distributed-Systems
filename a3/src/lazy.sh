sh timing.sh histo_lock2 300px > histo_lock2-cycle-300px.result
sh timing.sh histo_lock2 earth > histo_lock2-cycle-earth.result
sh timing.sh histo_lock2 flood > histo_lock2-cycle-flood.result
sh timing.sh histo_lock2 moon-small > histo_lock2-cycle-moon-small.result
sh timing.sh histo_lock2 moon-large > histo_lock2-cycle-moon-large.result
sh timing.sh histo_lock2 polar_bear_in_snow_8bit > histo_lock2-cycle-polar_bear_in_snow_8bit.result
sh timing.sh histo_lock2 noise_8bit > histo_lock2-cycle-noise_8bit.result
sh timing.sh histo_lock2 phobos > histo_lock2-cycle-phobos.result
sh timing.sh histo_lock2 university > histo_lock2-cycle-university.result

#python3 get.py histo_lock2-cycle-300px.result
#python3 get.py histo_lock2-cycle-earth.result
#python3 get.py histo_lock2-cycle-flood.result
#python3 get.py histo_lock2-cycle-moon-small.result
#python3 get.py histo_lock2-cycle-moon-large.result
#python3 get.py histo_lock2-cycle-polar_bear_in_snow_8bit.result
#python3 get.py histo_lock2-cycle-noise_8bit.result
#python3 get.py histo_lock2-cycle-phobos.result
#python3 get.py histo_lock2-cycle-university.result
