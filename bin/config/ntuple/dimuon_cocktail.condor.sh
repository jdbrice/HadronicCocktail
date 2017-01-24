#########################
# DiMuon Cocktail
#########################

Executable = /home/jdb12/work/HadronicCocktail/bin/prod.app
Initialdir = /home/jdb12/work/HadronicCocktail/bin/

# rho -> mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/rho_mumu.xml --jobIndex=$(Process) --SEED=7345387 --N=500000 --maxJobIndex=200
# Queue 200

# # Phi -> mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/phi_mumu.xml --jobIndex=$(Process) --SEED=9836311 --N=500000
# Queue 200

# # omega -> mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/omega_mumu.xml --jobIndex=$(Process) --SEED=4159768 --N=500000
# Queue 200

# # eta -> mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/eta_mumu.xml --jobIndex=$(Process) --SEED=8994701 --N=500000
# Queue 200

# J/Psi -> mumu
Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/jpsi_mumu.xml --jobIndex=$(Process) --SEED=75132502 --N=500000 --maxJobIndex=200
Queue 200

# # omega -> pi0mumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/omega_pi0mumu.xml --jobIndex=$(Process) --SEED=4806094 --N=500000
# Queue 200

# # eta -> gammamumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/eta_gammamumu.xml --jobIndex=$(Process) --SEED=9030314 --N=500000
# Queue 200

# # eta' -> gammamumu
# Arguments = /home/jdb12/work/HadronicCocktail/bin/config/ntuple/etaprime_gammamumu.xml --jobIndex=$(Process) --SEED=1480117 --N=500000
# Queue 200

