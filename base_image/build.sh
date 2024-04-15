git clone https://github.com/gabime/spdlog.git

git clone https://github.com/brainboxdotcc/DPP.git

git clone https://github.com/google/googletest.git

git clone --branch 7.7 https://github.com/jtv/libpqxx.git

docker build -t base_image:latest -f base.Dockerfile .

rm -rf DPP spdlog googletest libpqxx

