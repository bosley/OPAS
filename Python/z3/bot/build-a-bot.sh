#!/bin/bash

echo "Starting, and steralizing linux-deployable"
rm -rf linux-deployable
mkdir linux-deployable

cp -r xnmlib-sterile xnmlib

echo "Setting up bot."
python3 setup.py

echo "Injecting pillar."
python3 pillar_inject.py

echo "Moving libs, and sterilizing"
mv -f xnmlib linux-deployable/
#rm -rf xnmlib

echo "Relocating node config"
mv local_node.config linux-deployable/

echo "Copying bot-code.py"
cp bot-code.py linux-deployable/start.py

mkdir linux-deployable/tools

cp remote_scripts/* linux-deployable/tools





