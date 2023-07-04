# Testing file for indexer
#
# Jasper Tucker, May 2023

# file assumes that common and indexer have been built

mkdir ../data
mkdir ../data/fakeOutput

touch ../data/index
chmod +w ../data/index
touch ../data/fakeIndex
chmod 000 ../data/fakeIndex
touch ../data/secondIndex
chmod +w ../data/secondIndex
touch ../data/fakeSecondIndex
chmod 000 ../data/fakeSecondIndex

# too few arguments
./indexer

# too many arguments 
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index ../data

# invalid page directory
./indexer ../data/myOutput ../data/index

# non-crawler page directory
./indexer ../data/fakeOutput ../data/index

# non-existent index file, indexer creates one
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/myIndex

# non-writable index file
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/fakeIndex

# indexer as normal, with indextest
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index
./indextest ../data/index ../data/secondIndex
~/cs50-dev/shared/tse/indexcmp ../data/index ../data/secondIndex

# indexer as normal, with valgrind and indextest (also with valgrind)
valgrind --leak-check=full --show-leak-kinds=all ./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index
valgrind --leak-check=full --show-leak-kinds=all ./indextest ../data/index ../data/secondIndex
~/cs50-dev/shared/tse/indexcmp ../data/index ../data/secondIndex

# non-readable index file for indextest
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index
./indextest ../data/fakeIndex ../data/secondIndex

# non-writable index file for indextest
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index
./indextest ../data/index ../data/fakeSecondIndex

# too few arguments for indextest
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index
./indextest

# too many arguments for indextest
./indexer ~/cs50-dev/shared/tse/crawldata/toscrape-1 ../data/index
./indextest ../data/fakeIndex ../data/secondIndex ../data

# -- end --