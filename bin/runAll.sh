#!/opt/local/bin/zsh

SMEAR="0"


declare -a CHANNELS=("omega_pi0mumu_dalitz" "eta_gammamumu_dalitz" "etaprime_gammamumu_dalitz" "eta_mumu" "omega_mumu" "phi_mumu" "rho_mumu" "jpsi_mumu" "psi_mumu")

for c in "${CHANNELS[@]}" 
do
	echo "./hadronicCocktail.app DecayMaker.xml --mod="j0" --CHANNEL=${c} --N=10000001"
	./hadronicCocktail.app DecayMaker.xml --mod="j0" --CHANNEL=${c} --N=10000001
done
