if [[ ! $1 ]]; then
	echo -e "\n\n\tNo file given.\n\n"
	exit
fi

clear

# Convert to BASM
./_parser $1

# Run the BASM
./_engine result.s

