#########################
# Convert ccbar NTuple
#########################



Executable = /home/jdb12/work/HadronicCocktail/bin/prod.app
Initialdir = /home/jdb12/work/HadronicCocktail/bin/

# Phi -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/tasks/ConvertCCbar.xml --jobIndex=$(Process) --maxJobIndex=112
Queue 112