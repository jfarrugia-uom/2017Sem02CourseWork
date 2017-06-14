#install.packages("ggplot2")
library("ggplot2")

s <- 1:10
plot(s, main="Test Plot")

# what is this object
str(mtcars)

# typeof
typeof(mtcars)
# get data frame row names
rownames(mtcars)
colnames(mtcars)

# backup par
old.par <- par()
# to reload from backup
par(old.par)

plot(mtcars$hp, main = "Horse Power")

boxplot(mtcars$hp, main="Horse Power", ylab="MPG")$out

mtcars[mtcars$hp %in% boxplot(mtcars$hp, main="Horse Power", ylab="MPG")$out,]

help(boxplot)

faithful$eruptions

# histogram 
hist(faithful$eruptions, breaks=seq(1.6, 5.2, 0.2), prob=T, col = gray(0.95), xlab="Eruptions")
hist(faithful$eruptions, breaks=seq(1.6, 5.2, 0.2), prob=F, col = gray(0.95), xlab="Eruptions")

s <- rnorm(n = 1000, mean = 0, sd = 1)
hist(s, breaks=seq(-5, 5, 0.1), prob=T, col = gray(0.95), xlab="normal")
min(s)
max(s)

hist(s, breaks=seq(-5, 5, 0.1), prob=T, col = gray(0.95), xlab="normal")

lines(density(faithful$eruptions, bw=0.1))