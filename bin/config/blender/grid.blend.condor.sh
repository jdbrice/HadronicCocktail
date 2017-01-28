#########################
# Blend DiMuon Cocktail
#########################

Executable = /home/jdb12/work/HadronicCocktail/bin/prod.app
Initialdir = /home/jdb12/work/HadronicCocktail/bin/

# rho -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/rho_mumu.xml --jobIndex=$(Process) --maxJobIndex=100
Queue 100

# ccbar -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/ccbar.xml --jobIndex=$(Process) --maxJobIndex=56
Queue 56

# Phi -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/phi_mumu.xml --jobIndex=$(Process) --maxJobIndex=100
Queue 100

# omega -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/omega_mumu.xml --jobIndex=$(Process) --maxJobIndex=100
Queue 100

# J/Psi -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/jpsi_mumu.xml --jobIndex=$(Process) --maxJobIndex=100
Queue 100

# eta -> mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/eta_mumu.xml --jobIndex=$(Process) --maxJobIndex=100
# Queue 100

# # omega -> pi0mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/omega_pi0mumu.xml --jobIndex=$(Process) --maxJobIndex=100
# Queue 100

# # eta -> gammamumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/eta_gammamumu.xml --jobIndex=$(Process) --maxJobIndex=100
# Queue 100

# # eta' -> gammamumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/blender/etaprime_gammamumu.xml --jobIndex=$(Process) --maxJobIndex=100
# Queue 100

