library(ggplot2)
# ecdf example

P <- ecdf(c(0,1,1,1,2,2,2,3))

str(P)

plot(P, main ="CDF F(X")

help(pnorm)

help(seq)

x <- seq(20, 29.9, 0.1)
y <- pnorm(x, 33.2, 6.1)

c(1,2,23)
pnorm(29.9, mean=33.2, sd = 6.1) - pnorm(20, mean=33.2, sd = 6.1)


avg <- function(x) {
  sample.means <- numeric()
  for (i in 1:x) {
    sample.means <- append(sample.means, mean(sample(population.weights, 10, replace=T)))
  }
  mean(sample.means)
}


# population of weights taken from normal distribution
population.weights <-rnorm(10000, 65, 10)
# sample from population
my_means <- sapply(1:1000, avg)

df <- data.frame(attempt = 1:1000, observed_average = my_means )
hist(df$observed_average , breaks = seq(50, 80, by = 1), main = "1000 samples of 10 weights")
#qplot(df$attempt, df$observed_average, geom='line')
p5 <- ggplot(df, aes(x = attempt, y = observed_average))
p5 + geom_line() + expand_limits(y=c(55, 75))

draws = sample(population.weights, size = 10 * 10 , replace = TRUE)
# this populates matrix by column - ideal to draw histogram 
draws = matrix(draws, 10)
drawmeans = apply(draws, 2, mean)
length(drawmeans)
hist(drawmeans, breaks = seq(50, 80, by = 1), main = "test")

draws.2 = sample(population.weights, size = 10 * 1000 , replace = TRUE)
# this populates matrix by row so each row is effectively a sample of 10 random weights
draws.matrix = matrix(draws.2, 10, byrow=FALSE)
nrow(draws.matrix) # 10 rows
ncol(draws.matrix) # 1000 columns
# calculate column mean - so essentially mean of every 10 weights picked in those 10 * 1000 samples.  
drawmeans.2 = apply(draws.matrix, 2, mean)
length(drawmeans.2) # vector of 1000 means, 
hist(drawmeans.2, breaks = seq(50, 80, by = 1), main = "1000 averages of 10 weight draws")

df2 <- data.frame(attempt=1:1000, observed_average = drawmeans.2)
p5 <- ggplot(df2, aes(x = attempt, y = observed_average))
p5 + geom_line() + expand_limits(y=c(55, 75))
