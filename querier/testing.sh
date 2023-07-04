# Testing file for querier
#
# Jasper Tucker, May 2023

# file assumes that common, querier, and fuzzquery have been built

mkdir ../data
mkdir ../data/fakedir
mkdir ../data/fakeIndex
chmod 000 ../data/fakeIndex

# too few arguments
./querier 

# too many arguments
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index ~/cs50-dev/shared/tse/output/toscrape-2

# invalid page directory
./querier ~/cs50-dev/shared/tse/output/toscrape-9 ~/cs50-dev/shared/tse/output/toscrape-2.index

# non-crawler produced page directory
./querier ../data/fakedir ~/cs50-dev/shared/tse/output/toscrape-2.index

# non-readable index file
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ../data/fakeIndex

# normal arguments, query with numbers
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qnumbers.txt

# normal arguments, query with special characters
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qspecialchars.txt

# normal arguments, query with no matches
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qnomatches.txt

# normal arguments, query with one word
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/q1word.txt

# normal arguments, query using 'and'
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qand.txt

# normal arguments, query with two words, 'and' is implied
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qimpliedand.txt

# normal arguments, query using 'or'
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qor.txt

# normal arguments, query with a longer statement, mix of 'and' and 'or'
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qandor.txt

# normal arguments, query with a longer statement and capitalization
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qandorcaps.txt

# normal arguments, query includes misspelled words
./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qmisspelled.txt

# normal arguments, piped from fuzzquery
./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 0 | ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index

# normal arguments, run with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index < testingfiles/qandor.txt

