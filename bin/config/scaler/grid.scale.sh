#########################
# Scale DiMuon Cocktail
#########################

Executable = /home/jdb12/work/HadronicCocktail/bin/prod.app
Initialdir = /home/jdb12/work/HadronicCocktail/bin/

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=eta_mumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=eta_gammamumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=omega_mumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=omega_pi0mumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=rho_mumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=phi_mumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=etaprime_gammamumu
Queue 1

Arguments = /home/jdb12/work/HadronicCocktail/bin/config/scaler/template.xml --CHANNEL=ccbar_mumu
Queue 1

