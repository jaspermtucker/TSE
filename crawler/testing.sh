# Testing file for crawler
#
# Jasper Tucker, April 2023

mkdir ../data
mkdir ../data/letters
mkdir ../data/scrape
mkdir ../data/wiki

# too few arguments
./crawler

# too many arguments 
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 10 99

# invalid URL
./crawler http://cs50tse.cs.darts/index.html ../data/letters 10

# non-internal URL
./crawler http://republicofletters.stanford.edu/ ../data/letters 5

# invalid directory
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../myletters 6

# depth below range
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters -1

# depth above range
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 36

# crawler on small example, small depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/scrape 1

# crawler on small example, max depth, with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 10

# crawler on large example, small depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wiki 0

# large example, large depth is omitted for space