# Cheese Mod Kart
A dumb Mario Kart Wii mod for April Fools.

## Wiiki Page
https://wiki.tockdom.com/w/index.php?title=Cheese_Mod_Kart

## Building
To compile this yourself, you'll need the following:
* devkitPPC
* PistonMiner's [elf2rel](https://github.com/PistonMiner/ttyd-tools/tree/master/ttyd-tools/)
* The TTYDTOOLS environment variable set to the folder outside of the `bin` folder with your compiled `elf2rel` in (so `$(TTYDTOOLS)/bin/elf2rel` will point to it)

Once that's set up you can use `make rg` to build region `rg` ('eu', 'us', 'jp' or 'kr') or just `make` to build for all regions.

## Credits
* PistonMiner for creating the original TTYD rel framework.
* TheLordScruffy for the rel loader from Tournament Museum (not included in this repo).
