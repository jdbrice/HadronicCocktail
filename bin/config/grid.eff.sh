#############################
# Blend DiMuon Cocktail Eff
#############################

Executable = /home/jdb12/work/HadronicCocktail/bin/prod.app
Initialdir = /home/jdb12/work/HadronicCocktail/bin/

# TRUE
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/eff.xml --jobIndex=$(Process) --maxJobIndex=400 --blend.Efficiency:apply=true
Queue 60

# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/eff.xml --jobIndex=$(Process) --maxJobIndex=400 --blend.Efficiency:apply=false
# Queue 400