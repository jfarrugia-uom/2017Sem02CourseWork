# get exactly k heads
exactly.k.heads <- function(k, n) {
  choose(n, k)/2^n
}

sum(sapply(14:20, exactly.k.heads, 20))

toss <- function(x) {
  length(which(sample(0:1, 20, replace=TRUE)  == 1))
}

length(which(sapply(1:10000, toss) >= 15)) / 10000

owl <- c(72, 68, 75, 84, 61, 78)
sheep <- c(64, 68, 78, 81, 59, 62, 60, 48, 82, 77)

mean(owl)
mean(sheep)
length(sheep)
length(owl)

sd(owl)
sd(sheep)

mix.class <- c(owl, sheep)
length(mix.class)
comb.shuffle <- sample(mix.class)

new.owl <- comb.shuffle[1:length(owl)]
new.sheep <- comb.shuffle[length(owl)+1:length(comb.shuffle)]




