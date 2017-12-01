#########################
# Convert ccbar NTuple
#########################

# Requirements = ( Machine != "star1.local" && Machine != "star2.local" && Machine != "star3.local" && Machine != "star4.local" )

Executable = /home/jdb12/work/dimuonAna/HadronicCocktail/bin/hadronicCocktail.app
Initialdir = /home/jdb12/work/dimuonAna/HadronicCocktail/bin/

# ccbar -> mumu
Arguments = /home/jdb12/work/dimuonAna/HadronicCocktail/bin/config/tasks/ConvertCCbar.xml --jobIndex=$(Process) --maxJobIndex=405
Queue 405