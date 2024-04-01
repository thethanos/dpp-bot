git clone https://github.com/brainboxdotcc/DPP.git

wget https://www.sqlite.org/src/tarball/sqlite.tar.gz
tar -xvf sqlite.tar.gz

docker build -t base_image:latest -f base.Dockerfile .

rm -rf DPP sqlite*

