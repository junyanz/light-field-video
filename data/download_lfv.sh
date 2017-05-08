FILE=$1
URL=https://cseweb.ucsd.edu/~viscomp/projects/LF/papers/SIG17/lfv/paperData/seqs/$FILE.zip
TAR_FILE=./data/$FILE.zip

wget -N $URL -O $TAR_FILE
cd data

unzip $FILE.zip
rm $FILE.zip
