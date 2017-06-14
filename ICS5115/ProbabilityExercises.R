install.packages("plotly", dependencies = TRUE)
install.packages("gtools", dependencies = TRUE)


# Exercise 1: Compute probability space difference between R-J-R and J-R-J.
library(plotly)

# p is prob of beating Roger; p will be less than q which is a given
# q is prob of beating Jabba
p <- seq(0,1,0.05) / 2
q <- seq(0,1,0.05)

# given p < q
find.probability <- function(p, q) {
  ifelse(p < q, (q*p*q) - (p*q*p), NA)
}

#find.probability(1, 0.5)
# name vectors
names(p) <- p
names(q) <- q
# calculate probability matrix
prob.space <- outer(p, q, find.probability)
# plot 3-D surface plot
plot_p <- plot_ly(x = p, y = q, z = ~prob.space) %>% add_surface()

# p(I beat Roger) = 0.5
# p(I beat Jabba) = 1.0

# simulation of 2-diethrow; probability of drawing a 6 (either die) given sum is 10
# throw 1st die 10000 times
die.1 <- sample(1:6, 10000, replace=T )
# throw 2nd die 10000 times
die.2 <- sample(1:6, 10000, replace=T)
# add outcomes of both die throws
both <- die.1 + die.2
# select those which add up to 10
draw.10 <- which(both == 10)
number.10.draws <- length(draw.10)
# from tries resulting in 10, count the times die 1 came up 6; repeat for die 2
die.1.reading.6 <- length(die.1[draw.10][die.1[draw.10]==6])
die.2.reading.6 <- length(die.2[draw.10][die.2[draw.10]==6])
# add attempts where die 1 came up 6 and attempts where die 2 came up 6; divide by times sum of 10 came up
prob <- (die.1.reading.6 + die.2.reading.6) / number.10.draws
prob


no.doors <- 1000
attempts <- 10000
# project 10000 doors and select at random car behind a door
car.behind.door <- sample(c(1:no.doors), attempts, replace=T)
# project 10000 attempts at guessing a door
my.choice <- sample(c(1:no.doors), attempts, replace = T)
# compute boolean vector where door concealing car is equal to randomly picked door
corr.sel <- car.behind.door == my.choice
# compute probability by dividing the number of TRUE elements in vector over number of attempts
length(which(corr.sel%in%TRUE)) / attempts
# answer ~ 0.3333

# in this implementation, Monty will open a door, and I will flip
# if wrong door chosen flip will choose correct door since Monty exhausted all wrong options except my original one; 
# otherwise pick random door
monty.flip <- function(x, y) {
  ifelse(x == y, sample(setdiff(c(1:no.doors), c(x)), 1, replace=T), x)
}
# vector of Monty's opened door per attempt
flip.doors <- mapply(monty.flip, car.behind.door, my.choice)

flip.sel <- car.behind.door == flip.doors
# compute probability by dividing the number of TRUE elements in vector over number of attempts
length(which(flip.sel%in%TRUE)) / attempts


# calculate permutations of APPLE
# n = A, P, P, L ,E 

foo <- c('a','p', 'p','l', 'e')
sample(foo, 5, TRUE)

library(gtools)

unique(permutations(5, 5, c('a', 'p', 'p', 'l', 'e'), repeats.allowed = FALSE, set=FALSE))


choose(20, 2)

perm = function(n, x) {
  return(factorial(n) / factorial(n-x))
}

comb = function(n, x) {
  return(factorial(n) / (factorial(x) * factorial(n-x)))
}

