#!/bin/bash

source /Users/danielbrandenburg/bnl/vendor/prebuilt/root5.34.36/bin/thisroot.sh
declare -a CHANNELS=("omega_pi0mumu_dalitz" "eta_gammamumu_dalitz" "etaprime_gammamumu_dalitz" "eta_mumu" "omega_mumu" "phi_mumu" "rho_mumu" )

for c in "${CHANNELS[@]}" 
do
	echo "./hadronicCocktail.app tasks/DecayMaker.xml --mod="MTD-TOF-j2" --CHANNEL=${c} --N=10000000"
	./hadronicCocktail.app tasks/DecayMaker.xml --mod="MTD_TOF-j2" --CHANNEL=${c} --N=10000000 >& /dev/null &
done
